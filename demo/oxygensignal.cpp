
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignal.h"

namespace Oxygen
{

    //____________________________________________________________________
    void Signal::connect( GObject* object, const std::string& signal, GCallback callback, gpointer data )
    {
        // make sure that signal is not already connected
        assert( _object == 0L && _id == 0 );

        // store attributes and create connection
        _object = object;
        _id = g_signal_connect( object, signal.c_str(), callback, data );
    }

    //____________________________________________________________________
    void Signal::disconnect( void )
    {

        // disconnect signal
        if( _object && _id > 0 ) g_signal_handler_disconnect( _object, _id );

        // reset members
        _object = 0L;
        _id = 0;

    }

}
