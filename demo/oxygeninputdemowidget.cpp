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

#include "oxygeninputdemowidget.h"

namespace Oxygen
{

    //____________________________________________________
    InputDemoWidget::InputDemoWidget( void )
    {

        // main widget
        _mainWidget = gtk_vbox_new( false, 0 );

        // create top table
        GtkWidget* table = gtk_table_new( 5, 2, false );
        gtk_table_set_row_spacings( GTK_TABLE( table ), 5 );
        gtk_box_pack_start( GTK_BOX( _mainWidget ), table, false, true, 0 );
        gtk_widget_show( table );

        // generic label
        GtkWidget* label( 0L );

        // simple line editor
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Single line text editor: " ), 0, 1, 0, 1, GTK_SHRINK, GTK_FILL, 2, 2  );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        GtkWidget* lineEditor( 0L );
        gtk_table_attach_defaults( GTK_TABLE( table ), lineEditor = gtk_entry_new(), 1, 2, 0, 1 );
        gtk_entry_set_text( GTK_ENTRY( lineEditor ), "Example text" );
        gtk_widget_show( lineEditor );

        // invisible line editor
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Password editor: " ), 0, 1, 1, 2, GTK_SHRINK, GTK_FILL, 2, 2 );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        gtk_table_attach_defaults( GTK_TABLE( table ), lineEditor = gtk_entry_new(), 1, 2, 1, 2 );
        gtk_entry_set_text( GTK_ENTRY( lineEditor ), "Example text" );
        gtk_entry_set_visibility( GTK_ENTRY( lineEditor ), false );
        gtk_widget_show( lineEditor );

        // combobox
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Editable combobox: " ), 0, 1, 2, 3, GTK_SHRINK, GTK_FILL, 2, 2 );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        // model
        GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
        const char* columns[] = {
            "First item",
            "Second item",
            "Third item"
        };

        for( unsigned int i=0; i<3; i++ )
        {
            GtkTreeIter iter;
            gtk_list_store_append( model, &iter );
            gtk_list_store_set( model, &iter, 0, columns[i], -1 );
        }

        // combobox
        GtkWidget* comboBox(0L);
        gtk_table_attach_defaults( GTK_TABLE( table ), comboBox = gtk_combo_box_new_with_model_and_entry( GTK_TREE_MODEL( model ) ), 1, 2, 2, 3 );
        gtk_combo_box_set_entry_text_column( GTK_COMBO_BOX( comboBox ), 0 );
        gtk_widget_show( comboBox );

        // separator
        GtkWidget* separator( gtk_hseparator_new() );
        gtk_box_pack_start( GTK_BOX( _mainWidget ), separator, false, true, 0 );
        gtk_widget_show( separator );

        // text view
        gtk_box_pack_start( GTK_BOX( _mainWidget ), label = gtk_label_new( "Multi-line text editor: " ), false, true, 0 );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        GtkWidget* textView( gtk_text_view_new() );
        gtk_box_pack_start( GTK_BOX( _mainWidget ), textView, true, true, 0 );
        gtk_widget_show( textView );

    }

    //____________________________________________________
    InputDemoWidget::~InputDemoWidget( void )
    {}

}
