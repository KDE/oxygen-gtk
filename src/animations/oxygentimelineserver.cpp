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

#include "oxygentimelineserver.h"
#include "oxygentimeline.h"

namespace Oxygen
{

    //! time interval between two updates (use 20 msec for now)
    static const int updateInterval = 20;

    //____________________________________________________________________
    TimeLineServer* TimeLineServer::_instance = 0L;
    TimeLineServer& TimeLineServer::instance( void )
    {
        if( !_instance )
        { _instance = new TimeLineServer(); }

        return *_instance;
    }

    //____________________________________________________________________
    TimeLineServer::TimeLineServer( void ):
        _timerId( 0 )
        {}

    //____________________________________________________________________
    TimeLineServer::~TimeLineServer( void )
    {

        // clear timer Id and singleton
        if( _timerId ) g_source_remove( _timerId );
        _instance = 0L;

    }

    //____________________________________________________________________
    void TimeLineServer::start( void )
    {

        if( !_timerId )
        { _timerId =  g_timeout_add( updateInterval, (GSourceFunc)update, this ); }

        return;

    }

    //____________________________________________________________________
    void TimeLineServer::stop( void )
    {

        if( _timerId )
        {
            g_source_remove( _timerId );
            _timerId = 0;
        }

    }

    //____________________________________________________________________
    gboolean TimeLineServer::update( gpointer data )
    {

        bool running( false );
        TimeLineServer& server( *static_cast<TimeLineServer*>( data ) );

        // loop over timelines
        for( TimeLineSet::const_iterator iter = server._timeLines.begin(); iter != server._timeLines.end(); ++iter )
        { if( (*iter)->update() ) running = true; }

        // stop timeout
        if( !running ) server.stop();

        // return true if at least one timeline is still running
        return gboolean( running );

    }

}
