/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenhook.h"
#include "../config.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    #if OXYGEN_DEBUG
    static int counter( 0 );
    #endif

    //__________________________________________________________________
    bool Hook::connect( const std::string& signal, GType typeId, GSignalEmissionHook hookFunction, gpointer data )
    {
#if DISABLE_SIGNAL_HOOKS
        return false;
#endif
        // make sure that signal is not already connected
        assert( _signalId == 0 && _hookId == 0 );

        // check type id
        if( !g_type_class_peek( typeId ) )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::Hook::connect - typeId " << g_type_name(typeId) << " not yet installed" << std::endl;
            #endif

            g_type_class_ref( typeId );

        }

        // store signal id
        _signalId = g_signal_lookup( signal.c_str(), typeId );
        if( !_signalId )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::Hook::connect - signal " << signal << " not installed." << std::endl;
            #endif

            return false;

        }

        // store attributes and create connection
        _hookId = g_signal_add_emission_hook(
            _signalId,
            (GQuark)0L,
            hookFunction,
            data, 0L);

        #if OXYGEN_DEBUG
        ++counter;
        std::cerr << "Oxygen::Hook::connect - hook: " << _hookId << " counter: " << counter << std::endl;
        #endif

        return true;

    }

    //____________________________________________________________________
    void Hook::disconnect( void )
    {
#if DISABLE_SIGNAL_HOOKS
        return;
#endif

        // disconnect signal
        if( _signalId > 0 && _hookId > 0 )
        {

            #if OXYGEN_DEBUG
            --counter;
            std::cerr << "Oxygen::Hook::disconnect - hook: " << _hookId << " counter: " << counter << std::endl;
            #endif

            g_signal_remove_emission_hook( _signalId, _hookId );

        }

        _signalId = 0;
        _hookId = 0;

    }

}
