
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignal.h"
#include "../config.h"

#include <iostream>

namespace Oxygen
{

    #if OXYGEN_DEBUG
    static int counter( 0 );
    #endif

    //____________________________________________________________________
    bool Signal::connect( GObject* object, const std::string& signal, GCallback callback, gpointer data, bool after )
    {
        // make sure that signal is not already connected
        assert( _object == 0L && _id == 0 );

        // check object
        if( !object ) return false;

        // first try lookup signal
        if( !g_signal_lookup( signal.c_str(), G_OBJECT_TYPE(object) ) )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::Signal::connect - signal " << signal << " not installed on widget " << object << std::endl;
            #endif

            return false;

        }

        // store attributes and create connection
        _object = object;
        if(after) _id = g_signal_connect_after( object, signal.c_str(), callback, data );
        else _id = g_signal_connect( object, signal.c_str(), callback, data );

        #if OXYGEN_DEBUG
        ++counter;
        std::cerr << "Oxygen::Signal::connect - _id: " << _id << " counter: " << counter << std::endl;
        #endif

        return true;

    }

    //____________________________________________________________________
    void Signal::disconnect( void )
    {

        // disconnect signal
        if( _object && _id > 0 )
        {

            #if OXYGEN_DEBUG
            --counter;
            std::cerr << "Oxygen::Signal::disconnect - _id: " << _id << " counter: " << counter << std::endl;
            #endif

            g_signal_handler_disconnect( _object, _id );

        }

        // reset members
        _object = 0L;
        _id = 0;

    }

}
