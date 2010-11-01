#ifndef oxygentimer_h
#define oxygentimer_h
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

namespace Oxygen
{
    //! handles gtk timeouts
    /*! make sure the timer is properly reset at destruction */
    class Timer
    {

        public:

        //! constructor
        Timer( void );

        //! copy constructor
        /*! actually does not copy anything, and prints a warning if the other timer is running */
        Timer( const Timer& );

        //! destructor
        virtual ~Timer( void );

        //! start
        void start( int, GSourceFunc, gpointer );

        //! stop
        void stop( void );

        //! true if running
        bool isRunning( void ) const
        { return _timerId != 0; }

        protected:

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
