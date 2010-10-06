/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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
        _motionId = g_signal_connect( G_OBJECT(widget), "motion-notify-event", (GCallback)motionNotifyEvent, 0L);
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, 0L );
    }

    //________________________________________________________________________________
    void MenuShellData::disconnect( GtkWidget* widget )
    {
        // disconnect signal
        g_signal_handler_disconnect(G_OBJECT(widget), _motionId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
    }

    //________________________________________________________________________________
    gboolean MenuShellData::motionNotifyEvent(GtkWidget *widget, GdkEventMotion *event, gpointer )
    {
        if( !GTK_IS_MENU_SHELL( widget ) ) return FALSE;

        gint pointer_x, pointer_y;
        GdkModifierType pointer_mask;
        gdk_window_get_pointer(widget->window, &pointer_x, &pointer_y, &pointer_mask);

        GList *children( gtk_container_get_children( GTK_CONTAINER(widget) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {

            if( !( child->data && GTK_IS_MENU_ITEM( child->data ) ) ) continue;
            if( gtk_widget_get_state( GTK_WIDGET( child->data ) ) == GTK_STATE_INSENSITIVE ) continue;

            GtkAllocation allocation;
            gtk_widget_get_allocation( GTK_WIDGET( child->data ), &allocation );
            if( Gtk::gdk_rectangle_contains( &allocation, pointer_x, pointer_y ) )
            {

                gtk_widget_set_state( GTK_WIDGET(child->data), GTK_STATE_PRELIGHT );

            } else {

                gtk_widget_set_state( GTK_WIDGET(child->data), GTK_STATE_NORMAL );

            }
        }

        if( children ) g_list_free( children );

        return FALSE;

    }

    //________________________________________________________________________________
    gboolean MenuShellData::leaveNotifyEvent( GtkWidget *widget, GdkEventCrossing *event, gpointer )
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
