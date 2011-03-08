/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* Hook-setup code provided by Ruslan
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

#include "oxygenargbhelper.h"
#include "oxygengtktypenames.h"
#include "config.h"

#include <gtk/gtk.h>

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //__________________________________________________________________
    ArgbHelper::ArgbHelper( void ):
        _hooksInitialized( false ),
        _logId( 0 )
    {}


    //_____________________________________________________
    ArgbHelper::~ArgbHelper( void )
    {

        // disconnect hooks
        _colormapHook.disconnect();
        _styleHook.disconnect();

        if( _logId > 0 )
        {

            g_log_remove_handler( "GLib-GObject", _logId );
            g_log_set_handler( "GLib-GObject", G_LOG_LEVEL_CRITICAL, g_log_default_handler, 0L );

        }

    }

    //_____________________________________________________
    void ArgbHelper::initializeHooks( void )
    {

        if( _hooksInitialized ) return;

        // lookup relevant signal
        const guint signalId( g_signal_lookup("style-set", GTK_TYPE_WINDOW ) );
        if( signalId <= 0 ) return;

        // install hooks
        _colormapHook.connect( "style-set", (GSignalEmissionHook)colormapHook, 0L );
        _styleHook.connect( "parent-set", (GSignalEmissionHook)styleHook, this );

        /*
        the installation of "parent-set" hook triggers some glib critical error when destructing ComboBoxEntry.
        a glib/gtk bug has been filed. In the meanwhile, and since the error is apparently harmless, we simply
        disable the corresponding log
        */
        _logId = g_log_set_handler( "GLib-GObject", G_LOG_LEVEL_CRITICAL, logHandler, 0L );

        _hooksInitialized = true;
        return;

    }

    //_____________________________________________________
    void ArgbHelper::attachStyle( GtkWidget* widget, GdkWindow* window ) const
    {

        // retrieve widget style and check
        GtkStyle* style( gtk_widget_get_style( widget ) );
        if( !( style && style->depth >= 0 ) ) return;

        // adjust depth
        if( style->depth == gdk_drawable_get_depth( window ) )
        { return; }

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ArgbHelper::attachStyle -"
            << " widget: " << widget << " (" <<G_OBJECT_TYPE_NAME( widget ) << ")"
            << " style depth: " << style->depth
            << " window depth: " << gdk_drawable_get_depth( window )
            << std::endl;
        #endif

        widget->style = gtk_style_attach( style, window );

        // if widget is a container, we need to do the same for its children
        if( !GTK_IS_CONTAINER( widget ) ) return;

        // get children
        GList* children( gtk_container_get_children( GTK_CONTAINER( widget ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {
            if( !GTK_IS_WIDGET( child->data ) ) continue;
            attachStyle( GTK_WIDGET( child->data ), window );
        }

        if( children ) g_list_free( children );

    }



    //_____________________________________________________
    gboolean ArgbHelper::colormapHook( GSignalInvocationHint*, guint, const GValue* params, gpointer )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;
        if( !GTK_IS_WINDOW( widget ) ) return TRUE;

        // make sure widget has not been realized already
        #if GTK_CHECK_VERSION(2, 20, 0)
        if( gtk_widget_get_realized( widget ) ) return TRUE;
        #else
        if( GTK_WIDGET_REALIZED( widget ) ) return TRUE;
        #endif

        // cast to window
        GtkWindow* window( GTK_WINDOW( widget ) );

        // check type hint
        GdkWindowTypeHint hint = gtk_window_get_type_hint( window );

        // screen
        GdkScreen* screen = gdk_screen_get_default();
        if( !screen ) return TRUE;

        if(
            hint == GDK_WINDOW_TYPE_HINT_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_POPUP_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_TOOLTIP ||
            hint == GDK_WINDOW_TYPE_HINT_COMBO
            )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::ArgbHelper::colormapHook - "
                << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << " hint: " << Gtk::TypeNames::windowTypeHint( hint )
                << std::endl;
            #endif

            // assign argb colormap to widget
            gtk_widget_set_colormap( widget, gdk_screen_get_rgba_colormap( screen ) );

        }

        return TRUE;

    }

    //_____________________________________________________
    gboolean ArgbHelper::styleHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // retrieve parent window and check
        GdkWindow* window( gtk_widget_get_parent_window( widget ) );
        if( !window ) return TRUE;

        static_cast<const ArgbHelper*>(data)->attachStyle( widget, window );
        return TRUE;

    }

    //_________________________________________________________
    void ArgbHelper::logHandler( const gchar* domain, GLogLevelFlags flags, const gchar* message, gpointer data )
    {

        /*
        discard all messages containing "IA__gtk_box_reorder_child:"
        and fallback to default handler otherwise
        */
        if( std::string( message ).find( "g_object_ref" ) == std::string::npos )
        { g_log_default_handler( domain, flags, message, data ); }

    }

}
