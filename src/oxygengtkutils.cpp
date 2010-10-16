/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* GdkPixbuf modification code from Walmis
* <http://gnome-look.org/content/show.php?content=77783&forumpage=3>
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

#include <cmath>
#include <gtk/gtk.h>

namespace Gtk
{
    //_________________________________________________________
    bool gdk_pixbuf_to_gamma(GdkPixbuf* pixbuf, double value)
    {
                if(gdk_pixbuf_get_colorspace(pixbuf)==GDK_COLORSPACE_RGB &&
                        gdk_pixbuf_get_bits_per_sample(pixbuf)==8 &&
                        gdk_pixbuf_get_has_alpha(pixbuf) &&
                        gdk_pixbuf_get_n_channels(pixbuf)==4)
                {
                    double gamma=1./(2.*value+0.5);
                    guchar* data=gdk_pixbuf_get_pixels(pixbuf);
                    int height=gdk_pixbuf_get_height(pixbuf);
                    int width=gdk_pixbuf_get_width(pixbuf);
                    int rowstride=gdk_pixbuf_get_rowstride(pixbuf);
                    for(int x=0;x<width;++x)
                        for(int y=0; y<height; y++)
                        {
                            guchar* p=data + y*rowstride + x*4;
                            p[0]=(char)(pow((p[0]/255.),gamma)*255);
                            p[1]=(char)(pow((p[1]/255.),gamma)*255);
                            p[2]=(char)(pow((p[2]/255.),gamma)*255);
                        }
                    return true;
                } else
                {
                    return false;
                }
    }

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

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_BUTTON( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_menu( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_MENU( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_treeview( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_TREE_VIEW( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_combobox( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_COMBO_BOX( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_combobox_entry( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_COMBO_BOX_ENTRY( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_scrolled_window( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_SCROLLED_WINDOW( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* gtk_parent_notebook( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_NOTEBOOK( parent ) ) return parent; }

        return 0L;
    }


    //________________________________________________________
    bool gtk_button_is_flat( GtkWidget* widget )
    {
        if( !GTK_IS_BUTTON( widget ) ) return false;
        return ( gtk_button_get_relief( GTK_BUTTON( widget ) ) == GTK_RELIEF_NONE );
    }

    //________________________________________________________
    bool gtk_notebook_tab_contains( GtkWidget* widget, int tab, int x, int y )
    {

        if( !( tab >= 0 && GTK_IS_NOTEBOOK( widget ) ) ) return false;

        // cast to notebook and check against number of pages
        GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
        if( tab >= gtk_notebook_get_n_pages( notebook ) ) return false;

        // retrieve page and tab label
        GtkWidget* page( gtk_notebook_get_nth_page( notebook, tab ) );
        GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );

        // get allocted size and compare to position
        const GtkAllocation& allocation( tabLabel->allocation );
        return Gtk::gdk_rectangle_contains( &allocation, x, y );

    }

     //________________________________________________________
    int gtk_notebook_find_tab( GtkWidget* widget, int x, int y )
    {

        if( !GTK_IS_NOTEBOOK( widget ) ) return -1;

        // cast to notebook and check against number of pages
        GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
        int tab(-1);
        int minDistance( -1 );
        for( int i = gtk_notebook_find_first_tab( widget ); i <  gtk_notebook_get_n_pages( notebook ); ++i )
        {

            // retrieve page and tab label
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );

            // get allocted size and compare to position
            const GtkAllocation& allocation( tabLabel->allocation );

            // get manhattan length
            const int distance = int(
                std::abs( double( allocation.x + allocation.width/2 - x ) ) +
                std::abs( double( allocation.y + allocation.height/2 - y ) ) );
            if( minDistance < 0 || distance < minDistance )
            {
                tab = i;
                minDistance = distance;
            }
        }

        return tab;

    }

    //________________________________________________________
    int gtk_notebook_find_first_tab( GtkWidget* widget )
    {

        if( !GTK_IS_NOTEBOOK( widget ) ) return 0;

        // cast to notebook
        GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
        return g_list_position( notebook->children, notebook->first_tab );

    }

    //________________________________________________________
    bool gtk_is_parent( GtkWidget* widget, GtkWidget* potentialParent )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( potentialParent==parent ) return true; }

        return false;
    }

    //________________________________________________________
    GtkWidget* gtk_button_find_image(GtkWidget* button)
    {
        if(!GTK_IS_CONTAINER(button))
            return NULL;
        for(GList* children=gtk_container_get_children(GTK_CONTAINER(button)); children; children=children->next)
        {
            if(GTK_IS_IMAGE(children->data))
                return GTK_WIDGET(children->data);
            else if(GTK_IS_CONTAINER(children->data))
                return gtk_button_find_image(GTK_WIDGET(children->data));
        }
        return NULL;
    }

    //________________________________________________________
    bool is_notebook_close_button(GtkWidget* widget)
    {
        if(GtkNotebook* nb=GTK_NOTEBOOK(gtk_parent_notebook(widget)))
        {
            // check if the button resides on tab label, not anywhere on the tab
            bool tabLabelIsParent=false;
            for(int i=0;i<gtk_notebook_get_n_pages(nb);++i)
            {
                GtkWidget* tabLabel=gtk_notebook_get_tab_label(nb,gtk_notebook_get_nth_page(nb,i));
                if(gtk_is_parent(widget,GTK_WIDGET(tabLabel)))
                {
                    tabLabelIsParent=true;
                }
            }
            if(!tabLabelIsParent)
                return false;
            // make sure button has no text and some image (for now, just hope it's a close icon)
            if(!gtk_button_find_image(widget) || gtk_button_get_label(GTK_BUTTON(widget)))
                return false;
            else
                return true;
        }
        else
            return false;
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

    //___________________________________________________________
    GdkPixbuf* gdk_pixbuf_set_alpha( const GdkPixbuf *pixbuf, double alpha )
    {

        g_return_val_if_fail( pixbuf != 0L, 0L);
        g_return_val_if_fail( GDK_IS_PIXBUF( pixbuf ), 0L );

        /* Returns a copy of pixbuf with it's non-completely-transparent pixels to
        have an alpha level "alpha" of their original value. */
        GdkPixbuf* target( gdk_pixbuf_add_alpha( pixbuf, false, 0, 0, 0 ) );
        if( alpha >= 1.0 ) return target;
        if( alpha < 0 ) alpha = 0;

        const int width( gdk_pixbuf_get_width( target ) );
        const int height( gdk_pixbuf_get_height( target ) );
        const int rowstride( gdk_pixbuf_get_rowstride( target ) );
        unsigned char* data = gdk_pixbuf_get_pixels( target );

        for( int y = 0; y < height; ++y )
        {

            for( int x = 0; x < width; ++x )
            {
                /* The "4" is the number of chars per pixel, in this case, RGBA,
                the 3 means "skip to the alpha" */
                unsigned char* current = data + ( y*rowstride ) + ( x*4 ) + 3;
                *(current) = (unsigned char) ( *( current )*alpha );
            }
        }

        return target;
    }

    //___________________________________________________________
    GdkPixbuf* gdk_pixbuf_resize( GdkPixbuf* src, int width, int height )
    {
        if( width == gdk_pixbuf_get_width( src ) &&  height == gdk_pixbuf_get_height( src ) )
        {

            return static_cast<GdkPixbuf*>(g_object_ref (src));

        } else {

            return gdk_pixbuf_scale_simple( src, width, height, GDK_INTERP_BILINEAR );

        }

    }

}
