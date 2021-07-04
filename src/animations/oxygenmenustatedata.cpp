/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenmenustatedata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <cassert>
#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    const int MenuStateData::_timeOut = 50;
    void MenuStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::MenuStateData::connect - "
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        _target = widget;

        // save paddings
        if( GTK_IS_MENU( widget ) )
        {
            gtk_widget_style_get( _target,
                "vertical-padding", &_yPadding,
                "horizontal-padding", &_xPadding,
                NULL );
        }

        // this accounts for x/y thickness.
        // needs to retrieve it from widget
        _xPadding += gtk_widget_get_style( widget )->xthickness;
        _yPadding += gtk_widget_get_style( widget )->ythickness;

        // connect signals
        _motionId.connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), this );
        _leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );

        // connect timeLines
        _current._timeLine.connect( (GSourceFunc)delayedUpdate, this );
        _previous._timeLine.connect( (GSourceFunc)delayedUpdate, this );

        // set directions
        _current._timeLine.setDirection( TimeLine::Forward );
        _previous._timeLine.setDirection( TimeLine::Backward );

        // follow mouse animation
        FollowMouseData::connect( (GSourceFunc)followMouseUpdate, this );

    }

    //________________________________________________________________________________
    void MenuStateData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::MenuStateData::disconnect - "
            << " " << _target << " (" << (_target ? G_OBJECT_TYPE_NAME( _target ) : "0x0") << ")"
            << std::endl;
        #endif

        _target = 0L;

        // disconnect signal
        _motionId.disconnect();
        _leaveId.disconnect();

        // disconnect timelines
        _current._timeLine.disconnect();
        _previous._timeLine.disconnect();
        _timer.stop();

        // disconnect all children
        for( ChildrenMap::iterator iter = _children.begin(); iter != _children.end(); ++iter )
        { iter->second.disconnect(); }

        _children.clear();

        // base class
        FollowMouseData::disconnect();

   }

    //________________________________________________________________________________
    void MenuStateData::registerChild( GtkWidget* widget )
    {
        if( widget && _children.find( widget ) == _children.end() )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::MenuStateData::registerChild -"
                << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
            #endif

            Signal destroyId;
            destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            _children.insert( std::make_pair( widget, destroyId ) );
        }

    }

    //________________________________________________________________________________
    void MenuStateData::unregisterChild( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::MenuStateData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        ChildrenMap::iterator iter( _children.find( widget ) );

        // erase from children map
        if( iter != _children.end() )
        {
            iter->second.disconnect();
            _children.erase( iter );
        }

        // reset corresponding data, if matches
        if( widget == _previous._widget )
        {
            _previous._widget = 0L;
            _previous._timeLine.disconnect();
        }

        if( widget == _current._widget )
        {
            _current._widget = 0L;
            _current._timeLine.disconnect();
        }

    }

    //________________________________________________________________________________
    void MenuStateData::updateItems( void )
    {

        if( !_target ) return;

        gint xPointer, yPointer;
        gdk_window_get_pointer( gtk_widget_get_window( _target ), &xPointer, &yPointer, 0L );

        GdkWindow* window( gtk_widget_get_window( _target ) );
        GdkWindow* childWindow( 0L );

        // reset offset
        int xOffset(0);
        int yOffset(0);

        bool delayed( false );
        bool activeFound( false );
        GList *children( gtk_container_get_children( GTK_CONTAINER( _target ) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;

            GtkWidget* childWidget( GTK_WIDGET( child->data ) );
            registerChild( childWidget );
            const GtkStateType state( gtk_widget_get_state( childWidget ) );

            // do nothing for disabled child
            const bool active( state != GTK_STATE_INSENSITIVE && !GTK_IS_SEPARATOR_MENU_ITEM( childWidget ) );

            // update offsets
            if( childWindow != gtk_widget_get_window( childWidget ) )
            {

                childWindow = gtk_widget_get_window( childWidget );
                Gtk::gdk_window_translate_origin( window, childWindow, &xOffset, &yOffset );

            }

            // get allocation and add offsets
            GtkAllocation allocation( Gtk::gtk_widget_get_allocation( childWidget ) );
            allocation.x += xOffset;
            allocation.y += yOffset;

            if( Gtk::gdk_rectangle_contains( &allocation, xPointer, yPointer ) )
            {

                if( active )
                {

                    activeFound = true;
                    if( state != GTK_STATE_PRELIGHT )
                    { updateState( childWidget, Gtk::gtk_widget_get_allocation( childWidget ), xOffset, yOffset, true ); }

                } else delayed = true;

                break;

            }

        }

        if( children ) g_list_free( children );

        // fade-out current
        if( _current.isValid() && !activeFound && !menuItemIsActive( _current._widget ) )
        { updateState( _current._widget, _current._rect, _current._xOffset, _current._yOffset, false, delayed ); }

        return;

    }

    //________________________________________________________________________________
    bool MenuStateData::menuItemIsActive( GtkWidget* widget ) const
    {

        // check argument
        if( !GTK_IS_MENU_ITEM( widget ) ) return false;

        // check menu
        GtkWidget* menu( gtk_menu_item_get_submenu( GTK_MENU_ITEM( widget ) ) );
        if( !GTK_IS_MENU( menu ) ) return false;

        GtkWidget* topLevel( gtk_widget_get_toplevel( menu ) );
        if( !topLevel ) return false;

        return
            GTK_WIDGET_VISIBLE( menu ) &&
            GTK_WIDGET_REALIZED( topLevel ) &&
            GTK_WIDGET_VISIBLE( topLevel );
    }

    //________________________________________________________________________________
    bool MenuStateData::updateState( GtkWidget* widget, const GdkRectangle& rect, int xOffset, int yOffset, bool state, bool delayed )
    {

        if( state && widget != _current._widget )
        {

            // stop timer
            if( _timer.isRunning() ) _timer.stop();

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _current.isValid() )
            {
                if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

                if( _previous.isValid() )
                {
                    _dirtyRect = _previous._rect;
                    _dirtyRect.x += _previous._xOffset;
                    _dirtyRect.y += _previous._yOffset;
                }

                // move current to previous
                _previous.copy( _current );
            }

            // assign new widget to current and start animation
            const bool animate( !_current.isValid() );
            const GdkRectangle startRect( _current._rect );
            const int startOffset( _current._yOffset );
            _current.update( widget, rect, xOffset, yOffset );

            if( _current.isValid() )
            {
                if( animate ) _current._timeLine.start();
                else if( followMouse() && (startOffset == _current._yOffset ) ) startAnimation( startRect, _current._rect );
                else delayedUpdate( this );
            }

            return true;

        } else if( (!state) && widget == _current._widget ) {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

            if( _previous.isValid() )
            {
                _dirtyRect = _previous._rect;
                _dirtyRect.x += _previous._xOffset;
                _dirtyRect.y += _previous._yOffset;
            }

            // move current to previous; clear current, and animate
            if( followMouse() && delayed ) {

                if( !_timer.isRunning() )
                { _timer.start( _timeOut, (GSourceFunc)delayedAnimate, this ); }

            } else {

                if( _timer.isRunning() ) _timer.stop();
                _previous.copy( _current );
                _current.clear();
                if( _previous.isValid() && gtk_widget_get_state( _previous._widget ) == GTK_STATE_PRELIGHT )
                { _previous._timeLine.start(); }

            }

            return true;

        } else return false;

    }

    //_____________________________________________
    GdkRectangle MenuStateData::dirtyRect( void )
    {

        GdkRectangle rect( Gtk::gdk_rectangle() );
        const GdkRectangle previousRect( _previous.dirtyRect() );
        const GdkRectangle currentRect( _current.dirtyRect() );
        Gtk::gdk_rectangle_union( &previousRect, &currentRect, &rect );

        // add _dirtyRect
        if( Gtk::gdk_rectangle_is_valid( &_dirtyRect ) )
        {
            Gtk::gdk_rectangle_union( &_dirtyRect, &rect, &rect );
            _dirtyRect = Gtk::gdk_rectangle();
        }

        // add followMouse dirtyRect
        if( followMouse() )
        {

            // retrieve dirty rect and add relevant offsets
            GdkRectangle followMouseRect( FollowMouseData::dirtyRect() );
            if( Gtk::gdk_rectangle_is_valid( &_current._rect ) )
            {

                followMouseRect.x += _current._xOffset;
                followMouseRect.y += _current._yOffset;

            } else if( Gtk::gdk_rectangle_is_valid( &_previous._rect ) ) {

                followMouseRect.x += _previous._xOffset;
                followMouseRect.y += _previous._yOffset;

            } else if( Gtk::gdk_rectangle_is_valid( &followMouseRect ) && _target ) {

                // no valid offset found. Add full allocation
                followMouseRect = Gtk::gtk_widget_get_allocation( _target );
                followMouseRect.x += _xPadding;
                followMouseRect.y += _yPadding;
                followMouseRect.width -= 2*_xPadding;
                followMouseRect.height -= 2*_yPadding;

            }

            Gtk::gdk_rectangle_union( &followMouseRect, &rect, &rect );
        }

        // extend rect by some arbitrary number to prevent glitches
        if( Gtk::gdk_rectangle_is_valid( &rect ) ) rect.height += 1;

        return rect;

    }

    //____________________________________________________________________________________________
    gboolean MenuStateData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::MenuStateData::childDestroyNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<MenuStateData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean MenuStateData::motionNotifyEvent(GtkWidget*, GdkEventMotion*, gpointer pointer )
    {
        static_cast<MenuStateData*>( pointer )->updateItems();
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean MenuStateData::leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer pointer )
    {
        static_cast<MenuStateData*>( pointer )->updateItems();
        return FALSE;
    }

    //_____________________________________________
    gboolean MenuStateData::delayedUpdate( gpointer pointer )
    {

        MenuStateData& data( *static_cast<MenuStateData*>( pointer ) );

        if( data._target )
        {
            const GdkRectangle rect( data.dirtyRect() );
            Gtk::gtk_widget_queue_draw( data._target, &rect );
        }

        return FALSE;

    }

    //_____________________________________________
    gboolean MenuStateData::followMouseUpdate( gpointer pointer )
    {

        MenuStateData& data( *static_cast<MenuStateData*>( pointer ) );

        if( data._target && data.followMouse() )
        {

            data.updateAnimatedRect();
            const GdkRectangle rect( data.dirtyRect() );
            Gtk::gtk_widget_queue_draw( data._target, &rect );

        }

        return FALSE;

    }

    //_____________________________________________
    gboolean MenuStateData::delayedAnimate( gpointer pointer )
    {

        MenuStateData& data( *static_cast<MenuStateData*>( pointer ) );
        data._previous.copy( data._current );
        data._current.clear();

        if( data._previous.isValid() )
        { data._previous._timeLine.start(); }

        return FALSE;

    }

}
