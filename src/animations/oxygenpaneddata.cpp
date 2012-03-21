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

#include "oxygenpaneddata.h"

#include <cassert>

namespace Oxygen
{

    //_______________________________________________
    void PanedData::connect( GtkWidget* widget )
    {
        updateCursor( widget );
        _realizeId.connect( G_OBJECT(widget), "realize", G_CALLBACK( realizeEvent ), this );
    }

    //_______________________________________________
    void PanedData::disconnect( GtkWidget* )
    { _realizeId.disconnect(); }

    //_______________________________________________
    void PanedData::realizeEvent( GtkWidget* widget, gpointer data )
    { static_cast<PanedData*>( data )->updateCursor( widget ); }

    //_______________________________________________
    void PanedData::updateCursor( GtkWidget* widget )
    {

        // do nothing if incorrect widget type
        if( !GTK_IS_PANED( widget ) ) return;

        // load cursor if needed
        if( !_cursorLoaded )
        {
            assert( !_cursor );

            GdkDisplay *display( gtk_widget_get_display( widget ) );
            const bool vertical( gtk_orientable_get_orientation( GTK_ORIENTABLE( widget ) ) == GTK_ORIENTATION_VERTICAL );
            _cursor = gdk_cursor_new_from_name( display, vertical ? "col-resize":"row-resize" );
            _cursorLoaded = true;

        }

        // assign to widget
        if( _cursor )
        {

            // load handle window
            GdkWindow* window(  gtk_paned_get_handle_window( GTK_PANED( widget ) ) );

            // assign cursor
            gdk_window_set_cursor( window, _cursor );

        }

    }

}
