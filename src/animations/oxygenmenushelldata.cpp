/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* MenuShell prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc. <@redhat.com>
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
*
* the menushell data code is largely inspired from the gtk redmond engine
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

#include "oxygenmenushelldata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void MenuShellData::connect( GtkWidget* widget )
    {
        _motionId.connect( G_OBJECT(widget), "motion-notify-event", G_CALLBACK( motionNotifyEvent ), 0L);
        _leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), 0L );
    }

    //________________________________________________________________________________
    void MenuShellData::disconnect( GtkWidget* widget )
    {
        // disconnect signal
        _motionId.disconnect();
        _leaveId.disconnect();
    }

    //________________________________________________________________________________
    gboolean MenuShellData::motionNotifyEvent(GtkWidget* widget, GdkEventMotion*, gpointer )
    {
        if( !GTK_IS_MENU_SHELL( widget ) ) return FALSE;

        // get pointer position
        gint xPointer, yPointer;
        gdk_window_get_pointer(widget->window, &xPointer, &yPointer, 0L );

        GList *children( gtk_container_get_children( GTK_CONTAINER(widget) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;
            if( gtk_widget_get_state( GTK_WIDGET( child->data ) ) == GTK_STATE_INSENSITIVE ) continue;

            const GtkAllocation& allocation( GTK_WIDGET( child->data )->allocation );
            if( Gtk::gdk_rectangle_contains( &allocation, xPointer, yPointer ) )
            {

                // this triggers widget update
                gtk_widget_set_state( GTK_WIDGET(child->data), GTK_STATE_PRELIGHT );

            } else {

                // this triggers widget update
                gtk_widget_set_state( GTK_WIDGET(child->data), GTK_STATE_NORMAL );

            }
        }

        if( children ) g_list_free( children );

        return FALSE;

    }

    //________________________________________________________________________________
    gboolean MenuShellData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer )
    {
        if( !GTK_IS_MENU_SHELL( widget ) ) return FALSE;
        GList* children( gtk_container_get_children( GTK_CONTAINER( widget ) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;
            if( gtk_widget_get_state( GTK_WIDGET( child->data ) ) == GTK_STATE_INSENSITIVE ) continue;

            // this is terrible code. I hate gtk. (hugo)
            if( (!GTK_IS_MENU(GTK_MENU_ITEM(child->data)->submenu)) ||
                (!(GTK_WIDGET_REALIZED(GTK_MENU_ITEM(child->data)->submenu) &&
                GTK_WIDGET_VISIBLE(GTK_MENU_ITEM(child->data)->submenu) &&
                GTK_WIDGET_REALIZED(GTK_MENU(GTK_MENU_ITEM(child->data)->submenu)->toplevel) &&
                GTK_WIDGET_VISIBLE(GTK_MENU(GTK_MENU_ITEM(child->data)->submenu)->toplevel))))
            { gtk_widget_set_state( GTK_WIDGET(child->data), GTK_STATE_NORMAL ); }
        }

        if( children ) g_list_free( children );

        return FALSE;
    }

}
