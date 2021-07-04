#ifndef oxygenpaneddata_h
#define oxygenpaneddata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygensignal.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    class PanedData
    {

        public:

        //! constructor
        PanedData( void ):
            _cursorLoaded( false ),
            _cursor( 0L )
        {}

        //! destructor
        virtual ~PanedData( void )
        {
            disconnect( 0L );
            if( _cursor ) gdk_cursor_unref( _cursor );
        }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        protected:

        //! update cursor
        virtual void updateCursor( GtkWidget* );

        //! realization hook
        static void realizeEvent( GtkWidget*, gpointer );

        private:

        //! realization signal
        Signal _realizeId;

        //! cursor
        bool _cursorLoaded;
        GdkCursor* _cursor;

    };

}

#endif
