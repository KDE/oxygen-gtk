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
#include "oxygengtktypenames.h"
#include "config.h"

#include <cmath>
#include <cstring>
#include <gtk/gtk.h>
#include <iostream>
#include <set>

namespace Oxygen
{
    GQuark Gtk::Quarks::_rcStyle = 0L;

    //____________________________________________________________
    void Gtk::gtk_container_adjust_buttons_state(GtkContainer* container,gpointer data)
    {
        if(GTK_IS_BUTTON(container))
        {
            int x,y;
            GtkWidget* button=GTK_WIDGET(container);
            GtkAllocation allocation( gtk_widget_get_allocation( button ) );
            gtk_widget_get_pointer(button,&x,&y);
            if( !(x>0 && y>0 &&
                x < allocation.width &&
                y < allocation.height) && gtk_widget_get_state(button)==GTK_STATE_ACTIVE )
            { gtk_widget_set_state(button,GTK_STATE_NORMAL); }

            gtk_button_set_relief(GTK_BUTTON(button),GTK_RELIEF_NORMAL);
            gtk_widget_set_size_request(button,16,16);

            return;

        }

        if(GTK_IS_CONTAINER(container))
        { gtk_container_foreach(container,(GtkCallback)gtk_container_adjust_buttons_state,0L); }

    }

    //____________________________________________________________
    bool Gtk::gtk_widget_has_custom_background( GtkWidget* widget, GtkStateType state )
    {

        // loop over all parents, recursively
        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        { if( gtk_widget_get_modifier_style(parent)->color_flags[state]&GTK_RC_BG ) return true; }

        return false;

    }

    //____________________________________________________________
    bool Gtk::gtk_widget_is_applet( GtkWidget* widget )
    {
        if( !widget ) return false;

        #if OXYGEN_DEBUG
        std::cerr << "Gtk::gtk_widget_is_applet(): " << Gtk::gtk_widget_path(widget) << std::endl;
        #endif

        static const char* names[] =
        {
            "Panel",
            "PanelWidget",
            "PanelApplet",
            "XfcePanelWindow",
            0
        };

        // check widget name
        std::string name( G_OBJECT_TYPE_NAME( widget ) );
        for( unsigned int i = 0; names[i]; ++i )
        { if( g_object_is_a( G_OBJECT( widget ), names[i] ) || name.find( names[i] ) == 0  ) return true; }

        // also check parent
        if( GtkWidget* parent = gtk_widget_get_parent( widget ) )
        {
            name = G_OBJECT_TYPE_NAME( parent );
            for( unsigned int i = 0; names[i]; ++i )
            { if( g_object_is_a( G_OBJECT( parent ), names[i] ) || name.find( names[i] ) == 0 ) return true; }

        }

        // also check first widget path element (needed for xfce panel)
        std::string widgetPath=Gtk::gtk_widget_path(widget);
        {
            for( unsigned int i = 0; names[i]; ++i )
            {
                if( widgetPath.find(names[i]) != std::string::npos )
                    return true;
            }
        }

        return false;

    }

