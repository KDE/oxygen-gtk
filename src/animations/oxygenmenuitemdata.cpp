/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygenmenuitemdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void MenuItemData::connect( GtkWidget* widget )
    {
        _target = widget;
        _parentSetId.connect( G_OBJECT(widget), "parent-set", G_CALLBACK( parentSet ), this);
    }

    //________________________________________________________________________________
    void MenuItemData::disconnect( GtkWidget* widget )
    {
        _target = 0L;
        _parentSetId.disconnect();
    }

    //_____________________________________________________
    void MenuItemData::parentSet( GtkWidget* widget, GtkWidget*, gpointer data )
    {

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return;

        // retrieve parent window and check
        GdkWindow* window( gtk_widget_get_parent_window( widget ) );
        if( !window ) return;

        static_cast<const MenuItemData*>(data)->attachStyle( widget, window );
        return;

    }

    //_____________________________________________________
    void MenuItemData::attachStyle( GtkWidget* widget, GdkWindow* window ) const
    {

        // retrieve widget style and check
        GtkStyle* style( gtk_widget_get_style( widget ) );
        if( !( style && style->depth >= 0 ) ) return;

        // adjust depth
        if( style->depth == gdk_drawable_get_depth( window ) )
        { return; }

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::MenuItemData::attachStyle -"
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


}
