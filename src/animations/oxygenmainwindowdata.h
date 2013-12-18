#ifndef oxygenmainwindowdata_h
#define oxygenmainwindowdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
