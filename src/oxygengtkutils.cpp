/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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

#include "oxygengtkutils.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <iostream>

namespace Gtk
{

    //________________________________________________________
    bool gtk_widget_has_rgba( GtkWidget* widget )
    {

        if( !widget ) return false;

        GdkScreen *screen( gtk_widget_get_screen (widget) );
        if( gdk_screen_is_composited( screen ) )
        {

            GdkVisual *visual = gtk_widget_get_visual (widget);
            return
                visual->depth == 32 &&
                visual->red_mask   == 0xff0000 &&
                visual->green_mask == 0x00ff00 &&
                visual->blue_mask  == 0x0000ff;

        } else return false;

    }

    //________________________________________________________
    GtkWidget* gtk_parent_button( GtkWidget* widget )
    {

        while( widget && (widget = gtk_widget_get_parent( widget ) ) )
        { if( GTK_IS_BUTTON( widget ) ) return widget; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_menu( GtkWidget* widget )
    {

        while( widget && (widget = gtk_widget_get_parent( widget ) ) )
        { if( GTK_IS_MENU( widget ) ) return widget; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_treeview( GtkWidget* widget )
    {

        while( widget && (widget = gtk_widget_get_parent( widget ) ) )
        { if( GTK_IS_TREE_VIEW( widget ) ) return widget; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_combobox( GtkWidget* widget )
    {

        while( widget && (widget = gtk_widget_get_parent( widget ) ) )
        { if( GTK_IS_COMBO_BOX( widget ) ) return widget; }

        return 0L;
    }

    //________________________________________________________
	bool gtk_object_is_a( const GObject* object, const gchar * type_name )
	{

		if( object )
		{
            const GType tmp( g_type_from_name( type_name ) );
            if( tmp )
            { return g_type_check_instance_is_a( (GTypeInstance*) object, tmp ); }
		}

		return false;
	}
    //________________________________________________________
    bool gtk_progress_bar_is_horizontal( GtkWidget* widget )
    {

        if( !GTK_IS_PROGRESS_BAR( widget ) ) return true;

        switch( gtk_progress_bar_get_orientation( GTK_PROGRESS_BAR( widget ) ) )
        {
            default:
            case GTK_PROGRESS_LEFT_TO_RIGHT:
            case GTK_PROGRESS_RIGHT_TO_LEFT:
            return true;

            case GTK_PROGRESS_BOTTOM_TO_TOP:
            case GTK_PROGRESS_TOP_TO_BOTTOM:
            return false;
        }

    }

    //________________________________________________________
    bool gdk_map_to_toplevel( GdkWindow* window, GtkWidget* widget, gint* x, gint* y, gint* w, gint* h, bool frame )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !( window && GDK_IS_WINDOW( window ) ) )
        {
            if( !widget ) return false;

            // this is an alternative way to get widget position with respect to top level window
            // and top level window size. This is used in case the GdkWindow passed as argument is
            // actually a 'non window' drawable
            window = gtk_widget_get_parent_window( widget );
            if( frame ) gdk_toplevel_get_frame_size( window, w, h );
            else gdk_toplevel_get_size( window, w, h );
            int xlocal, ylocal;
            const bool success( gtk_widget_translate_coordinates( widget, gtk_widget_get_toplevel( widget ), 0, 0, &xlocal, &ylocal ) );
            if( success )
            {

                if( x ) *x=xlocal;
                if( y ) *y=ylocal;

            }

            return success && ((!w) || *w > 0) && ((!h) || *h>0);

        } else {

            // get window size and height
            if( frame ) gdk_toplevel_get_frame_size( window, w, h );
            else gdk_toplevel_get_size( window, w, h );
            Gtk::gdk_window_get_toplevel_origin( window, x, y );
            return ((!w) || *w > 0) && ((!h) || *h>0);

        }

    }

    //________________________________________________________
    void gdk_toplevel_get_size( GdkWindow* window, gint* w, gint* h )
    {

        if( !( window && GDK_IS_WINDOW( window ) ) )
        {
            if( w ) *w = -1;
            if( h ) *h = -1;
            return;
        }

        if( GdkWindow* topLevel = gdk_window_get_toplevel( window ) )
        {
            gdk_drawable_get_size( topLevel, w, h );
        } else gdk_drawable_get_size( window, w, h );

        return;

    }

    //________________________________________________________
    void gdk_toplevel_get_frame_size( GdkWindow* window, gint* w, gint* h )
    {

        if( !( window && GDK_IS_WINDOW( window ) ) )
        {
            if( w ) *w = -1;
            if( h ) *h = -1;
            return;
        }

        if( GdkWindow* topLevel = gdk_window_get_toplevel( window ) )
        {
            GdkRectangle rect = {0, 0, -1, -1};
            gdk_window_get_frame_extents( topLevel, &rect );
            if( w ) *w = rect.width;
            if( h ) *h = rect.height;
        }

        return;

    }

    //________________________________________________________
    void gdk_window_get_toplevel_origin( GdkWindow* window, gint* x, gint* y )
    {
        if( x ) *x = 0;
        if( y ) *y = 0;
        if( !window ) return;
        while( window && GDK_IS_WINDOW( window ) &&
            gdk_window_get_window_type( window ) != GDK_WINDOW_TOPLEVEL &&
            gdk_window_get_window_type( window ) != GDK_WINDOW_TEMP
            )
        {
            gint xloc;
            gint yloc;
            gdk_window_get_position( window, &xloc, &yloc );
            if( x ) *x += xloc;
            if( y ) *y += yloc;
            window = gdk_window_get_parent( window );
        }

        return;
    }

}
