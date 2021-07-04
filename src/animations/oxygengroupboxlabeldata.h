#ifndef oxygengroupboxlabeldata_h
#define oxygengroupboxlabeldata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    // track groupboxlabels
    class GroupBoxLabelData
    {

        public:

        //! constructor
        GroupBoxLabelData( void ):
            _resized( false )
        {}

        //! destructor
        virtual ~GroupBoxLabelData( void )
        {}

        //! setup connections
        void connect( GtkWidget* )
        {}

        //! disconnect
        void disconnect( GtkWidget* )
        { _resized = false; }

        //! adjust widget size
        void adjustSize( GtkWidget* widget )
        {
            if( _resized ) return;
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            if( allocation.height > 1 )
            {

                // Save resized state before actually resizing to prevent infinite recursion (bug 305833)
                _resized = true;
                gtk_widget_set_size_request( widget, allocation.width, allocation.height+14 );

            }
        }

        private:

        bool _resized;

    };

}

#endif
