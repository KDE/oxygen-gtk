/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenframedemowidget.h"

#include <iostream>
#include <string>

namespace Oxygen
{

    //____________________________________________________
    FrameDemoWidget::FrameDemoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 5 );
        setWidget( mainWidget );

        // setup
        setName( "Frames" );
        setComments( "Shows the appearance of various framed widgets" );
        setIconName( "draw-rectangle" );
        realize();

        // orientation
        {
            GtkWidget* hbox( gtk_hbox_new( false, 0 ) );
            gtk_box_set_spacing( GTK_BOX( hbox ), 5 );
            gtk_box_pack_start( GTK_BOX( mainWidget ), hbox, false, true, 0 );
            gtk_widget_show( hbox );

            // label
            GtkWidget* label( gtk_label_new( "Layout orientation: " ) );
            gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
            gtk_box_pack_start( GTK_BOX( hbox ), label, false, true, 0 );
            gtk_widget_show( label );

            // combobox
            GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
            const char* columns[] =
            {
                "Horizontal",
                "Vertical"
            };

            for( unsigned int i=0; i<2; i++ )
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
            _changedId.connect( G_OBJECT( comboBox ), "changed", G_CALLBACK( orientationChanged ), this );

        }

        {
            // box
            _box = gtk_hbox_new( true, 0 );
            gtk_box_set_spacing( GTK_BOX( _box ), 5 );
            gtk_box_pack_start( GTK_BOX( mainWidget ), _box, true, true, 0 );
            gtk_widget_show( _box );

            // named frame
            {
                GtkWidget* frame( gtk_frame_new( "GroupBox" ) );
                gtk_box_pack_start( GTK_BOX( _box ), frame, true, true, 0 );
                gtk_widget_show( frame );

            }

            // unnamed frame
            {
                _frame = gtk_frame_new( 0L );
                gtk_frame_set_shadow_type( GTK_FRAME( _frame ), GTK_SHADOW_OUT );
                gtk_box_pack_start( GTK_BOX( _box ), _frame, true, true, 0 );
                gtk_widget_show( _frame );

                GtkWidget* vbox( gtk_vbox_new( false, 0 ) );
                gtk_container_add( GTK_CONTAINER( _frame ), vbox );
                gtk_container_set_border_width( GTK_CONTAINER( vbox ), 5 );
                gtk_widget_show( vbox );

                GtkWidget* label( gtk_label_new( "Frame" ) );
                gtk_box_pack_start( GTK_BOX( vbox ), label, false, true, 0 );
                gtk_widget_show( label );

                Signal signal;

                // radio buttons
                GtkWidget* radiobutton;
                gtk_box_pack_start( GTK_BOX( vbox ), radiobutton = gtk_radio_button_new_with_label( 0L, "Raised" ), false, true, 0 );
                gtk_widget_show( radiobutton );
                _widgets.insert( std::make_pair( radiobutton, GTK_SHADOW_OUT ) );
                g_signal_connect( G_OBJECT( radiobutton ), "toggled", G_CALLBACK( shadowChanged ), this );

                gtk_box_pack_start( GTK_BOX( vbox ), radiobutton = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( radiobutton ), "Etched" ), false, true, 0 );
                gtk_widget_show( radiobutton );
                _widgets.insert( std::make_pair( radiobutton, GTK_SHADOW_ETCHED_IN ) );
                g_signal_connect( G_OBJECT( radiobutton ), "toggled", G_CALLBACK( shadowChanged ), this );

                gtk_box_pack_start( GTK_BOX( vbox ), radiobutton = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( radiobutton ), "Sunken" ), false, true, 0 );
                gtk_widget_show( radiobutton );
                _widgets.insert( std::make_pair( radiobutton, GTK_SHADOW_IN ) );
                g_signal_connect( G_OBJECT( radiobutton ), "toggled", G_CALLBACK( shadowChanged ), this );

            }

            // notebook
            {
                GtkWidget* notebook( gtk_notebook_new() );
                gtk_box_pack_start( GTK_BOX( _box ), notebook, true, true, 0 );
                gtk_widget_show( notebook );

                // add empty page
                GtkWidget* label( gtk_label_new( "Tab Widget" ) );
                GtkWidget* vbox( gtk_vbox_new( false, 0 ) );
                gtk_notebook_append_page( GTK_NOTEBOOK( notebook ), vbox, label );
                gtk_widget_show( label );
                gtk_widget_show( vbox );
            }

        }
    }

    //____________________________________________________
    FrameDemoWidget::~FrameDemoWidget( void )
    {}

    //____________________________________________________
    void FrameDemoWidget::shadowChanged( GtkToggleButton* button, gpointer pointer )
    {
        FrameDemoWidget& data( *static_cast<FrameDemoWidget*>( pointer ) );
        if( !gtk_toggle_button_get_active( button ) ) return;

        WidgetMap::const_iterator iter( data._widgets.find( GTK_WIDGET( button ) ) );
        if( iter == data._widgets.end() ) return;
        gtk_frame_set_shadow_type( GTK_FRAME( data._frame ), iter->second );

    }

    //____________________________________________________
    void FrameDemoWidget::orientationChanged( GtkComboBox* comboBox, gpointer pointer )
    {
        const gint id( gtk_combo_box_get_active( comboBox ) );
        gtk_orientable_set_orientation(
            GTK_ORIENTABLE( static_cast<FrameDemoWidget*>( pointer )->_box ),
            id == 0 ? GTK_ORIENTATION_HORIZONTAL : GTK_ORIENTATION_VERTICAL );
    }

}
