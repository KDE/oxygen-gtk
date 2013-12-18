/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenwidgetstatedata.h"
#include "../config.h"

#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    void WidgetStateData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WidgetStateData::connect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _target = widget;
        _timeLine.connect( (GSourceFunc)delayedUpdate, this );
    }

    //_____________________________________________
    void WidgetStateData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WidgetStateData::disconnect - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _timeLine.disconnect();
        _target = 0L;
        _state = false;
    }

    //_____________________________________________
    bool WidgetStateData::updateState( bool state, const GdkRectangle& rect )
    {

        // always update dirty rect
        _dirtyRect = rect;

        // check state and update
        if( state == _state ) return false;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::WidgetStateData::updateState - "
            << _target << " (" << ( _target ? std::string( G_OBJECT_TYPE_NAME( _target ) ) : "none" ) << ")"
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
    gboolean WidgetStateData::delayedUpdate( gpointer pointer )
    {

        WidgetStateData& data( *static_cast<WidgetStateData*>( pointer ) );

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::WidgetStateData::updateState - "
            << data._target << " (" << G_OBJECT_TYPE_NAME( data._target ) << ")"
            << " state: " << data._state
            << " rect: " << data._dirtyRect
            << " opacity: " << data._timeLine.value()
            << std::endl;
        #endif

        if( data._target && gtk_widget_get_realized(data._target) )
        { Gtk::gtk_widget_queue_draw( data._target, &data._dirtyRect ); }

        return FALSE;

    }

}
