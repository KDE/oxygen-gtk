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

#include "oxygentreeviewengine.h"

namespace Oxygen
{


    //____________________________________________________________________
    TreeViewEngine::TreeViewEngine( Animations* parent ):
        GenericEngine<TreeViewData>( parent ),
        _cursorLoaded( false ),
        _cursor( 0L )
    {}

    //____________________________________________________________________
    TreeViewEngine::~TreeViewEngine( void )
    { if( _cursor ) gdk_cursor_unref( _cursor ); }

    //____________________________________________________________________
    bool TreeViewEngine::registerWidget( GtkWidget* widget )
    {
        if( !GenericEngine<TreeViewData>::registerWidget( widget ) ) return false;
        else if( GTK_IS_TREE_VIEW( widget ) )
        {

            // always disable treelines.
            /* tree lines are rendered by the style, in order to be able to draw them with custom color */
            GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
            gtk_tree_view_set_enable_tree_lines( treeView, false );

            GtkWidget* parent( gtk_widget_get_parent( widget ) );
            GtkScrolledWindow* scrolledWindow(0);
            if(
                GTK_IS_SCROLLED_WINDOW( parent ) &&
                gtk_scrolled_window_get_shadow_type( (scrolledWindow = GTK_SCROLLED_WINDOW( parent ) ) ) != GTK_SHADOW_IN &&
                !Gtk::gtk_parent_is_shadow_in( parent ) )
            { gtk_scrolled_window_set_shadow_type( scrolledWindow, GTK_SHADOW_IN ); }

            // try load cursor
            if( !_cursorLoaded )
            {

                assert( !_cursor );

                // get display
                GdkDisplay *display( gdk_display_get_default () );
                _cursor = gdk_cursor_new_from_name( display, "row-resize" );
                _cursorLoaded = true;

            }

            if( _cursor )
            {
                GList* children( gtk_tree_view_get_columns( treeView ) );
                for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
                {
                    if( GTK_IS_TREE_VIEW_COLUMN( child->data ) )
                    {
                        GdkWindow* window( GTK_TREE_VIEW_COLUMN( child->data )->window );
                        gdk_window_set_cursor( window, _cursor );
                    }
                }

                if( children ) g_list_free( children );

            }


        }

        return true;

    }


}
