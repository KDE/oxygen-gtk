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
#include <cstring>
#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{

    //____________________________________________________________
    void Gtk::gtk_container_adjust_buttons_state(GtkContainer* container,gpointer data)
    {
        if(GTK_IS_BUTTON(container))
        {
            int x,y;
            GtkWidget* button=GTK_WIDGET(container);
            gtk_widget_get_pointer(button,&x,&y);
            if( !(x>0 && y>0 &&
                    x < button->allocation.width &&
                    y < button->allocation.height) && gtk_widget_get_state(button)==GTK_STATE_ACTIVE )
            { gtk_widget_set_state(button,GTK_STATE_NORMAL); }

            gtk_button_set_relief(GTK_BUTTON(button),GTK_RELIEF_NORMAL);
            gtk_widget_set_size_request(button,16,16);

            return;

        }

        if(GTK_IS_CONTAINER(container))
        { gtk_container_foreach(container,(GtkCallback)gtk_container_adjust_buttons_state,0L); }

    }

    //____________________________________________________________
    bool Gtk::gtk_widget_is_panel_applet( GtkWidget* widget )
    {
        std::string name( G_OBJECT_TYPE_NAME( widget ) );
        return name.find( "PanelApplet" ) == 0 || name.find( "PanelWidget" ) == 0;
    }

    //____________________________________________________________
    void Gtk::gtk_widget_print_tree( GtkWidget* widget )
    {

        if( !widget ) return;
        std::cout << "Oxygen::Gtk::gtk_widget_print_tree - widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        while( ( widget = gtk_widget_get_parent( widget ) ) )
        { std::cout << "    parent: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl; }

    }

    //________________________________________________________
    bool Gtk::gtk_widget_has_rgba( GtkWidget* widget )
    {

        if( !widget ) return false;

        GdkScreen *screen( gtk_widget_get_screen( widget ) );
        if( screen && gdk_screen_is_composited( screen ) )
        {

            GdkVisual *visual( gtk_widget_get_visual (widget) );
            return
                visual->depth == 32 &&
                visual->red_mask   == 0xff0000 &&
                visual->green_mask == 0x00ff00 &&
                visual->blue_mask  == 0x0000ff;

        } else return false;

    }

    //________________________________________________________
    bool Gtk::gdk_window_is_base( GdkWindow* window )
    {
        GdkWindowTypeHint hint = gdk_window_get_type_hint( window );
        return(
            hint == GDK_WINDOW_TYPE_HINT_NORMAL ||
            hint == GDK_WINDOW_TYPE_HINT_DIALOG ||
            hint == GDK_WINDOW_TYPE_HINT_UTILITY );
    }

    //________________________________________________________
    bool Gtk::gdk_window_nobackground( GdkWindow* window )
    {
        GdkWindowTypeHint hint = gdk_window_get_type_hint( window );
        return( hint == GDK_WINDOW_TYPE_HINT_COMBO || hint == GDK_WINDOW_TYPE_HINT_TOOLTIP );
    }

    //________________________________________________________
    bool Gtk::gdk_window_has_rgba( GdkWindow* window )
    {

        if( !window ) return false;

        GdkScreen *screen( gdk_drawable_get_screen( GDK_DRAWABLE( window ) ) );
        if( gdk_screen_is_composited( screen ) )
        {

            GdkVisual *visual( gdk_drawable_get_visual( GDK_DRAWABLE( window ) ) );
            return
                visual->depth == 32 &&
                visual->red_mask   == 0xff0000 &&
                visual->green_mask == 0x00ff00 &&
                visual->blue_mask  == 0x0000ff;

        } else return false;

    }

    //________________________________________________________
    bool Gtk::gtk_object_is_a( const GObject* object, const std::string& type_name )
    {

        if( object )
        {
            const GType tmp( g_type_from_name( type_name.c_str() ) );
            if( tmp )
            { return g_type_check_instance_is_a( (GTypeInstance*) object, tmp ); }
        }

        return false;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_button( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_BUTTON( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_menubar( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_MENU_BAR( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_menu( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_MENU( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_tree_view( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_TREE_VIEW( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_combo( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_COMBO( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_combobox( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_COMBO_BOX( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_combobox_entry( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_COMBO_BOX_ENTRY( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_scrolled_window( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_SCROLLED_WINDOW( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_statusbar( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_STATUSBAR( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_notebook( GtkWidget* widget )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( GTK_IS_NOTEBOOK( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    bool Gtk::gtk_widget_is_parent( GtkWidget* widget, GtkWidget* potentialParent )
    {

        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        { if( potentialParent==parent ) return true; }

        return false;
    }

    //________________________________________________________
    bool Gtk::gtk_parent_is_shadow_in( GtkWidget* widget )
    {
        GtkWidget *parent( widget );
        while( parent && (parent = gtk_widget_get_parent( parent ) ) )
        {
            if( GTK_IS_FRAME( parent ) && gtk_frame_get_shadow_type( GTK_FRAME( parent ) ) == GTK_SHADOW_IN ) return true;
            if( GTK_IS_SCROLLED_WINDOW( parent ) && gtk_scrolled_window_get_shadow_type( GTK_SCROLLED_WINDOW( parent ) ) == GTK_SHADOW_IN ) return true;
        }

        return false;
    }

    //________________________________________________________
    bool Gtk::gtk_button_is_flat( GtkWidget* widget )
    {
        if( !GTK_IS_BUTTON( widget ) ) return false;
        return ( gtk_button_get_relief( GTK_BUTTON( widget ) ) == GTK_RELIEF_NONE );
    }

    //________________________________________________________
    bool Gtk::gtk_button_is_in_path_bar( GtkWidget* widget )
    {
        if( !GTK_IS_BUTTON( widget ) ) return false;

        std::string name(G_OBJECT_TYPE_NAME( gtk_widget_get_parent( widget ) ) );
        return name == "GtkPathBar" || name == "NautilusPathBar";
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_button_find_image(GtkWidget* button)
    {
        if( !GTK_IS_CONTAINER(button) ) return 0L;
        for(GList* children=gtk_container_get_children(GTK_CONTAINER(button)); children; children=children->next)
        {
            if( GTK_IS_IMAGE(children->data) ) return GTK_WIDGET(children->data);
            else if( GTK_IS_CONTAINER(children->data) ) return gtk_button_find_image(GTK_WIDGET(children->data));
        }
        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_button_find_label(GtkWidget* button)
    {
        if(!GTK_IS_CONTAINER(button))
            return 0L;
        for(GList* children=gtk_container_get_children(GTK_CONTAINER(button)); children; children=children->next)
        {
            if( GTK_IS_LABEL(children->data) ) return GTK_WIDGET(children->data);
            else if( GTK_IS_CONTAINER(children->data) ) return gtk_button_find_image(GTK_WIDGET(children->data));
        }
        return 0L;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_tree_view( GtkWidget* widget )
    {
        // check types
        if( !widget && GTK_IS_TREE_VIEW( widget ) && GTK_IS_SCROLLED_WINDOW( gtk_widget_get_parent( widget ) ) ) return false;

        // retrieve widget path, compare and free
        gchar* widgetPath;
        gtk_widget_path( widget, 0L, &widgetPath, 0L);
        bool out( std::string( widgetPath ) == "gtk-combobox-popup-window.GtkScrolledWindow.GtkTreeView" );
        g_free( widgetPath );
        return out;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_scrolled_window( GtkWidget* widget )
    {
        if( !GTK_IS_SCROLLED_WINDOW(widget) ) return false;

        gchar* widgetPath;
        gtk_widget_path( widget, 0L, &widgetPath, 0L);
        bool out( std::string( widgetPath ) == "gtk-combobox-popup-window.GtkScrolledWindow" );
        g_free( widgetPath );
        return out;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_viewport( GtkWidget* widget )
    {
        if( !GTK_IS_VIEWPORT(widget) ) return false;
        gchar* widgetPath;
        gtk_widget_path( widget, 0L, &widgetPath, 0L);
        static const std::string match( "gtk-combo-popup-window" );
        bool out = ( std::string( widgetPath ).substr( 0, match.size() ) == match );
        g_free( widgetPath );
        return out;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_frame( GtkWidget* widget )
    {
        if( !GTK_IS_FRAME(widget) ) return false;
        gchar* widgetPath;
        gtk_widget_path( widget, 0L, &widgetPath, 0L);
        static const std::string match( "gtk-combo-popup-window" );
        bool out = ( std::string( widgetPath ).substr( 0, match.size() ) == match );
        g_free( widgetPath );
        return out;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_appears_as_list( GtkWidget* widget )
    {
        gboolean appearsAsList;
        gtk_widget_style_get( widget, "appears-as-list", &appearsAsList, NULL );
        return (bool) appearsAsList;
    }

    //________________________________________________________
    bool Gtk::gtk_notebook_tab_contains( GtkWidget* widget, int tab, int x, int y )
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
    int Gtk::gtk_notebook_find_tab( GtkWidget* widget, int x, int y )
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

            if(!tabLabel)
                return tab;

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
    int Gtk::gtk_notebook_find_first_tab( GtkWidget* widget )
    {

        if( !GTK_IS_NOTEBOOK( widget ) ) return 0;

        // cast to notebook
        GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
        return g_list_position( notebook->children, notebook->first_tab );

    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_is_tab_label(GtkNotebook* notebook, GtkWidget* widget )
    {

        for( int i = 0; i <  gtk_notebook_get_n_pages( notebook ); ++i )
        {
            // retrieve page and tab label
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );
            if( widget == tabLabel ) return true;
        }

        return false;

    }


    //____________________________________________________________
    void Gtk::gtk_notebook_get_tabbar_rect( GtkNotebook* notebook, GdkRectangle* rect )
    {
        // check notebook and rect
        if( !( notebook && rect ) ) return;

        // check tab visibility
        if( !( gtk_notebook_get_show_tabs( notebook ) && gtk_container_get_children( GTK_CONTAINER( notebook ) ) ) )
        {
            *rect = gdk_rectangle();
            return;
        }

        // get full rect
        gtk_widget_get_allocation( GTK_WIDGET( notebook ), rect );

        // adjust to account for borderwidth
        guint borderWidth( gtk_container_get_border_width( GTK_CONTAINER( notebook ) ) );
        rect->x += borderWidth;
        rect->y += borderWidth;
        rect->height -= 2*borderWidth;
        rect->width -= 2*borderWidth;

        // get current page
        int pageIndex( gtk_notebook_get_current_page( notebook ) );
        if( !( pageIndex >= 0 && pageIndex < gtk_notebook_get_n_pages( notebook ) ) )
        {
            *rect = gdk_rectangle();
            return;
        }

        GtkWidget* page( gtk_notebook_get_nth_page( notebook, pageIndex ) );
        if( !page )
        {
            *rect = gdk_rectangle();
            return;
        }

        // removes page allocated size from rect, based on tabwidget orientation
        const GtkAllocation& pageAllocation( page->allocation );
        switch( gtk_notebook_get_tab_pos( notebook ) )
        {
            case GTK_POS_BOTTOM:
            rect->y += pageAllocation.height;
            rect->height -= pageAllocation.height;
            break;

            case GTK_POS_TOP:
            rect->height -= pageAllocation.height;
            break;

            case GTK_POS_RIGHT:
            rect->x += pageAllocation.width;
            rect->width -= pageAllocation.width;
            break;

            case GTK_POS_LEFT:
            rect->width -= pageAllocation.width;
            break;
        }

        return;

    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_has_visible_arrows( GtkNotebook* notebook )
    {

        if( !gtk_notebook_get_show_tabs( notebook ) ) return false;

        // loop over pages
        for( int i = 0; i <  gtk_notebook_get_n_pages( notebook ); ++i )
        {

            // retrieve page and tab label
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            GtkWidget* label( gtk_notebook_get_tab_label( notebook, page ) );
            if( label && !gtk_widget_get_mapped( label ) ) return true;

        }

        return false;

    }


    //____________________________________________________________
    int Gtk::gtk_notebook_get_current_tab(GtkNotebook* notebook)
    {
        if( notebook && notebook->children && notebook->cur_page ) return g_list_index( notebook->children, notebook->cur_page );
        else return -1;
    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_update_close_buttons(GtkNotebook* notebook)
    {
        // cast to notebook and check against number of pages
        if( GTK_IS_NOTEBOOK( notebook ) )
        {
            GtkWidget* tabLabel=0;
            int numPages=gtk_notebook_get_n_pages( notebook );
            for( int i = 0; i < numPages; ++i )
            {

                // retrieve page and tab label
                GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
                if(page)
                    tabLabel=gtk_notebook_get_tab_label( notebook, page );

                if(page && tabLabel && GTK_IS_CONTAINER(tabLabel))
                    gtk_container_adjust_buttons_state(GTK_CONTAINER(tabLabel));

            }
        }
        return FALSE;
    }

    //________________________________________________________
    bool Gtk::gtk_notebook_is_close_button(GtkWidget* widget)
    {
        if( GtkNotebook* nb=GTK_NOTEBOOK(gtk_parent_notebook(widget) ) )
        {
            // check if the button resides on tab label, not anywhere on the tab
            bool tabLabelIsParent=false;
            for( int i=0; i < gtk_notebook_get_n_pages(nb); ++i )
            {
                GtkWidget* tabLabel( gtk_notebook_get_tab_label(nb,gtk_notebook_get_nth_page( nb, i ) ) );
                if( gtk_widget_is_parent( widget, GTK_WIDGET(tabLabel) ) )
                { tabLabelIsParent=true; }
            }

            if( !tabLabelIsParent ) return false;

            // make sure button has no text and some image (for now, just hope it's a close icon)
            if( gtk_button_find_image(widget) && !gtk_button_get_label( GTK_BUTTON(widget) ) )
            { return true; }

            // check for pidgin 'x' close button
            if( GtkWidget* label = gtk_button_find_label(widget) )
            {

                const gchar* labelText=gtk_label_get_text( GTK_LABEL(label) );
                if(!strcmp(labelText,"×")) // It's not letter 'x' - it's a special symbol
                {
                    gtk_widget_hide( label );
                    return true;
                } else return false;

            } else return false;

        } else return false;

    }

    //________________________________________________________
    bool Gtk::gtk_progress_bar_is_horizontal( GtkWidget* widget )
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
    bool Gtk::gdk_map_to_toplevel( GdkWindow* window, GtkWidget* widget, gint* x, gint* y, gint* w, gint* h, bool frame )
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
    void Gtk::gdk_toplevel_get_size( GdkWindow* window, gint* w, gint* h )
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
    void Gtk::gdk_toplevel_get_frame_size( GdkWindow* window, gint* w, gint* h )
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
    void Gtk::gdk_window_get_toplevel_origin( GdkWindow* window, gint* x, gint* y )
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
    GdkPixbuf* Gtk::gdk_pixbuf_set_alpha( const GdkPixbuf *pixbuf, double alpha )
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

    //_________________________________________________________
    bool Gtk::gdk_pixbuf_to_gamma(GdkPixbuf* pixbuf, double value)
    {
        if(gdk_pixbuf_get_colorspace(pixbuf)==GDK_COLORSPACE_RGB &&
            gdk_pixbuf_get_bits_per_sample(pixbuf)==8 &&
            gdk_pixbuf_get_has_alpha(pixbuf) &&
            gdk_pixbuf_get_n_channels(pixbuf)==4)
        {
            double gamma=1./(2.*value+0.5);
            unsigned char* data=gdk_pixbuf_get_pixels(pixbuf);
            const int height=gdk_pixbuf_get_height(pixbuf);
            const int width=gdk_pixbuf_get_width(pixbuf);
            const int rowstride=gdk_pixbuf_get_rowstride(pixbuf);
            for(int x=0;x<width;++x)
            {
                for(int y=0; y<height; y++)
                {
                    unsigned char* p=data + y*rowstride + x*4;
                    *p=(char)(pow((*p/255.),gamma)*255); ++p;
                    *p=(char)(pow((*p/255.),gamma)*255); ++p;
                    *p=(char)(pow((*p/255.),gamma)*255);
                }

            }

            return true;

        } else return false;

    }

    //___________________________________________________________
    GdkPixbuf* Gtk::gdk_pixbuf_resize( GdkPixbuf* src, int width, int height )
    {
        if( width == gdk_pixbuf_get_width( src ) &&  height == gdk_pixbuf_get_height( src ) )
        {

            return static_cast<GdkPixbuf*>(g_object_ref (src));

        } else {

            return gdk_pixbuf_scale_simple( src, width, height, GDK_INTERP_BILINEAR );

        }

    }

}
