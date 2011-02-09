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
    void WidgetStateEngine::unregisterWidget( GtkWidget* widget )
    {
        unregisterWidget( widget, _hoverData );
        unregisterWidget( widget, _focusData );
    }

    //________________________________________________________
    bool WidgetStateEngine::contains( GtkWidget* widget, AnimationMode mode )
    {
        switch( mode )
        {
            case AnimationHover: return _hoverData.contains( widget );
            case AnimationFocus: return _focusData.contains( widget );
            default: return false;
        }
    }

    //________________________________________________________
    bool WidgetStateEngine::isAnimated( GtkWidget* widget, AnimationMode mode )
    {
        switch( mode )
        {
            case AnimationHover:
            return _hoverData.value( widget ).timeLine().isRunning();

            case AnimationFocus:
            return _focusData.value( widget ).timeLine().isRunning();

            default: return false;
        }
    }

    //________________________________________________________
    double WidgetStateEngine::opacity( GtkWidget* widget, AnimationMode mode )
    {
        if( !isAnimated( widget, mode ) ) return OpacityInvalid;
        switch( mode )
        {
            case AnimationHover:
            return _hoverData.value( widget ).opacity();

            case AnimationFocus:
            return _focusData.value( widget ).opacity();

            default: return OpacityInvalid;

        }

    }

    //________________________________________________________
    bool WidgetStateEngine::updateState( GtkWidget* widget, AnimationMode mode, bool value )
    {


        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::WidgetStateEngine::updateState - "
            << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " mode: " << mode
            << " value: " << value
            << std::endl;
        #endif

        switch( mode )
        {
            case AnimationHover:
            return _hoverData.value( widget ).updateState( value );

            case AnimationFocus:
            return _focusData.value( widget ).updateState( value );

            default: return false;
        }

    }

    //________________________________________________________
    bool WidgetStateEngine::registerWidget( GtkWidget* widget, DataMap<WidgetStateData>& dataMap, const bool& state ) const
    {

        if( dataMap.contains( widget ) ) return false;

        WidgetStateData& data( dataMap.registerWidget( widget ) );
        data.updateState( state );
        data.timeLine().setDuration( _duration );

        /*
        blacklist some applications based on name and widget
        the widget is effectively registered, but not connected
        */
        if( widgetIsBlackListed( widget ) ) return true;

        // connect
        if( enabled() ) data.connect( widget );

        return true;

    }

    //________________________________________________________
    void WidgetStateEngine::unregisterWidget( GtkWidget* widget, DataMap<WidgetStateData>& data ) const
    {
        if( !data.contains( widget ) ) return;
        data.value( widget ).disconnect( widget );
        data.erase( widget );
    }

    //________________________________________________________________________
    void WidgetStateEngine::setDuration( int value )
    {

        if( _duration == value ) return;
        _duration = value;

        // hover data map
        for( DataMap<WidgetStateData>::Map::iterator iter = _hoverData.map().begin(); iter != _hoverData.map().end(); iter++ )
        { iter->second.timeLine().setDuration( value ); }

        // focus data map
        for( DataMap<WidgetStateData>::Map::iterator iter = _focusData.map().begin(); iter != _focusData.map().end(); iter++ )
        { iter->second.timeLine().setDuration( value ); }

    }

    //________________________________________________________________________
    void WidgetStateEngine::setEnabled( bool value )
    {

        if( enabled() == value ) return;
        BaseEngine::setEnabled( value );

        // hover data map
        for( DataMap<WidgetStateData>::Map::iterator iter = _hoverData.map().begin(); iter != _hoverData.map().end(); iter++ )
        {
            iter->second.timeLine().setEnabled( value );
            if( enabled() && !widgetIsBlackListed( iter->first ) ) iter->second.connect( iter->first );
            else iter->second.disconnect( iter->first );
        }

        // focus data map
        for( DataMap<WidgetStateData>::Map::iterator iter = _focusData.map().begin(); iter != _focusData.map().end(); iter++ )
        {
            iter->second.timeLine().setEnabled( value );
            if( enabled() && !widgetIsBlackListed( iter->first ) ) iter->second.connect( iter->first );
            else iter->second.disconnect( iter->first );
        }

    }

}


