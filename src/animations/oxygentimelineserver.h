#ifndef oxygentimelineserver_h
#define oxygentimelineserver_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <glib.h>
#include <set>
namespace Oxygen
{


    //! forward declaration
    class TimeLine;

    //! keep track of timelines, triggers update at regular intervals, as long as running
    class TimeLineServer
    {

        public:

        //! singleton
        static TimeLineServer& instance( void );

        //! destructor
        virtual ~TimeLineServer( void );

        //! register timeline
        void registerTimeLine( TimeLine* timeLine )
        { _timeLines.insert( timeLine ); }

        //! unregister timeline
        void unregisterTimeLine( TimeLine* timeLine )
        { _timeLines.erase( timeLine ); }

        //! start timeout if needed
        void start( void );

        protected:

        //! stop timeout
        void stop( void );

        //! update registered timers
        static gboolean update( gpointer );

        private:

        //! constructor is private
        TimeLineServer( void );

        //! keeps track of all registered timelines
        typedef std::set< TimeLine* > TimeLineSet;
        TimeLineSet _timeLines;

        //! timer id
        int _timerId;

        //! singleton
        static TimeLineServer* _instance;




    };

}

#endif
