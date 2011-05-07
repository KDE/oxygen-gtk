/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygentoolbarstatedata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    const int ToolBarStateData::_timeOut = 50;
    void ToolBarStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::connect - "
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        // store widget
        _target = widget;

        // connect signals
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
    void ToolBarStateData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::disconnect - "
            << " " << _target << " (" << (_target ? G_OBJECT_TYPE_NAME( _target ) : "0x0") << ")"
            << std::endl;
        #endif

        _target = 0L;

        // disconnect signal
        _leaveId.disconnect();

        // disconnect timelines
        _current._timeLine.disconnect();
        _previous._timeLine.disconnect();
        _timer.stop();

        // disconnect all children
        for( HoverDataMap::iterator iter = _hoverData.begin(); iter != _hoverData.end(); ++iter )
        { iter->second.disconnect(); }

        _hoverData.clear();

        FollowMouseData::disconnect();

    }

    //________________________________________________________________________________
    void ToolBarStateData::registerChild( GtkWidget* widget, bool value )
    {

        // make sure widget is not already in map
        if( _hoverData.find( widget ) == _hoverData.end() )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::ToolBarStateData::registerChild -"
                << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
            #endif

            // allocate new Hover data
            HoverData data;
            data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( childEnterNotifyEvent ), this );
            data._leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( childLeaveNotifyEvent ), this );

            // and insert in map
            _hoverData.insert( std::make_pair( widget, data ) );
            updateState( widget, value, false );


        }

    }

    //________________________________________________________________________________
    void ToolBarStateData::unregisterChild( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        // loopup in hover map
        HoverDataMap::iterator iter( _hoverData.find( widget ) );
        if( iter != _hoverData.end() )
        {
            iter->second.disconnect();
            _hoverData.erase( iter );
        }

    }

    //________________________________________________________________________________
    bool ToolBarStateData::updateState( GtkWidget* widget, bool state, bool delayed )
    {

        const GdkRectangle rect( widget ? Gtk::gtk_widget_get_allocation( widget ) : Gtk::gdk_rectangle() );

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
                { _dirtyRect = _previous._rect; }

                // move current to previous
                _previous.copy( _current );
            }

            // assign new widget to current and start animation
            const GdkRectangle startRect( _current._rect );
            const bool animate( !_current.isValid() );
            _current.update( widget, rect );
            if( _current.isValid() )
            {
                if( animate || !followMouse() ) _current._timeLine.start();
                else startAnimation( startRect, _current._rect );

            }

            return true;

        } else if( (!state) && widget == _current._widget ) {

            // stop current animation if running
            if( _current._timeLine.isRunning() ) _current._timeLine.stop();

            // stop previous animation if running
            if( _previous._timeLine.isRunning() ) _previous._timeLine.stop();

            if( _previous.isValid() )
            { _dirtyRect = _previous._rect; }

            // move current to previous; clear current, and animate
            if( followMouse() && delayed ) {

                // start delayed animation
                if( !_timer.isRunning() )
                { _timer.start( _timeOut, (GSourceFunc)delayedAnimate, this ); }

            } else {

                if( _timer.isRunning() ) _timer.stop();
                _previous.copy( _current );
                _current.clear();

                // start fade-out timeLine
                if( _previous.isValid() ) _previous._timeLine.start();

            }

            return true;

        } else return false;

    }

    //_____________________________________________
    GdkRectangle ToolBarStateData::dirtyRect( void )
    {

        GdkRectangle rect( Gtk::gdk_rectangle() );
        Gtk::gdk_rectangle_union( &_previous._rect, &_current._rect, &rect );

        // add _dirtyRect
        if( Gtk::gdk_rectangle_is_valid( &_dirtyRect ) )
        {
            Gtk::gdk_rectangle_union( &_dirtyRect, &rect, &rect );
            _dirtyRect = Gtk::gdk_rectangle();
        }

        // add followMouse dirtyRect
        if( followMouse() )
        {
            const GdkRectangle followMouseRect( FollowMouseData::dirtyRect() );
            Gtk::gdk_rectangle_union( &followMouseRect, &rect, &rect );
        }

        // check validity
        if( !Gtk::gdk_rectangle_is_valid( &rect ) ) return rect;

        /* Add viewport offsets if any */
        if( GTK_IS_VIEWPORT( _target ) )
        {
            gint xOffset, yOffset;
            Gtk::gtk_viewport_get_position( GTK_VIEWPORT( _target ), &xOffset, &yOffset );
            rect.x -= xOffset;
            rect.y -= yOffset;
        }

        /* Add extra margin to rect. FIXME: this should be triggered from widget's metrics */
        rect.x -= 2;
        rect.y -= 2;
        rect.width += 4;
        rect.height += 4;

        return rect;

    }

    //____________________________________________________________________________________________
    gboolean ToolBarStateData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::childDestroyNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<ToolBarStateData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ToolBarStateData::childEnterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::childEnterNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ToolBarStateData*>( data )->updateState( widget, true, false );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ToolBarStateData::childLeaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::childLeaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ToolBarStateData*>( data )->updateState( widget, false, true );
        return FALSE;

    }

    //________________________________________________________________________________
    gboolean ToolBarStateData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer pointer )
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::leaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        ToolBarStateData& data( *static_cast<ToolBarStateData*>( pointer ) );
        if( data._current.isValid() ) data.updateState( data._current._widget, false, false );
        return FALSE;
    }

    //_____________________________________________
    gboolean ToolBarStateData::delayedUpdate( gpointer pointer )
    {

        ToolBarStateData& data( *static_cast<ToolBarStateData*>( pointer ) );

        if( data._target )
        {
            const GdkRectangle rect( data.dirtyRect() );
            Gtk::gtk_widget_queue_draw( data._target, &rect );
        }

        return FALSE;

    }

    //_____________________________________________
    gboolean ToolBarStateData::followMouseUpdate( gpointer pointer )
    {

        ToolBarStateData& data( *static_cast<ToolBarStateData*>( pointer ) );

        if( data._target && data.followMouse() )
        {

            data.updateAnimatedRect();

            GdkRectangle rect( data.dirtyRect() );
            Gtk::gtk_widget_queue_draw( data._target, &rect );

        }

        return FALSE;

    }

    //_____________________________________________
    gboolean ToolBarStateData::delayedAnimate( gpointer pointer )
    {

        ToolBarStateData& data( *static_cast<ToolBarStateData*>( pointer ) );
        data._previous.copy( data._current );
        data._current.clear();

        if( data._previous.isValid() )
        { data._previous._timeLine.start(); }

        return FALSE;

    }
}
