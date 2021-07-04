/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
