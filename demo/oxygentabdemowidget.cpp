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

#include "oxygentabdemowidget.h"

#include <iostream>
#include <string>

namespace Oxygen
{

    //____________________________________________________
    TabDemoWidget::TabDemoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 5 );
        setWidget( mainWidget );

        // setup
        setName( "Tab Widgets" );
        setComments( "Shows the appearance of tab widgets" );
        setIconName( "tab-detach" );
        realize();

        // orientation
        {
            GtkWidget* hbox( gtk_hbox_new( false, 0 ) );
            gtk_box_set_spacing( GTK_BOX( hbox ), 5 );
            gtk_box_pack_start( GTK_BOX( mainWidget ), hbox, false, true, 0 );
            gtk_widget_show( hbox );

            // label
            GtkWidget* label( gtk_label_new( "Tab orientation: " ) );
            gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
            gtk_box_pack_start( GTK_BOX( hbox ), label, false, true, 0 );
            gtk_widget_show( label );

            // combobox
            GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
            const char* columns[] =
            {
                "North",
                "South",
                "West",
                "East"
            };

            for( unsigned int i=0; i<4; i++ )
            {
                GtkTreeIter iter;
                gtk_list_store_append( model, &iter );
                gtk_list_store_set( model, &iter, 0, columns[i], -1 );
            }

            GtkWidget* comboBox( gtk_combo_box_new() );
            gtk_combo_box_set_model( GTK_COMBO_BOX( comboBox ), GTK_TREE_MODEL( model ) );
            g_object_unref( model );

            // text renderer
            GtkCellRenderer* cell( gtk_cell_renderer_text_new() );
            gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( comboBox ), cell, FALSE );
            gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( comboBox ), cell, "text", 0, NULL );

            gtk_combo_box_set_active( GTK_COMBO_BOX( comboBox ), 0 );
            gtk_box_pack_start( GTK_BOX( hbox ), comboBox, false, true, 0 );
            gtk_widget_show( comboBox );

            // connection
            connect( G_OBJECT( comboBox ), "changed", G_CALLBACK( orientationChanged ), this );

        }

        {

            // frame
            GtkWidget* frame( gtk_frame_new( "Preview" ) );
            gtk_box_pack_start( GTK_BOX( mainWidget ), frame, true, true, 0 );
            gtk_widget_show( frame );

            _notebook = gtk_notebook_new();
            gtk_container_add( GTK_CONTAINER( frame ), _notebook );
            gtk_container_set_border_width( GTK_CONTAINER( _notebook ), 2 );
            gtk_notebook_set_tab_pos( GTK_NOTEBOOK( _notebook ), GTK_POS_TOP );
            gtk_widget_show( _notebook );

            {

                // first page
                GtkWidget* vbox( gtk_vbox_new( false, 0 ) );
                gtk_box_set_spacing( GTK_BOX( vbox ), 5 );

                // line editor
                GtkWidget* lineEditor( gtk_entry_new() );
                gtk_box_pack_start( GTK_BOX( vbox ), lineEditor, false, true, 0 );
                gtk_widget_show( lineEditor );

                // text editor
                GtkTextTagTable* tags( gtk_text_tag_table_new() );
                GtkTextBuffer* buffer( gtk_text_buffer_new( tags ) );
                GtkWidget* textView( gtk_text_view_new_with_buffer( buffer ) );
                gtk_container_set_border_width( GTK_CONTAINER( textView ), 2 );
                gtk_widget_show( textView );

                GtkWidget* scrolledWindow( gtk_scrolled_window_new( 0L, 0L ) );
                gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_SHADOW_IN );
                gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
                gtk_container_add( GTK_CONTAINER( scrolledWindow ), textView );
                gtk_container_set_border_width( GTK_CONTAINER( scrolledWindow ), 2 );
                gtk_widget_show( scrolledWindow );

                gtk_box_pack_start( GTK_BOX( vbox ), scrolledWindow, true, true, 0 );

                addPage( "First Tab", vbox );


            }
            addPage( "Second Tab", gtk_vbox_new( false, 0 ) );
            addPage( "Third Tab", gtk_vbox_new( false, 0 ) );
            addPage( "Fourth Tab", gtk_vbox_new( false, 0 ) );

        }
    }

    //____________________________________________________
    TabDemoWidget::~TabDemoWidget( void )
    {}

    //____________________________________________________
    void TabDemoWidget::addPage( const std::string& title, GtkWidget* widget ) const
    {
        assert( _notebook );
        GtkWidget* label( gtk_label_new( title.c_str() ) );
        gtk_notebook_append_page( GTK_NOTEBOOK( _notebook ), widget, label );
        gtk_widget_show( label );
        gtk_widget_show( widget );
        gtk_notebook_set_tab_reorderable( GTK_NOTEBOOK( _notebook ), widget, true );
    }

    //____________________________________________________
    void TabDemoWidget::orientationChanged( GtkComboBox* comboBox, gpointer data )
    {
        GtkNotebook* notebook( GTK_NOTEBOOK( static_cast<TabDemoWidget*>( data )->_notebook ) );
        switch( gtk_combo_box_get_active( comboBox ) )
        {
            default:
            case 0: gtk_notebook_set_tab_pos( notebook, GTK_POS_TOP ); break;
            case 1: gtk_notebook_set_tab_pos( notebook, GTK_POS_BOTTOM ); break;
            case 2: gtk_notebook_set_tab_pos( notebook, GTK_POS_LEFT ); break;
            case 3: gtk_notebook_set_tab_pos( notebook, GTK_POS_RIGHT ); break;
        }

    }

}
