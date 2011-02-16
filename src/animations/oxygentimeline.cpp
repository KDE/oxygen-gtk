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

#include "oxygentimeline.h"
#include "oxygentimelineserver.h"

#include <cassert>

namespace Oxygen
{

    //_________________________________________________
    TimeLine::TimeLine( int duration ):
        _duration( duration ),
        _enabled( true ),
        _direction( Forward ),
        _running( false ),
        _value( 0 ),
        _time( 0 ),
        _timer( g_timer_new() ),
        _func( 0L ),
        _data( 0L )
    { TimeLineServer::instance().registerTimeLine( this ); }

    //_________________________________________________
    TimeLine::TimeLine( const TimeLine& other ):
        _duration( other._duration ),
        _enabled( other._enabled ),
        _direction( other._direction ),
        _running( false ),
        _value( 0 ),
        _time( 0 ),
        _timer( g_timer_new() ),
        _func( other._func ),
        _data( other._data )
    { TimeLineServer::instance().registerTimeLine( this ); }

    //_________________________________________________
    TimeLine::~TimeLine( void )
    {
        if( _timer ) g_timer_destroy( _timer );
        TimeLineServer::instance().unregisterTimeLine( this );
    }

    //_________________________________________________
    TimeLine& TimeLine::operator = ( const TimeLine& other )
    {

        stop();

        _duration = other._duration;
        _enabled = other._enabled;
        _direction = other._direction;
        _value = 0;
        _time = 0;
        _func = other._func;
        _data = other._data;

        return *this;

    }

    //_________________________________________________
    void TimeLine::start( void )
    {

        // do nothing if disabled
        if( (!_enabled) || _duration <= 0 ) return;

        assert( !_running );

        _value = (_direction == Forward ) ? 0:1;
        _time = 0;
        g_timer_start( _timer );
        _running = true;

        TimeLineServer::instance().start();
        trigger();

    }

    //_________________________________________________
    void TimeLine::stop( void )
    {

        if( !_running ) return;

        g_timer_stop( _timer );
        _running = false;
    }

    //_________________________________________________
    bool TimeLine::update( void )
    {

        if( !_running ) return false;

        // get time (msec)
        const int elapsed( int(1000*g_timer_elapsed(_timer, 0L)) );
        const double end( _direction == Forward ? 1:0 );
        if( elapsed >= _duration )
        {

            _time = _duration;
            _value = end;
            trigger();
            stop();
            return false;

        } else {


            // for debugging only
            assert( _time < _duration );
            assert( _time <= elapsed );

            _value = ( _value*double(_duration - elapsed) + end*double(elapsed - _time) )/double(_duration - _time);
            _time = elapsed;
            trigger();
            return true;

        }

    }

}
