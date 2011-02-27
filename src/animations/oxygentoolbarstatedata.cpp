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
    void ToolBarStateData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ToolBarStateData::disconnect - "
            << " " << _target << " (" << G_OBJECT_TYPE_NAME( _target ) << ")"
            << std::endl;
        #endif

        _target = 0L;

        // disconnect timelines
        _current._timeLine.disconnect();
        _previous._timeLine.disconnect();
        _timeLine.disconnect();
        _timer.stop();

        // disconnect all children
        for( HoverDataMap::iterator iter = _hoverData.begin(); iter != _hoverData.end(); ++iter )
        { iter->second.disconnect(); }

        _hoverData.clear();

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
            data._styleChangeId.connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
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

        const GdkRectangle rect( widget ? widget->allocation : Gtk::gdk_rectangle() );

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
                if( animate || !_followMouse ) _current._timeLine.start();
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
            if( _followMouse && delayed ) {

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
    GdkRectangle ToolBarStateData::dirtyRect( void )
    {

        GdkRectangle rect( Gtk::gdk_rectangle() );

        const GdkRectangle previousRect( _previous._rect );
        const GdkRectangle currentRect( _current._rect );

        if( Gtk::gdk_rectangle_is_valid( &previousRect ) && Gtk::gdk_rectangle_is_valid( &currentRect ) )
        {

            gdk_rectangle_union( &previousRect, &currentRect, &rect );

        } else if( Gtk::gdk_rectangle_is_valid( &previousRect ) ) {

            rect = previousRect;

        } else if( Gtk::gdk_rectangle_is_valid( &currentRect ) ) {

            rect = currentRect;

        }

        // also union with dirty rect
        if( Gtk::gdk_rectangle_is_valid( &_dirtyRect ) )
        {
            if( Gtk::gdk_rectangle_is_valid( &rect ) ) gdk_rectangle_union( &_dirtyRect, &rect, &rect );
            else rect = _dirtyRect;

            _dirtyRect = Gtk::gdk_rectangle();

        }

        /* Add extra margin to rect. FIXME: this should be triggered from widget's metrics */
        if( Gtk::gdk_rectangle_is_valid( &rect ) )
        { rect.x -= 2; rect.y -= 2; rect.width += 4; rect.height += 4; }

        return rect;

    }

    //________________________________________________________________________________
    void ToolBarStateData::startAnimation( const GdkRectangle& startRect, const GdkRectangle& endRect )
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
    void ToolBarStateData::updateAnimatedRect( void )
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

    //____________________________________________________________________________________________
    void ToolBarStateData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<ToolBarStateData*>(data)->unregisterChild( widget ); }

        //________________________________________________________________________________
    gboolean ToolBarStateData::childEnterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ToolBarStateData::childEnterNotifyEvent -"
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
        std::cerr << "Oxygen::ToolBarStateData::childLeaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ToolBarStateData*>( data )->updateState( widget, false, true );
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

        if( data._target && data._followMouse )
        {

            data.updateAnimatedRect();

            // TODO: implement dedicated dirtyRect
            Gtk::gtk_widget_queue_draw( data._target );

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
