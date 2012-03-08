#ifndef oxygenscrollbardata_h
#define oxygenscrollbardata_h
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

#include "oxygensignal.h"
#include "oxygentimer.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    // track scrollbars
    class ScrollBarData
    {

        public:

        //! constructor
        ScrollBarData( void ):
            _target( 0L ),
            _updatesDelayed( false ),
            _delay( 10 ),
            _locked( false )
        {}

        //! destructor
        virtual ~ScrollBarData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! toggle delayed updates
        void setUpdatesDelayed( bool value )
        { _updatesDelayed = value; }

        //! set delay
        void setDelay( int value )
        { _delay = value; }

        protected:

        static void valueChanged( GtkRange*, gpointer );

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! pointer to associated widget
        GtkWidget* _target;

        //! true if updates are delayed
        bool _updatesDelayed;

        //! update delay
        int _delay;

        //! timer
        Timer _timer;

        //! true if next update must be delayed
        bool _locked;

        //! signal
        Signal _valueChangedId;

    };

}

#endif
