#ifndef oxygenmenuitemdata_h
#define oxygenmenuitemdata_h
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
