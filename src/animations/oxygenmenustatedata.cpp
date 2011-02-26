/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* MenuState prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc. <@redhat.com>
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
*
* the menushell data code is largely inspired from the gtk redmond engine
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/

#include "oxygenmenustatedata.h"
#include "../oxygengtkutils.h"

#include <cassert>
#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void MenuStateData::connect( GtkWidget* widget )
    {

        _target = widget;

        // save paddings
        if( GTK_IS_MENU( widget ) )
        {
            gtk_widget_style_get( _target,
                "vertical-padding", &_verticalPadding,
                "horizontal-padding", &_horizontalPadding,
                NULL );
        }

        _motionId.connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), this );
        _leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );

        // connect timeLines
        _current._timeLine.connect( (GSourceFunc)delayedUpdate, this );
        _previous._timeLine.connect( (GSourceFunc)delayedUpdate, this );

        // set directions
        _current._timeLine.setDirection( TimeLine::Forward );
        _previous._timeLine.setDirection( TimeLine::Backward );

        // follow mouse animation
        _timeLine.connect( (GSourceFunc)followMouseUpdate, this );
        _timeLine.setDirection( TimeLine::Forward );

    }

    //________________________________________________________________________________
    void MenuStateData::disconnect( GtkWidget* widget )
    {

        _target = 0L;

        // disconnect signal
        _motionId.disconnect();
        _leaveId.disconnect();

        // disconnect timelines
        _current._timeLine.disconnect();
        _previous._timeLine.disconnect();
        _timeLine.disconnect();
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

        bool activeFound( false );
        GtkWidget *activeWidget( 0L );
        GList *children( gtk_container_get_children( GTK_CONTAINER( _target ) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;

            GtkWidget* childWidget( GTK_WIDGET( child->data ) );
            const GtkStateType state( gtk_widget_get_state( childWidget ) );

            // do nothing for disabled child
            if( state == GTK_STATE_INSENSITIVE ) continue;
            //if( GTK_IS_SEPARATOR_MENU_ITEM( childWidget ) ) continue;

            // update offsets
            if( childWindow != gtk_widget_get_window( childWidget ) )
            {

                childWindow = gtk_widget_get_window( childWidget );
                Gtk::gdk_window_translate_origin( window, childWindow, &xOffset, &yOffset );

            }

            // get allocation and add offsets
            GtkAllocation allocation( childWidget->allocation );
            allocation.x += xOffset;
            allocation.y += yOffset;

            if( Gtk::gdk_rectangle_contains( &allocation, xPointer, yPointer ) )
            {

                activeFound = true;
                if( state != GTK_STATE_PRELIGHT )
                { updateState( childWidget, childWidget->allocation, xOffset, yOffset, true ); }

            } else if( state != GTK_STATE_NORMAL ) {

                activeWidget = childWidget;

            }
        }

        if( children ) g_list_free( children );

        // fade-out current
        if( _current.isValid() && !activeFound && !menuItemIsActive( _current._widget ) )
        { updateState( _current._widget, _current._rect, _current._xOffset, _current._yOffset, false ); }

        return;

    }

    //________________________________________________________________________________
    bool MenuStateData::menuItemIsActive( GtkWidget* widget ) const
    {

        // check argument
        if( !GTK_IS_MENU_ITEM( widget ) ) return false;

        // check menu
        GtkWidget* menu( GTK_MENU_ITEM( widget )->submenu );
        if( !GTK_IS_MENU( menu ) ) return false;

        GtkWidget* topLevel( gtk_widget_get_toplevel( menu ) );
        if( !topLevel ) return false;

        return
            GTK_WIDGET_VISIBLE( menu ) &&
            GTK_WIDGET_REALIZED( topLevel ) &&
            GTK_WIDGET_VISIBLE( topLevel );
    }

    //________________________________________________________________________________
    bool MenuStateData::updateState( GtkWidget* widget, const GdkRectangle& rect, int xOffset, int yOffset, bool state )
    {

        // do nothing if animations are disabled
        if( !_animationsEnabled ) return true;

        if( state && widget != _current._widget )
        {

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
            _current.update( widget, rect, xOffset, yOffset );
            if( _current.isValid() )
            {
                if( animate ) _current._timeLine.start();
                else if( _followMouse ) startAnimation( startRect, _current._rect );
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
            _previous.copy( _current );
            _current.clear();
            if( _previous.isValid() && gtk_widget_get_state( _previous._widget ) == GTK_STATE_PRELIGHT ) _previous._timeLine.start();

            return true;

        } else return false;

    }

    //_____________________________________________
    GdkRectangle MenuStateData::dirtyRect( void )
    {

        GdkRectangle rect( Gtk::gdk_rectangle() );
        GdkRectangle previousRect( _previous._rect );
        GdkRectangle currentRect( _current._rect );

        if( Gtk::gdk_rectangle_is_valid( &previousRect ) && Gtk::gdk_rectangle_is_valid( &currentRect ) )
        {

            previousRect.x += _previous._xOffset;
            previousRect.y += _previous._yOffset;

            currentRect.x += _current._xOffset;
            currentRect.y += _current._yOffset;

            gdk_rectangle_union( &previousRect, &currentRect, &rect );

        } else if( Gtk::gdk_rectangle_is_valid( &previousRect ) ) {

            previousRect.x += _previous._xOffset;
            previousRect.y += _previous._yOffset;

            rect = previousRect;

        } else if( Gtk::gdk_rectangle_is_valid( &currentRect ) ) {

            currentRect.x += _current._xOffset;
            currentRect.y += _current._yOffset;

            rect = currentRect;

        }

        // also union with dirty rect
        if( Gtk::gdk_rectangle_is_valid( &_dirtyRect ) )
        {

            if( Gtk::gdk_rectangle_is_valid( &rect ) ) gdk_rectangle_union( &_dirtyRect, &rect, &rect );
            else rect = _dirtyRect;

            _dirtyRect = Gtk::gdk_rectangle();

        }

        return rect;

    }

    //________________________________________________________________________________
    void MenuStateData::startAnimation( const GdkRectangle& startRect, const GdkRectangle& endRect )
    {

        // copy end rect
        _endRect = endRect;

        // check timeLine status
        if( _timeLine.isRunning() &&
            _timeLine.value() < 1.0 &&
            Gtk::gdk_rectangle_is_valid( &_endRect ) &&
            Gtk::gdk_rectangle_is_valid( &_animatedRect ) )
        {

            // do some math so that the animation finishes at new endRect without discontinuity
            const double ratio( _timeLine.value()/(1.0-_timeLine.value() ) );
            _startRect.x += double( _animatedRect.x - _endRect.x )*ratio;
            _startRect.y += double( _animatedRect.y - _endRect.y )*ratio;
            _startRect.width += double( _animatedRect.width - _endRect.width )*ratio;
            _startRect.height += double( _animatedRect.height - _endRect.height )*ratio;


        } else {

            if( _timeLine.isRunning() ) _timeLine.stop();
            _startRect = startRect;
            _timeLine.start();

        }

        return;

    }

    //________________________________________________________________________________
    void MenuStateData::updateAnimatedRect( void )
    {
        if( _timeLine.isRunning() &&
            Gtk::gdk_rectangle_is_valid( &_startRect ) &&
            Gtk::gdk_rectangle_is_valid( &_endRect ) )
        {

            _animatedRect.x = _startRect.x + double( _endRect.x - _startRect.x )*_timeLine.value();
            _animatedRect.y = _startRect.y + double( _endRect.y - _startRect.y )*_timeLine.value();
            _animatedRect.width = _startRect.width + double( _endRect.width - _startRect.width )*_timeLine.value();
            _animatedRect.height = _startRect.height + double( _endRect.height - _startRect.height )*_timeLine.value();

        } else {

            _animatedRect = Gtk::gdk_rectangle();

        }

        return;

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

        if( data._target && data._followMouse )
        {

            data.updateAnimatedRect();

            // TODO: implement dedicated dirtyRect
            GdkRectangle rect( data._target->allocation );
            rect.x += data._horizontalPadding;
            rect.y += data._verticalPadding;
            rect.width -= 2*data._horizontalPadding;
            rect.height -= 2*data._verticalPadding;

            Gtk::gtk_widget_queue_draw( data._target, &rect );

        }

        return FALSE;

    }

}
