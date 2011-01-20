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
#include <sys/stat.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace Oxygen
{

    //__________________________________________________________________
    ArgbHelper::ArgbHelper( void ):
        _hooksInitialized( false )
    {}


    //_____________________________________________________
    ArgbHelper::~ArgbHelper( void )
    {

        // disconnect hooks
        _colormapHook.disconnect();
        _styleHook.disconnect();

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
        _styleHook.connect( "parent-set", (GSignalEmissionHook)styleHook, 0L );

        _hooksInitialized = true;
        return;

    }

    //_____________________________________________________
    gboolean ArgbHelper::colormapHook( GSignalInvocationHint*, guint, const GValue* params, gpointer* )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;
        if( !GTK_IS_WINDOW( widget ) ) return TRUE;

        // make sure widget has not been realized already
        if( gtk_widget_get_realized( widget ) ) return TRUE;

        // cast to window
        GtkWindow* window( GTK_WINDOW( widget ) );

        // check type hint
        GdkWindowTypeHint hint = gtk_window_get_type_hint( window );

        // screen
        GdkScreen* screen = gdk_screen_get_default();
        if( !screen ) return TRUE;

        if(
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

            GdkColormap* cmap=gdk_screen_get_rgba_colormap( screen );
            gtk_widget_push_colormap( cmap );
            gtk_widget_set_colormap( widget, cmap );

        } else {

            GdkColormap* cmap=gdk_screen_get_rgb_colormap( screen );
            gtk_widget_push_colormap( cmap );
            gtk_widget_set_colormap( widget, cmap );

        }

        return TRUE;

    }

    //_____________________________________________________
    gboolean ArgbHelper::styleHook( GSignalInvocationHint*, guint, const GValue* params, gpointer* )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // retrieve widget style and check
        GtkStyle* style( widget->style );
        if( !( style && style->depth >= 0 ) ) return TRUE;

        // retrieve parent window and check
        GdkWindow* window( gtk_widget_get_parent_window( widget ) );
        if( !window ) return TRUE;

        // adjust depth
        if( style->depth != gdk_drawable_get_depth( window ) )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::ArgbHelper::styleHook -"
                << " widget: " << widget << " (" <<G_OBJECT_TYPE_NAME( widget ) << ")"
                << " style depth: " << style->depth
                << " window depth: " << gdk_drawable_get_depth( window )
                << std::endl;
            #endif

            widget->style = gtk_style_attach( style, window );

        }

        return TRUE;

    }

}
