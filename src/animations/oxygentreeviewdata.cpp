/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* the treeview data code is largely inspired from the gtk redmond engine
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

#include "oxygentreeviewdata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void TreeViewData::connect( GtkWidget* widget )
    {

        // get full-width flag
        if( GTK_IS_TREE_VIEW( widget ) )
        { gtk_widget_style_get( widget, "row_ending_details", &_fullWidth, NULL ); }

        _enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", (GCallback)enterNotifyEvent, this );
        _motionId = g_signal_connect( G_OBJECT(widget), "motion-notify-event", (GCallback)motionNotifyEvent, this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
    }

    //________________________________________________________________________________
    void TreeViewData::disconnect( GtkWidget* widget )
    {
        g_signal_handler_disconnect(G_OBJECT(widget), _enterId );
        g_signal_handler_disconnect(G_OBJECT(widget), _motionId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
    }

    //________________________________________________________________________________
    void TreeViewData::updatePosition( GtkWidget* widget, int x, int y )
    {

        if( !GTK_IS_TREE_VIEW( widget ) ) return;
        GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );

        // get path at x and y
        GtkTreePath* path(0L);
        gtk_tree_view_get_path_at_pos( treeView, x,y, &path, 0L, 0L, 0L );

        // compare path.
        // do nothing if unchanged
        if( !( path || _path ) ) return;
        else if( _path && path && !gtk_tree_path_compare( _path, path ) )
        {
            gtk_tree_path_free( path );
            return;
        }

        // prepare update area
        GdkRectangle oldRect( Gtk::gdk_rectangle() );
        if( _path )
        {
            gtk_tree_view_get_background_area( treeView, _path, 0L, &oldRect );
            oldRect.width = widget->allocation.width;
        }

        GdkRectangle newRect( Gtk::gdk_rectangle() );
        if( path )
        {
            gtk_tree_view_get_background_area( treeView, path, 0L, &newRect );
            newRect.width = widget->allocation.width;
        }

        GdkRectangle updateRect;
        if( Gtk::gdk_rectangle_is_valid( &oldRect ) )
        {
            if( Gtk::gdk_rectangle_is_valid( &newRect ) ) gdk_rectangle_union( &oldRect, &newRect, &updateRect );
            else updateRect = oldRect;
        } else updateRect = newRect;

        // update path and position
        if( !path )
        {

            _x = -1;
            _y = -1;
            if( _path ) gtk_tree_path_free( _path );
            _path = 0L;

        } else {

            _x = newRect.x + newRect.width/2;
            _y = newRect.y + newRect.height/2;
            if( _path ) gtk_tree_path_free( _path );
            _path = path;

        }

        // schedule redraw
        Gtk::gtk_widget_queue_draw( widget, &updateRect );

    }

    //________________________________________________________________________________
    gboolean TreeViewData::enterNotifyEvent(GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<TreeViewData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean TreeViewData::motionNotifyEvent(GtkWidget* widget, GdkEventMotion* event, gpointer data )
    {

        static_cast<TreeViewData*>( data )->updatePosition( widget, event->x, event->y );
        return FALSE;

    }

    //________________________________________________________________________________
    gboolean TreeViewData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<TreeViewData*>( data )->setHovered( widget, false );
        return FALSE;
    }

}