    //____________________________________________________________
    void Gtk::gtk_widget_print_tree( GtkWidget* widget )
    {

        if( !widget ) return;
        std::cerr << "Oxygen::Gtk::gtk_widget_print_tree - widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        while( ( widget = gtk_widget_get_parent( widget ) ) )
        { std::cerr << "    parent: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl; }

    }

    //________________________________________________________
    bool Gtk::gdk_default_screen_is_composited( void )
    {
        GdkScreen* screen( gdk_screen_get_default() );
        return (screen && gdk_screen_is_composited( screen ) );
    }

    //________________________________________________________
    bool Gtk::gtk_widget_has_rgba( GtkWidget* widget )
    {

        if( !widget ) return false;
        if( !gdk_default_screen_is_composited() ) return false;
        return gdk_visual_has_rgba( gtk_widget_get_visual (widget) );

    }

    //________________________________________________________
    bool Gtk::gdk_window_is_base( GdkWindow* window )
    {

        if( !GDK_IS_WINDOW( window ) ) return false;

        GdkWindowTypeHint hint = gdk_window_get_type_hint( window );

        #if OXYGEN_DEBUG
        std::cerr << "Gtk::gdk_window_is_base - " << TypeNames::windowTypeHint( hint ) << std::endl;
        #endif

        return(
            hint == GDK_WINDOW_TYPE_HINT_NORMAL ||
            hint == GDK_WINDOW_TYPE_HINT_DIALOG ||
            hint == GDK_WINDOW_TYPE_HINT_UTILITY );
    }

    //________________________________________________________
    bool Gtk::gdk_window_nobackground( GdkWindow* window )
    {
        if( !GDK_IS_WINDOW( window ) ) return false;

        GdkWindowTypeHint hint = gdk_window_get_type_hint( window );
        return( hint == GDK_WINDOW_TYPE_HINT_COMBO || hint == GDK_WINDOW_TYPE_HINT_TOOLTIP );

    }

    //________________________________________________________
    bool Gtk::gdk_window_has_rgba( GdkWindow* window )
    {

        if( !window ) return false;

        if( !gdk_default_screen_is_composited() ) return false;
        return gdk_visual_has_rgba( gdk_drawable_get_visual( GDK_DRAWABLE( window ) ) );

    }

    //________________________________________________________
    bool Gtk::gdk_visual_has_rgba( GdkVisual* visual )
    {
        return
            visual->depth == 32 &&
            visual->red_mask   == 0xff0000 &&
            visual->green_mask == 0x00ff00 &&
            visual->blue_mask  == 0x0000ff;

    }

    //________________________________________________________
    bool Gtk::g_object_is_a( const GObject* object, const std::string& typeName )
    {

        if( object )
        {
            const GType tmp( g_type_from_name( typeName.c_str() ) );
            if( tmp )
            { return g_type_check_instance_is_a( (GTypeInstance*) object, tmp ); }
        }

        return false;
    }

    //________________________________________________________
    std::string Gtk::gtk_widget_path( GtkWidget* widget )
    {

        if(GTK_IS_WIDGET(widget))
        {
            gchar* widgetPath;
            gtk_widget_path( widget, 0L, &widgetPath, 0L);
            const std::string  out( widgetPath );
            g_free( widgetPath );
            return out;
        }
        return std::string("not-widget");

    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_widget_find_parent( GtkWidget* widget, GType type )
    {

        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        { if( G_TYPE_CHECK_INSTANCE_TYPE( parent, type ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_parent_groupbox( GtkWidget* widget )
    {

        for( GtkWidget* parent = widget; parent; parent = gtk_widget_get_parent( parent ) )
        { if( gtk_widget_is_groupbox( parent ) ) return parent; }

        return 0L;
    }

    //________________________________________________________
    bool Gtk::gtk_widget_is_parent( GtkWidget* widget, GtkWidget* potentialParent )
    {

        for( GtkWidget* parent = gtk_widget_get_parent( widget ); parent; parent = gtk_widget_get_parent( parent ) )
        { if( potentialParent==parent ) return true; }

        return false;
    }

    //________________________________________________________
    bool Gtk::gtk_parent_is_shadow_in( GtkWidget* widget )
    {
        for( GtkWidget* parent = gtk_widget_get_parent( widget ); parent; parent = gtk_widget_get_parent( parent ) )
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
    bool Gtk::gtk_button_is_header( GtkWidget* widget )
    {
        if( !GTK_IS_BUTTON( widget ) ) return false;
        return gtk_parent_tree_view( widget ) || gtk_widget_find_parent( widget, "GimpThumbBox" );
    }

    //________________________________________________________
    bool Gtk::gtk_button_is_in_path_bar( GtkWidget* widget )
    {
        if( !( GTK_IS_BUTTON( widget ) && gtk_widget_get_parent( widget ) ) ) return false;

        std::string name(G_OBJECT_TYPE_NAME( gtk_widget_get_parent( widget ) ) );
        return name == "GtkPathBar" || name == "NautilusPathBar";
    }

    //________________________________________________________
    bool Gtk::gtk_path_bar_button_is_last( GtkWidget* widget )
    {

        GtkWidget* parent( gtk_widget_get_parent( widget ) );

        // get parent and check type
        if( !( parent && GTK_IS_CONTAINER( parent ) ) ) return false;

        // get children
        GList* children( gtk_container_get_children( GTK_CONTAINER( parent ) ) );

        /*
        for some reason, pathbar buttons are ordered in the container in reverse order.
        meaning that the last button (in the pathbar) is stored first in the list.
        */
        bool result = (widget == g_list_first( children )->data );
        if( children ) g_list_free( children );
        return result;

    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_button_find_image(GtkWidget* button)
    {

        // check widget type
        if(!GTK_IS_CONTAINER(button)) return 0L;

        GtkWidget* result( 0L );
        GList* children( gtk_container_get_children( GTK_CONTAINER( button ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {
            if( GTK_IS_IMAGE( child->data ) )
            {
                result = GTK_WIDGET( child->data );
                break;

            } else if( GTK_IS_CONTAINER( child->data ) ) {

                result = gtk_button_find_image( GTK_WIDGET(child->data ) );
                break;

            }

        }

        if( children ) g_list_free( children );
        return result;

    }

    //________________________________________________________
    GtkWidget* Gtk::gtk_button_find_label(GtkWidget* button)
    {

        // check widget type
        if( !GTK_IS_CONTAINER(button) ) return 0L;

        GtkWidget* result( 0L );
        GList* children( gtk_container_get_children( GTK_CONTAINER( button ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {

            if( GTK_IS_LABEL( child->data) )
            {
                result = GTK_WIDGET( child->data );
                break;

            } else if( GTK_IS_CONTAINER( child->data ) ) {

                result = gtk_button_find_image(GTK_WIDGET(child->data));
                break;

            }

        }

        if( children ) g_list_free( children );
        return result;

    }

    //________________________________________________________
    bool Gtk::gtk_combobox_has_frame( GtkWidget* widget )
    {

        GValue val = { 0, };
        g_value_init(&val, G_TYPE_BOOLEAN);
        g_object_get_property( G_OBJECT( widget ), "has-frame", &val );
        return (bool) g_value_get_boolean( &val );

    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_tree_view( GtkWidget* widget )
    {
        // check types and path
        if( !widget && GTK_IS_TREE_VIEW( widget ) && GTK_IS_SCROLLED_WINDOW( gtk_widget_get_parent( widget ) ) ) return false;
        return Gtk::gtk_widget_path( widget ) == "gtk-combobox-popup-window.GtkScrolledWindow.GtkTreeView";
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_scrolled_window( GtkWidget* widget )
    {
        // check types and path
        if( !GTK_IS_SCROLLED_WINDOW(widget) ) return false;
        return Gtk::gtk_widget_path( widget ) == "gtk-combobox-popup-window.GtkScrolledWindow";
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_is_popup( GtkWidget* widget )
    {
        // check types and path
        if( !GTK_IS_WINDOW(widget) ) return false;
        return Gtk::gtk_widget_path( widget ) == "gtk-combobox-popup-window";
    }

    //________________________________________________________
    bool Gtk::gtk_combo_is_viewport( GtkWidget* widget )
    {
        if( !GTK_IS_VIEWPORT(widget) ) return false;
        static const std::string match( "gtk-combo-popup-window" );
        return Gtk::gtk_widget_path( widget ).substr( 0, match.size() ) == match;
    }

    //________________________________________________________
    bool Gtk::gtk_combo_is_frame( GtkWidget* widget )
    {
        if( !GTK_IS_FRAME(widget) ) return false;
        static const std::string match( "gtk-combo-popup-window" );
        return Gtk::gtk_widget_path( widget ).substr( 0, match.size() ) == match;
    }

    //________________________________________________________
    bool Gtk::gtk_combo_is_popup( GtkWidget* widget )
    {
        if( !GTK_IS_WINDOW(widget) ) return false;
        static const std::string match( "gtk-combo-popup-window" );
        return Gtk::gtk_widget_path( widget ) == match;
    }

    //________________________________________________________
    bool Gtk::gtk_combobox_appears_as_list( GtkWidget* widget )
    {
        gboolean appearsAsList;
        gtk_widget_style_get( widget, "appears-as-list", &appearsAsList, NULL );
        return (bool) appearsAsList;
    }

    //________________________________________________________
    bool Gtk::gtk_is_tooltip( GtkWidget* widget )
    {
        if( GTK_IS_TOOLTIP(widget) ) return true;
        const std::string path(Gtk::gtk_widget_path( widget ));
        return  path == "gtk-tooltip" || path == "gtk-tooltips";
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
        const GtkAllocation allocation( gtk_widget_get_allocation( tabLabel ) );
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
            if( !page ) continue;

            // get label
            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );
            if(!tabLabel) continue;

            // get allocted size and compare to position
            const GtkAllocation allocation( gtk_widget_get_allocation( tabLabel ) );

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
            if( !page ) continue;

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
        GList* children( gtk_container_get_children( GTK_CONTAINER( notebook ) ) );
        if( !( gtk_notebook_get_show_tabs( notebook ) && children ) )
        {
            if( children ) g_list_free( children );
            *rect = gdk_rectangle();
            return;
        }

        // free children
        if( children ) g_list_free( children );

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
        const GtkAllocation pageAllocation( gtk_widget_get_allocation( page ) );
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
            if( !page ) continue;

            GtkWidget* label( gtk_notebook_get_tab_label( notebook, page ) );

            #if GTK_CHECK_VERSION(2, 20, 0)
            if( label && !gtk_widget_get_mapped( label ) ) return true;
            #else
            if( label && !GTK_WIDGET_MAPPED( label ) ) return true;
            #endif
        }

        return false;

    }

    //____________________________________________________________
    bool Gtk::gtk_notebook_update_close_buttons(GtkNotebook* notebook)
    {
        int numPages=gtk_notebook_get_n_pages( notebook );
        for( int i = 0; i < numPages; ++i )
        {

            // retrieve page
            GtkWidget* page( gtk_notebook_get_nth_page( notebook, i ) );
            if( !page ) continue;

            // retrieve tab label
            GtkWidget* tabLabel( gtk_notebook_get_tab_label( notebook, page ) );
            if( tabLabel && GTK_IS_CONTAINER( tabLabel ) )
            { gtk_container_adjust_buttons_state( GTK_CONTAINER( tabLabel ) ); }

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
    bool Gtk::gtk_scrolled_window_force_sunken( GtkWidget* widget)
    {

        // FMIconView (from nautilus) always get sunken
        if( g_object_is_a( G_OBJECT( widget ), "FMIconView" ) ) return true;

        // other checks require widget to be of type GtkBin
        if( !GTK_IS_BIN( widget ) ) return false;

        // retrieve child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        if( GTK_IS_TREE_VIEW( child ) || GTK_IS_ICON_VIEW( child ) ) return true;
        else return false;

    }

    //________________________________________________________
    bool Gtk::gdk_window_map_to_toplevel( GdkWindow* window, gint* x, gint* y, gint* w, gint* h, bool frame )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !( window && GDK_IS_WINDOW( window ) ) ) return false;

        // get window size and height
        if( frame ) gdk_toplevel_get_frame_size( window, w, h );
        else gdk_toplevel_get_size( window, w, h );
        Gtk::gdk_window_get_toplevel_origin( window, x, y );
        return ((!w) || *w > 0) && ((!h) || *h>0);

    }

    //________________________________________________________
    bool Gtk::gtk_widget_map_to_toplevel( GtkWidget* widget, gint* x, gint* y, gint* w, gint* h, bool frame )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !widget ) return false;

        // get window
        GdkWindow* window( gtk_widget_get_parent_window( widget ) );
        if( !( window && GDK_IS_WINDOW( window ) ) ) return false;

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

    }

    //________________________________________________________
    bool Gtk::gtk_widget_map_to_parent( GtkWidget* widget, GtkWidget* parent, gint* x, gint* y, gint* w, gint* h )
    {

        // always initialize arguments (to invalid values)
        if( x ) *x=0;
        if( y ) *y=0;
        if( w ) *w = -1;
        if( h ) *h = -1;

        if( !( widget && parent ) ) return false;

        const GtkAllocation allocation( gtk_widget_get_allocation(  parent ) );
        if( w ) *w = allocation.width;
        if( h ) *h = allocation.height;

        int xlocal, ylocal;
        const bool success( gtk_widget_translate_coordinates( widget, parent, 0, 0, &xlocal, &ylocal ) );
        if( success )
        {

            if( x ) *x=xlocal;
            if( y ) *y=ylocal;

        }

        return success && ((!w) || *w > 0) && ((!h) || *h>0);

    }

    //________________________________________________________
    bool Gtk::gdk_window_translate_origin( GdkWindow* parent, GdkWindow* child, gint* x, gint* y )
    {
        if( x ) *x = 0;
        if( y ) *y = 0;
        if( !( parent && child ) ) return false;
        while( child && GDK_IS_WINDOW( child ) &&
            child != parent &&
            gdk_window_get_window_type( child ) == GDK_WINDOW_CHILD )
        {
            gint xloc;
            gint yloc;
            gdk_window_get_position( child, &xloc, &yloc );
            if( x ) *x += xloc;
            if( y ) *y += yloc;
            child = gdk_window_get_parent( child );
        }

        return( child == parent );

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
        while( window && GDK_IS_WINDOW( window ) && gdk_window_get_window_type( window ) == GDK_WINDOW_CHILD )
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

    //___________________________________________________________
    void Gtk::gtk_viewport_get_position( GtkViewport* viewport, gint* x, gint* y )
    {

        // initialize
        if( x ) *x = 0;
        if( y ) *y = 0;

        // get bin window
        #if GTK_CHECK_VERSION( 2, 20, 0 )
        GdkWindow* binWindow( gtk_viewport_get_bin_window( viewport ) );
        #else
        GdkWindow* binWindow( viewport->bin_window );
        #endif

        gint xBin(0), yBin(0);
        gdk_window_get_geometry( binWindow, &xBin, &yBin, 0, 0, 0 );

        // get view window
        #if GTK_CHECK_VERSION( 2, 22, 0 )
        GdkWindow* viewWindow( gtk_viewport_get_view_window( viewport ) );
        #else
        GdkWindow* viewWindow( viewport->view_window );
        #endif

        gint xView(0), yView(0);
        gdk_window_get_geometry( viewWindow, &xView, &yView, 0, 0, 0 );

        // calculate offsets
        if( x ) *x = xView - xBin;
        if( y ) *y = yView - yBin;

        // also correct from widget thickness
        GtkStyle* style( gtk_widget_get_style( GTK_WIDGET( viewport ) ) );
        if( style )
        {
            if( x ) *x -= style->xthickness;
            if( y ) *y -= style->ythickness;
        }

        return;

    }

    //___________________________________________________________
    GtkWidget* Gtk::gtk_dialog_find_button(GtkDialog* dialog,gint response_id)
    {

        // get children of dialog's action area
        GList* children( gtk_container_get_children( GTK_CONTAINER( gtk_dialog_get_action_area( dialog ) ) ) );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Gtk::gtk_dialog_find_button - buttons: ";
        #endif

        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {

            // check data
            if( !GTK_IS_WIDGET( child->data ) ) continue;
            GtkWidget* childWidget( GTK_WIDGET( child->data ) );

            const gint id( gtk_dialog_get_response_for_widget(dialog, childWidget ) );

            #if OXYGEN_DEBUG
            std::cerr << Gtk::TypeNames::response( (GtkResponseType) id ) << ", ";
            #endif
            if( id == response_id ) return childWidget;

        }

        #if OXYGEN_DEBUG
        std::cerr << std::endl;
        #endif

        if( children ) g_list_free( children );
        return 0L;

    }


}
