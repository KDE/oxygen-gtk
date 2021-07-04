#ifndef oxygentimeline_h
#define oxygentimeline_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <glib.h>
#include <cassert>
#include <cmath>

namespace Oxygen
{

    //! timeline used to handle animations
    class TimeLine
    {

        public:

        //! constructor
        TimeLine( int = 0 );

        //! copy constructor
        /*! warning other timeline state is not copied */
        TimeLine( const TimeLine& );

        //! destructor
        virtual ~TimeLine( void );

        //! assignment operator
        TimeLine& operator = (const TimeLine& );

        //! connect callback
        void connect( GSourceFunc func, gpointer data )
        {
            _func = func;
            _data = data;
        }

        //! disconnect
        void disconnect( void )
        {
            _func = 0L;
            _data = 0L;
        }

        //!@name accessors
        //@{

        //! value (between 0 and 1)
        double value( void ) const
        { return _value; }

        //! true if connected
        bool isConnected( void ) const
        { return _func && _data; }

        //! true if running
        bool isRunning( void ) const
        { return _running; }

        //@}

        //!@name modifiers
        //@{

        //! duration
        void setDuration( int value )
        { _duration = value; }

        //! enable state
        void setEnabled( bool value )
        { _enabled = value; }

        //! direction
        enum Direction
        {
            Forward,
            Backward
        };

        //! direction
        void setDirection( Direction direction )
        { _direction = direction; }

        //! start
        void start( void );

        //! stop
        void stop( void );

        //! update value and running state
        /*!
        also emits signal when value has changed since last time.
        returns true if timeline is still running
        */
        bool update( void );

        //@}

        //! steps
        static void setSteps( int value )
        { _steps = value; }

        protected:

        //! run callback
        void trigger( void ) const
        { if( _func )  (_func)(_data); }

        //! digitize value, based on steps
        double digitize( const double& value ) const
        {
            if( _steps > 0 ) return std::floor( value*_steps )/_steps;
            else return value;
        }


        private:

        //! duration
        int _duration;

        //! enable state
        bool _enabled;

        //! direction
        Direction _direction;

        //! true if timer is running
        bool _running;

        //! value (between 0 and 1)
        double _value;

        //! time (at which _value was last calculated)
        int _time;

        //! timer
        GTimer* _timer;

        //! source function
        GSourceFunc _func;

        //! data
        gpointer _data;

        //! steps
        static int _steps;

    };


};

#endif
