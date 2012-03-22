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
        _hooksInitialized( false )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ArgbHelper::ArgbHelper" << std::endl;
        #endif
    }


    //_____________________________________________________
    ArgbHelper::~ArgbHelper( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ArgbHelper::~ArgbHelper" << std::endl;
        #endif

        // disconnect hooks
        _styleSetHook.disconnect();
        _styleUpdatedHook.disconnect();
        _parentSetHook.disconnect();

    }

    //_____________________________________________________
    void ArgbHelper::initializeHooks( void )
    {

        if( _hooksInitialized ) return;

        // style hooks
        if( !_styleSetHook.connect( "style-set", (GSignalEmissionHook)styleSetHook, 0L ) )
        { return; }

        if( !_styleUpdatedHook.connect( "style-updated", (GSignalEmissionHook)styleSetHook, 0L ) )
        { return; }

        // parent-set hook
        _parentSetHook.connect( "parent-set", (GSignalEmissionHook)parentSetHook, this );

        _hooksInitialized = true;
        return;

    }

    //_____________________________________________________
    void ArgbHelper::attachStyle( GtkWidget* widget, GdkWindow* window ) const
    {

        // retrieve widget style and check
        GtkStyle* style( gtk_widget_get_style( widget ) );
        if( !style ) return;

        // TODO: check whether there is an equivalent for Gtk3
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ArgbHelper::attachStyle -"
            << " widget: " << widget << " (" <<G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        // TODO: check whether this is needed for gtk+3, and if yes, if working
        // This is known *not* to work for gtk+2
        // gtk_widget_set_style( widget, gtk_style_attach( style, window ) );

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

    //_________________________________________________________________
    bool ArgbHelper::acceptWidget( GtkWidget* widget )
    {

        // check type
        if( !GTK_IS_WINDOW( widget ) ) return false;

        // if widget is already realized, then its tool late
        if( gtk_widget_get_realized( widget ) ) return false;

        // cast to window and check type hint
        GtkWindow* window( GTK_WINDOW( widget ) );
        GdkWindowTypeHint hint = gtk_window_get_type_hint( window );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ArgbHelper::acceptWidget - "
            << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " hint: " << Gtk::TypeNames::windowTypeHint( hint )
            << std::endl;
        #endif

        if(
            hint == GDK_WINDOW_TYPE_HINT_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_DROPDOWN_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_POPUP_MENU ||
            hint == GDK_WINDOW_TYPE_HINT_TOOLTIP ||
            hint == GDK_WINDOW_TYPE_HINT_COMBO
            )
        { return true; }

        // also manually check for menu
        if( GTK_IS_MENU( gtk_bin_get_child( GTK_BIN( widget ) ) ) )
        { return true; }

        return false;
    }

    //_____________________________________________________
    gboolean ArgbHelper::styleSetHook( GSignalInvocationHint*, guint, const GValue* params, gpointer )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // check widget validity
        if( !acceptWidget( widget ) ) return TRUE;

        // screen
        GdkScreen* screen = gdk_screen_get_default();
        if( !screen ) return TRUE;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ArgbHelper::styleSetHook - "
            << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " hint: " << Gtk::TypeNames::windowTypeHint( gtk_window_get_type_hint( GTK_WINDOW( widget ) ) )
            << std::endl;
        #endif

        // assign argb colormap to widget
        gtk_widget_set_visual( widget, gdk_screen_get_rgba_visual( screen ) );

        return TRUE;

    }

    //_____________________________________________________
    gboolean ArgbHelper::parentSetHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
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

}
