/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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

#include "oxygenhook.h"
#include "../config.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //__________________________________________________________________
    bool Hook::connect( const std::string& signal, GType typeId, GSignalEmissionHook hookFunction, gpointer data )
    {
        // make sure that signal is not already connected
        assert( _signalId == 0 && _hookId == 0 );

        // check type id
        if( !g_type_class_peek( typeId ) )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::Hook::connect - typeId " << g_type_name(typeId) << " not yet installed" << std::endl;
            #endif

            return false;

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

        return true;

    }

    //____________________________________________________________________
    void Hook::disconnect( void )
    {

        // disconnect signal
        if( _signalId > 0 && _hookId > 0 ) g_signal_remove_emission_hook( _signalId, _hookId );
        _signalId = 0;
        _hookId = 0;

    }

}
