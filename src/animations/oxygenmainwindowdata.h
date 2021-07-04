#ifndef oxygenmainwindowdata_h
#define oxygenmainwindowdata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignal.h"
#include "oxygentimer.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    // track main window resize events
    class MainWindowData
    {

        public:

        //! constructor
        MainWindowData( void ):
            _target(0L),
            _locked(false),
            _width(-1),
            _height(-1)
        {}

        //! destructor
        virtual ~MainWindowData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        //! update size
        void updateSize( int width, int height );

        //!name static callbacks
        static gboolean configureNotifyEvent( GtkWidget*, GdkEventConfigure*, gpointer);

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! pointer to associated widget
        GtkWidget* _target;

        //! timer
        Timer _timer;

        //! true if next update must be delayed
        bool _locked;

        //! configure signal id
        Signal _configureId;

        //! old width
        int _width;

        //! old height
        int _height;


    };

}

#endif
