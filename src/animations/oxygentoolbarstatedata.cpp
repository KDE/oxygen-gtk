/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* ToolBarState prelight effect is based on
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

#include "oxygentoolbarstatedata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
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
            data._enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
            data._leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );

            // and insert in map
            _hoverData.insert( std::make_pair( widget, data ) );
            updateState( widget, value );


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
    bool ToolBarStateData::updateState( GtkWidget* widget, bool state )
    {

        const GdkRectangle rect( widget ? widget->allocation : Gtk::gdk_rectangle() );

        if( state && widget != _current._widget )
        {

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
            const bool animate( !_current.isValid() );
            _current.update( widget, rect );
            if( _current.isValid() )
            {
                if( animate ) _current._timeLine.start();
                else delayedUpdate( this );
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
            _previous.copy( _current );
            _current.clear();
            if( _previous.isValid() && gtk_widget_get_state( _previous._widget ) == GTK_STATE_PRELIGHT ) _previous._timeLine.start();

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

    //____________________________________________________________________________________________
    void ToolBarStateData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<ToolBarStateData*>(data)->unregisterChild( widget ); }

        //________________________________________________________________________________
    gboolean ToolBarStateData::enterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ToolBarStateData::enterNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ToolBarStateData*>( data )->updateState( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ToolBarStateData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ToolBarStateData::leaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ToolBarStateData*>( data )->updateState( widget, false );
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

}
