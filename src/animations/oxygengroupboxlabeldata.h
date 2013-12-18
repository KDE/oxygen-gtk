#ifndef oxygengroupboxlabeldata_h
#define oxygengroupboxlabeldata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
