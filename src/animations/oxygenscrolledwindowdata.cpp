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
* the scrolledwindow data code is largely inspired from the gtk redmond engine
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

#include "oxygenscrolledwindowdata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <gtk/gtkscrolledwindow.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void ScrolledWindowData::connect( GtkWidget* widget )
    {
        if( !GTK_IS_SCROLLED_WINDOW(widget) ) return;
        GtkScrolledWindow* scrolledWindow( GTK_SCROLLED_WINDOW( widget ) );

        // connect vertical scrollbar
        if( GtkWidget* vscrollbar = gtk_scrolled_window_get_vscrollbar( scrolledWindow ) )
        { _verticalId = g_signal_connect( G_OBJECT(vscrollbar), "value-changed", (GCallback)scrolled, widget ); }

        // connect horizontal scrollbar
        if( GtkWidget* hscrollbar = gtk_scrolled_window_get_hscrollbar( scrolledWindow ) )
        { _horizontalId = g_signal_connect( G_OBJECT(hscrollbar), "value-changed", (GCallback)scrolled, widget ); }

    }

    //________________________________________________________________________________
    void ScrolledWindowData::disconnect( GtkWidget* widget )
    {

        if( !GTK_IS_SCROLLED_WINDOW(widget) ) return;
        GtkScrolledWindow* scrolledWindow( GTK_SCROLLED_WINDOW( widget ) );

        // vertical scrollbar
        GtkWidget* vscrollbar( gtk_scrolled_window_get_vscrollbar( scrolledWindow ) );
        if( vscrollbar && _verticalId > 0 ) g_signal_handler_disconnect(G_OBJECT(vscrollbar), _verticalId );

        // horizontal scrollbar
        GtkWidget* hscrollbar( gtk_scrolled_window_get_hscrollbar( scrolledWindow ) );
        if( hscrollbar && _horizontalId > 0 ) g_signal_handler_disconnect(G_OBJECT(hscrollbar), _horizontalId );

    }

    //________________________________________________________________________________
    void ScrolledWindowData::scrolled( GtkRange*, gpointer data )
    {
        // trigger repaint
        gtk_widget_queue_draw( static_cast<GtkWidget*>(data) );
        return;
    }

}
