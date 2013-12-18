#ifndef oxygenmenuitemdata_h
#define oxygenmenuitemdata_h
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
    class MenuItemData
    {

        public:

        //! constructor
        MenuItemData( void ):
            _target(0)
        {}

        //! destructor
        virtual ~MenuItemData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        //! attach style of widget to passed window [recursive]
        void attachStyle( GtkWidget*, GdkWindow* ) const;

        //! parent set callback
        static void parentSet( GtkWidget*, GtkWidget*, gpointer );

        private:

        //! target
        GtkWidget* _target;

        //! reparent signal id
        Signal _parentSetId;

    };

}

#endif
