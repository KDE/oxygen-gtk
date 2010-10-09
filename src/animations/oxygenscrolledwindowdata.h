#ifndef oxygenscrolledwindowdata_h
#define oxygenscrolledwindowdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* ScrolledWindow prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc.
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
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

/* TODO: right now this code is quite dangerous. It installs connections on ScrolledWindow scrollbars,
only if the later are found (which is not good), and does not track the scrollbar destruction.
(which is not good either). One would rather add the relevant ScrollBarData, instead */

#include <gtk/gtkwidget.h>
#include <gtk/gtkrange.h>

namespace Oxygen
{
    // track main window resize events
    class ScrolledWindowData
    {

        public:

        //! constructor
        ScrolledWindowData( void ):
            _verticalId(-1),
            _horizontalId(-1)
        {}

        //! destructor
        virtual ~ScrolledWindowData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        static void scrolled( GtkRange*, gpointer );

        private:

        int _verticalId;
        int _horizontalId;

    };

}

#endif
