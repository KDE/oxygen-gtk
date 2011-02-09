/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

namespace Oxygen
{

    //_____________________________________________
    const double WidgetStateData::OpacityInvalid = -1;

    //_____________________________________________
    WidgetStateData::WidgetStateData( void ):
        _target( 0L ),
        _timeLine( 500 ),
        _state( false )
    {}

    //_____________________________________________
    void WidgetStateData::connect( GtkWidget* widget )
    {
        _target = widget;
        _timeLine.connect( (GSourceFunc)delayedUpdate, this );
    }

    //_____________________________________________
    void WidgetStateData::disconnect( GtkWidget* )
    {
        _timeLine.disconnect();
        _target = 0L;
        _state = false;
    }

    //_____________________________________________
    bool WidgetStateData::updateState( bool state )
    {
        if( state == _state ) return false;
        _state = state;

        // change direction
        _timeLine.setDirection( state ? TimeLine::Forward:TimeLine::Backward );

        // restart timeLine if needed
        if( _target && !_timeLine.isRunning() ) _timeLine.start();

        return true;
    }

    //_____________________________________________
    gboolean WidgetStateData::delayedUpdate( gpointer pointer )
    {
        WidgetStateData& data( *static_cast<WidgetStateData*>( pointer ) );
        if( data._target )
        { gtk_widget_queue_draw( data._target ); }

        return FALSE;

    }

}
