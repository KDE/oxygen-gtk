#ifndef oxygentimer_h
#define oxygentimer_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <glib.h>

namespace Oxygen
{
    //! handles gtk timeouts
    /*! make sure the timer is properly reset at destruction */
    class Timer
    {

        public:

        //! constructor
        Timer( void ):
            _timerId( 0 ),
            _func( 0L ),
            _data( 0L )
        {}

        //! copy constructor
        /*! actually does not copy anything, and prints a warning if the other timer is running */
        Timer( const Timer& other ):
            _timerId( 0 ),
            _func( 0L ),
            _data( 0L )
        {
            if( other.isRunning() )
            { g_warning( "Oxygen::Timer::Timer - Copy constructor on running timer called." ); }

        }

        //! destructor
        virtual ~Timer( void )
        { if( _timerId ) g_source_remove( _timerId ); }

        //! start
        void start( int, GSourceFunc, gpointer );

        //! stop
        void stop( void )
        {
            if( _timerId ) g_source_remove( _timerId );
            reset();
        }

        //! true if running
        bool isRunning( void ) const
        { return _timerId != 0; }

        protected:

        //! reset
        void reset( void )
        {
            _timerId = 0;
            _data = 0;
            _func = 0;
        }

        //! delayed update
        static gboolean timeOut( gpointer );

        private:

        // assignment operator is private
        Timer& operator = (const Timer& )
        { return *this; }

        //! timer id
        int _timerId;

        //! source function
        GSourceFunc _func;

        //! data
        gpointer _data;

    };
}


#endif
