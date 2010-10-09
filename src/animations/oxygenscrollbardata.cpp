/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* ScrollBar prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc.
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
*
* the scrollbar data code is largely inspired from the gtk redmond engine
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

#include "oxygenscrollbardata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void ScrollBarData::connect( GtkWidget* widget )
    { _valueChangedId = g_signal_connect( G_OBJECT(widget), "value-changed", (GCallback)valueChanged, widget ); }

    //________________________________________________________________________________
    void ScrollBarData::disconnect( GtkWidget* widget )
    { g_signal_handler_disconnect(G_OBJECT(widget), _valueChangedId ); }

    //________________________________________________________________________________
    void ScrollBarData::valueChanged( GtkRange* widget, gpointer data )
    {
        if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( GTK_WIDGET( widget ) ) )
        { gtk_widget_queue_draw( static_cast<GtkWidget*>(parent) ); }
        return;
    }

}
