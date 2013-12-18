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
