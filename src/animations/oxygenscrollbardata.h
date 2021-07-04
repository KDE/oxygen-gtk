#ifndef oxygenscrollbardata_h
#define oxygenscrollbardata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
