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

#include "oxygenscrollbarstatedata.h"
#include "../config.h"

#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    void ScrollBarStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrollBarStateData::connect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _target = widget;
        _upArrowData._timeLine.connect( (GSourceFunc)delayedUpdate, this );
        _downArrowData._timeLine.connect( (GSourceFunc)delayedUpdate, this );
    }

    //_____________________________________________
    void ScrollBarStateData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrollBarStateData::disconnect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _upArrowData._timeLine.disconnect();
        _upArrowData._rect = Gtk::gdk_rectangle();
        _upArrowData._state = false;

        _downArrowData._timeLine.disconnect();
        _downArrowData._rect = Gtk::gdk_rectangle();
        _downArrowData._state = false;

        _target = 0L;

    }

    //_____________________________________________
    bool ScrollBarStateData::Data::updateState( bool state )
    {
        if( state == _state ) return false;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ScrollBarStateData::Data::updateState - "
            << " state: " << state
            << std::endl;
        #endif

        _state = state;

        // change direction
        _timeLine.setDirection( state ? TimeLine::Forward:TimeLine::Backward );

        // restart timeLine if needed
        if( _timeLine.isConnected() && !_timeLine.isRunning() ) _timeLine.start();

        return true;
    }

    //_____________________________________________
    gboolean ScrollBarStateData::delayedUpdate( gpointer pointer )
    {

        ScrollBarStateData& data( *static_cast<ScrollBarStateData*>( pointer ) );

        if( data._target )
        { gtk_widget_queue_draw( data._target ); }

        return FALSE;

    }

}
