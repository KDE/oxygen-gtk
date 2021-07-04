/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygentimelineserver.h"
#include "oxygentimeline.h"
#include "../config.h"

#include <gdk/gdk.h>
#include <iostream>

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
    {
        #if OXYGEN_DEBUG
        std::cerr << "TimeLineServer::TimeLineServer." << std::endl;
        #endif
    }

    //____________________________________________________________________
    TimeLineServer::~TimeLineServer( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "TimeLineServer::~TimeLineServer." << std::endl;
        #endif

        // clear timer Id and singleton
        if( _timerId ) g_source_remove( _timerId );
        _instance = 0L;

    }

    //____________________________________________________________________
    void TimeLineServer::start( void )
    {

        if( !_timerId )
        { _timerId =  gdk_threads_add_timeout( updateInterval, (GSourceFunc)update, this ); }

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
