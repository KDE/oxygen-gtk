/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygentimer.h"

#include <gdk/gdk.h>
#include <iostream>

namespace Oxygen
{

    //____________________________________________________
    void Timer::start( int delay, GSourceFunc func, gpointer data )
    {

        // make sure timer is not already running
        g_return_if_fail( _timerId == 0 );

        _func = func;
        _data = data;
        _timerId = gdk_threads_add_timeout( delay, (GSourceFunc)timeOut, this );

    }

    //____________________________________________________
    gboolean Timer::timeOut( gpointer data )
    {

        // cast to timer, and execute relevant function
        Timer& timer( *static_cast<Timer*>( data ) );
        gboolean result = (timer._func)( timer._data );

        // make sure timerId is properly reset if the embedded function returns false
        if( !result ) timer.reset();
        return result;

    }

}
