/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    based on the Null Theme Engine for Gtk+.
    SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygeninputdemowidget.h"

#include <iostream>
#include <string>

namespace Oxygen
{

    //____________________________________________________
    InputDemoWidget::InputDemoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 5 );
        setWidget( mainWidget );

        // setup
        setName( "Input Widgets" );
        setComments( "Shows the appearance of text input widgets" );
        setIconName( "edit-rename" );
        realize();

        // main container
        GtkWidget* table = gtk_table_new( 4, 3, false );
        gtk_table_set_row_spacings( GTK_TABLE( table ), 5 );
        gtk_box_pack_start( GTK_BOX( mainWidget ), table, false, true, 0 );
        gtk_widget_show( table );

        // spacing
        GtkWidget* spacing( gtk_label_new( "" ) );
        gtk_table_attach( GTK_TABLE( table ), spacing, 2, 3, 0, 1, GTK_EXPAND, GTK_FILL, 2, 0  );
        gtk_widget_show( spacing );

        // generic label
        GtkWidget* label( 0L );

        // simple line editor
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Single line text editor: " ), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 2, 0  );
        gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
        gtk_widget_show( label );

        GtkWidget* lineEditor( 0L );
        gtk_table_attach( GTK_TABLE( table ), lineEditor = gtk_entry_new(), 1, 2, 0, 1, GTK_FILL, GTK_FILL, 2, 0 );
        gtk_entry_set_text( GTK_ENTRY( lineEditor ), "Example text" );
        gtk_widget_show( lineEditor );

        _entries.push_back( lineEditor );

        // invisible line editor
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Password editor: " ), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 2, 0 );
        gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
        gtk_widget_show( label );

        gtk_table_attach( GTK_TABLE( table ), lineEditor = gtk_entry_new(), 1, 2, 1, 2, GTK_FILL, GTK_FILL, 2, 0 );
        gtk_entry_set_text( GTK_ENTRY( lineEditor ), "Example text" );
        gtk_entry_set_visibility( GTK_ENTRY( lineEditor ), false );
        gtk_widget_show( lineEditor );

        _entries.push_back( lineEditor );

        // combobox
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Editable combobox: " ), 0, 1, 2, 3, GTK_FILL, GTK_FILL, 2, 0 );
        gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
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

        GtkWidget* comboBox(0L);
        #if GTK_CHECK_VERSION(2, 24, 0)
        gtk_table_attach( GTK_TABLE( table ), comboBox = gtk_combo_box_new_with_entry(), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 2, 0 );
        gtk_combo_box_set_model( GTK_COMBO_BOX( comboBox ), GTK_TREE_MODEL( model ) );
        gtk_combo_box_set_entry_text_column( GTK_COMBO_BOX( comboBox ), 0 );

        #else
        gtk_table_attach( GTK_TABLE( table ), comboBox = gtk_combo_box_entry_new(), 1, 2, 2, 3, GTK_FILL, GTK_FILL, 2, 0 );
        gtk_combo_box_set_model( GTK_COMBO_BOX( comboBox ), GTK_TREE_MODEL( model ) );
        gtk_combo_box_entry_set_text_column( GTK_COMBO_BOX_ENTRY( comboBox ), 0 );
        #endif
        g_object_unref( model );

        gtk_combo_box_set_active( GTK_COMBO_BOX( comboBox ), 0 );
        gtk_widget_show( comboBox );

        // spin button
        gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Spinbox: " ), 0, 1, 3, 4, GTK_FILL, GTK_FILL, 2, 0  );
        gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
        gtk_widget_show( label );

        GtkWidget* spinButton( 0L );
        gtk_table_attach( GTK_TABLE( table ), spinButton = gtk_spin_button_new_with_range( 0, 100, 1 ), 1, 2, 3, 4, GTK_FILL, GTK_FILL, 2, 0  );
        gtk_widget_show( spinButton );

        _entries.push_back( spinButton );

        // flat widget button
        GtkWidget* checkbutton;
        gtk_table_attach( GTK_TABLE( table ), checkbutton = gtk_check_button_new_with_label( "Use flat widgets" ), 0, 2, 4, 5, GTK_FILL, GTK_FILL, 2, 0  );
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( checkbutton ), false );
        gtk_widget_show( checkbutton );

        connect( G_OBJECT( checkbutton ), "toggled", G_CALLBACK( flatModeChanged ), this );


        // separator
        GtkWidget* separator( gtk_hseparator_new() );
        gtk_box_pack_start( GTK_BOX( mainWidget ), separator, false, true, 0 );
        gtk_widget_show( separator );

        // text view
        gtk_box_pack_start( GTK_BOX( mainWidget ), label = gtk_label_new( "Multi-line text editor: " ), false, true, 0 );
        gtk_misc_set_alignment( GTK_MISC( label ), 0, 0.5 );
        gtk_widget_show( label );

        const std::string content( "Lorem ipsum dolor sit amet, consectetur "
            "adipisicing elit, sed do eiusmod tempor incididunt ut labore "
            "et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud "
            "exercitation ullamco laboris nisi ut aliquip ex ea commodo "
            "consequat. Duis aute irure dolor in reprehenderit in voluptate "
            "velit esse cillum dolore eu fugiat nulla pariatur. Excepteur "
            "sint occaecat cupidatat non proident, sunt in culpa qui officia "
            "deserunt mollit anim id est laborum.\n"
            "\n"
            "Sed ut perspiciatis unde omnis iste natus error sit voluptatem "
            "accusantium doloremque laudantium, totam rem aperiam, eaque "
            "ipsa quae ab illo inventore veritatis et quasi architecto beatae "
            "vitae dicta sunt explicabo. Nemo enim ipsam voluptatem quia "
            "voluptas sit aspernatur aut odit aut fugit, sed quia consequuntur "
            "magni dolores eos qui ratione voluptatem sequi nesciunt. "
            "Neque porro quisquam est, qui dolorem ipsum quia dolor sit amet, "
            "consectetur, adipisci velit, sed quia non numquam eius modi "
            "tempora incidunt ut labore et dolore magnam aliquam quaerat "
            "voluptatem. Ut enim ad minima veniam, quis nostrum exercitationem "
            "ullam corporis suscipit laboriosam, nisi ut aliquid ex ea commodi "
            "consequatur? Quis autem vel eum iure reprehenderit qui in ea "
            "voluptate velit esse quam nihil molestiae consequatur, vel illum "
            "qui dolorem eum fugiat quo voluptas nulla pariatur?" );

        GtkTextTagTable* tags( gtk_text_tag_table_new() );
        GtkTextBuffer* buffer( gtk_text_buffer_new( tags ) );
        gtk_text_buffer_set_text( buffer, content.c_str(), content.size() );
        _textView = gtk_text_view_new_with_buffer( buffer );
        gtk_text_view_set_wrap_mode( GTK_TEXT_VIEW( _textView ), GTK_WRAP_WORD );
        gtk_container_set_border_width( GTK_CONTAINER( _textView ), 2 );
        gtk_widget_show( _textView );

        GtkWidget* scrolledWindow( gtk_scrolled_window_new( 0L, 0L ) );
        gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_SHADOW_IN );
        gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC );
        gtk_container_add( GTK_CONTAINER( scrolledWindow ), _textView );
        gtk_box_pack_start( GTK_BOX( mainWidget ), scrolledWindow, true, true, 0 );
        gtk_widget_show( scrolledWindow );

        gtk_box_pack_start( GTK_BOX( mainWidget ), checkbutton = gtk_check_button_new_with_label( "Wrap words" ), false, true, 0 );
        gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( checkbutton ), true );
        gtk_widget_show( checkbutton );

        connect( G_OBJECT( checkbutton ), "toggled", G_CALLBACK( wrapModeChanged ), this );

    }

    //____________________________________________________
    InputDemoWidget::~InputDemoWidget( void )
    {}

    //____________________________________________________
    void InputDemoWidget::flatModeChanged( GtkToggleButton* button, gpointer data )
    {
        WidgetList entries( static_cast<InputDemoWidget*>( data )->_entries );
        bool state( gtk_toggle_button_get_active( button ) );
        for( WidgetList::const_iterator iter = entries.begin(); iter != entries.end(); iter++ )
        { gtk_entry_set_has_frame( GTK_ENTRY( *iter ), !state ); }
    }

    //____________________________________________________
    void InputDemoWidget::wrapModeChanged( GtkToggleButton* button, gpointer data )
    {
        GtkTextView* textView( GTK_TEXT_VIEW( static_cast<InputDemoWidget*>( data )->_textView ) );
        bool state( gtk_toggle_button_get_active( button ) );
        gtk_text_view_set_wrap_mode( textView, (state ? GTK_WRAP_WORD:GTK_WRAP_NONE) );
    }

}
