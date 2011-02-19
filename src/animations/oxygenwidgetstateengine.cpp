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

#include "oxygenwidgetstateengine.h"

namespace Oxygen
{

    //________________________________________________________
    bool WidgetStateEngine::registerWidget( GtkWidget* widget, AnimationModes modes, const StyleOptions& options )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::WidgetStateEngine::registerWidget - "
            << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " mode: " << modes
            << std::endl;
        #endif

        bool registered( false );
        if( (modes&AnimationHover) && registerWidget( widget, _hoverData, (options&Hover)&&!(options&Disabled) ) ) registered = true;
        if( (modes&AnimationFocus) && registerWidget( widget, _focusData, (options&Focus)&&!(options&Disabled) ) ) registered = true;

        if( registered )
        { BaseEngine::registerWidget( widget ); }

        return registered;

    }

    //________________________________________________________
    bool WidgetStateEngine::registerWidget( GtkWidget* widget, DataMap<WidgetStateData>& dataMap, const bool& state ) const
    {

        if( dataMap.contains( widget ) ) return false;

        WidgetStateData& data( dataMap.registerWidget( widget ) );
        data.setEnabled( enabled() );
        data.updateState( state );
        data.setDuration( duration() );

        /*
        blacklist some applications based on name and widget
        the widget is effectively registered, but not connected
        */
        if( widgetIsBlackListed( widget ) ) return true;

        // connect
        if( enabled() ) data.connect( widget );

        return true;

    }

}


