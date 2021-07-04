/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
            _cursor = gdk_cursor_new_from_name( display, GTK_IS_VPANED( widget ) ? "col-resize":"row-resize" );
            _cursorLoaded = true;

        }

        // assign to widget
        if( _cursor )
        {

            // load handle window
            #if GTK_CHECK_VERSION(2, 20, 0)
            GdkWindow* window(  gtk_paned_get_handle_window( GTK_PANED( widget ) ) );
            #else
            GdkWindow* window( GTK_PANED( widget )->handle );
            #endif

            // assign cursor
            gdk_window_set_cursor( window, _cursor );

        }

    }

}
