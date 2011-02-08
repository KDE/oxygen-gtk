#ifndef oxygentimelineserver_h
#define oxygentimelineserver_h

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
