/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#include "oxygenlistdemowidget.h"

#include <iostream>
#include <string>

namespace Oxygen
{

    //____________________________________________________
    ListDemoWidget::ListDemoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 5 );
        setWidget( mainWidget );

        // setup
        setName( "Lists" );
        setComments( "Shows the appearance of lists and trees" );
        setIconName( "view-list-tree" );
        realize();

        // vertical pane
        GtkWidget* vpaned( gtk_vpaned_new() );
        gtk_widget_show( vpaned );
        gtk_box_pack_start( GTK_BOX( mainWidget ), vpaned, true, true, 0 );

        // simple list
        {

            GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
            const char* columns[] =
            {
                "First Item",
                "Second Item",
                "Third Item"
            };

            for( unsigned int i=0; i<3; i++ )
            {
                GtkTreeIter iter;
                gtk_list_store_append( model, &iter );
                gtk_list_store_set( model, &iter, 0, columns[i], -1 );
            }

            GtkWidget* treeView( gtk_tree_view_new_with_model( GTK_TREE_MODEL( model ) ) );
            gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( treeView ), false );
            gtk_widget_show( treeView );

            // renderer
            GtkCellRenderer *renderer( gtk_cell_renderer_text_new() );
            GtkTreeViewColumn *column( gtk_tree_view_column_new_with_attributes( "", renderer, "text", 0, NULL ) );

            gtk_tree_view_append_column( GTK_TREE_VIEW( treeView ), column );

            // scrolled window
            GtkWidget* scrolledWindow( gtk_scrolled_window_new( 0L, 0L ) );
            gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_SHADOW_IN );
            gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
            gtk_container_add( GTK_CONTAINER( scrolledWindow ), treeView );
            gtk_container_set_border_width( GTK_CONTAINER( scrolledWindow ), 2 );
            gtk_widget_show( scrolledWindow );

            gtk_paned_pack1( GTK_PANED( vpaned ), scrolledWindow, true, true );

        }

        // tree
        {

            GtkTreeStore* model( gtk_tree_store_new ( 2, G_TYPE_STRING, G_TYPE_STRING ) );
            const char* titleColumns[] =
            {
                "First Item",
                "Second Item",
                "Third Item"
            };

            const char* descriptionColumns[] =
            {
                "First Description",
                "Second Description",
                "Third Description"
            };

            const char* subTitleColumns[] =
            {
                "First Subitem",
                "Second Subitem",
                "Third Subitem"
            };

            const char* subDescriptionColumns[] =
            {
                "First Subitem Description",
                "Second Subitem Description",
                "Third Subitem Description"
            };

            for( unsigned int i=0; i<3; i++ )
            {
                GtkTreeIter iter;
                gtk_tree_store_append( model, &iter, 0L );
                gtk_tree_store_set( model, &iter, 0, titleColumns[i], 1, descriptionColumns[i], -1 );

                // append children
                if( i == 1 )
                {
                    for( unsigned int i=0; i<2; i++ )
                    {
                        GtkTreeIter subiter;
                        gtk_tree_store_append( model, &subiter, &iter );
                        gtk_tree_store_set( model, &subiter, 0, subTitleColumns[i], 1, subDescriptionColumns[i], -1 );
                    }

                } else if( i == 2 ) {

                    GtkTreeIter subiter;
                    gtk_tree_store_append( model, &subiter, &iter );
                    gtk_tree_store_set( model, &subiter, 0, subTitleColumns[2], 1, subDescriptionColumns[2], -1 );

                }

            }

            GtkWidget* treeView( gtk_tree_view_new_with_model( GTK_TREE_MODEL( model ) ) );
            gtk_tree_view_set_headers_visible( GTK_TREE_VIEW( treeView ), true );
            gtk_widget_show( treeView );

            // renderers
            {
                GtkCellRenderer *renderer( gtk_cell_renderer_text_new() );
                GtkTreeViewColumn *column( gtk_tree_view_column_new_with_attributes( "Title", renderer, "text", 0, NULL ) );
                gtk_tree_view_column_set_resizable(column, TRUE);
                gtk_tree_view_append_column( GTK_TREE_VIEW( treeView ), column );
            }

            {
                GtkCellRenderer *renderer( gtk_cell_renderer_text_new() );
                GtkTreeViewColumn *column( gtk_tree_view_column_new_with_attributes( "Description", renderer, "text", 1, NULL ) );
                gtk_tree_view_column_set_resizable(column, TRUE);
                gtk_tree_view_append_column( GTK_TREE_VIEW( treeView ), column );
            }

            // scrolled window
            GtkWidget* scrolledWindow( gtk_scrolled_window_new( 0L, 0L ) );
            gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_SHADOW_IN );
            gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
            gtk_container_add( GTK_CONTAINER( scrolledWindow ), treeView );
            gtk_container_set_border_width( GTK_CONTAINER( scrolledWindow ), 2 );
            gtk_widget_show( scrolledWindow );

            gtk_paned_pack2( GTK_PANED( vpaned ), scrolledWindow, true, true );

        }


    }

}
