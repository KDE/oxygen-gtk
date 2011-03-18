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
        _mainWidget = gtk_table_new( 5, 2, false );
        gtk_table_set_row_spacings( GTK_TABLE( _mainWidget ), 5 );
        // generic label
        GtkWidget* label( 0L );

        // simple line editor
        gtk_table_attach( GTK_TABLE( _mainWidget ), label = gtk_label_new( "Single line text editor: " ), 0, 1, 0, 1, GTK_SHRINK, GTK_FILL, 2, 2  );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        GtkWidget* lineEditor( 0L );
        gtk_table_attach_defaults( GTK_TABLE( _mainWidget ), lineEditor = gtk_entry_new(), 1, 2, 0, 1 );
        gtk_entry_set_text( GTK_ENTRY( lineEditor ), "Example text" );
        gtk_widget_show( lineEditor );

        // invisible line editor
        gtk_table_attach( GTK_TABLE( _mainWidget ), label = gtk_label_new( "Password editor: " ), 0, 1, 1, 2, GTK_SHRINK, GTK_FILL, 2, 2 );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        gtk_table_attach_defaults( GTK_TABLE( _mainWidget ), lineEditor = gtk_entry_new(), 1, 2, 1, 2 );
        gtk_entry_set_text( GTK_ENTRY( lineEditor ), "Example text" );
        gtk_entry_set_visibility( GTK_ENTRY( lineEditor ), false );
        gtk_widget_show( lineEditor );

        // combobox
        gtk_table_attach( GTK_TABLE( _mainWidget ), label = gtk_label_new( "Editable combobox: " ), 0, 1, 2, 3, GTK_SHRINK, GTK_FILL, 2, 2 );
        gtk_label_set_justify( GTK_LABEL( label ), GTK_JUSTIFY_RIGHT );
        gtk_widget_show( label );

        GtkWidget* comboBox(0L);
        gtk_table_attach_defaults( GTK_TABLE( _mainWidget ), comboBox = gtk_combo_box_entry_new_text(), 1, 2, 2, 3 );
        gtk_combo_box_append_text( GTK_COMBO_BOX( comboBox ), "First item" );
        gtk_combo_box_append_text( GTK_COMBO_BOX( comboBox ), "Second item" );
        gtk_combo_box_append_text( GTK_COMBO_BOX( comboBox ), "Third item" );
        gtk_widget_show( comboBox );



    }

    //____________________________________________________
    InputDemoWidget::~InputDemoWidget( void )
    {}

}
