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

#include "oxygenbuttondemowidget.h"

#include <string>

namespace Oxygen
{

    //____________________________________________________
    ButtonDemoWidget::ButtonDemoWidget( void )
    {

        // main widget
        GtkWidget* mainWidget( gtk_vbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( mainWidget ), 4 );
        setWidget( mainWidget );

        // setup
        setName( "Buttons" );
        setComments( "Shows the appearance of buttons" );
        setIconName( "go-jump-locationbar" );
        realize();

        // pushbuttons
        {
            GtkWidget* frame( gtk_frame_new( "Pushbutton" ) );
            gtk_box_pack_start( GTK_BOX( mainWidget ), frame, false, true, 0 );
            gtk_widget_show( frame );

            // inner table
            GtkWidget* table = gtk_table_new( 2, 4, false );
            gtk_container_set_border_width( GTK_CONTAINER( table ), 4 );
            gtk_table_set_row_spacings( GTK_TABLE( table ), 4 );
            gtk_container_add( GTK_CONTAINER( frame ), table );
            gtk_widget_show( table );

            // spacing
            GtkWidget* spacing( gtk_label_new( "" ) );
            gtk_table_attach( GTK_TABLE( table ), spacing, 3, 4, 0, 1, GTK_EXPAND, GTK_FILL, 2, 0  );
            gtk_widget_show( spacing );

            // generic label
            GtkWidget* label( 0L );

            {
                // text only
                gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Text only: " ), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
                gtk_widget_show( label );

                // button
                GtkWidget* button( gtk_button_new_with_label( "Normal" ) );
                gtk_table_attach( GTK_TABLE( table ), button, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_widget_show( button );
                gtk_widget_set_tooltip_text( button, "This is a normal, text only button" );

                // combobox model
                GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
                const char* columns[] =
                {
                    "Small",
                    "Normal",
                    "Large"
                };

                for( unsigned int i=0; i<3; i++ )
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

                gtk_table_attach( GTK_TABLE( table ), comboBox, 2, 3, 0, 1, GTK_FILL, GTK_FILL, 2, 0 );
                gtk_widget_show( comboBox );
                gtk_widget_set_tooltip_text( comboBox, "This is a normal, text only combo box" );

            }

            {
                // text and icons
                gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Text and icon: " ), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
                gtk_widget_show( label );

                // button
                GtkWidget* button( gtk_button_new_with_label( "Normal" ) );
                gtk_table_attach( GTK_TABLE( table ), button, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 2, 0  );

                GtkIconTheme* theme( gtk_icon_theme_get_default() );
                GdkPixbuf* icon = gtk_icon_theme_load_icon( theme, "oxygen", 16, (GtkIconLookupFlags) 0, 0L );
                if(icon)
                {
                    GtkWidget* image( gtk_image_new_from_pixbuf( icon ) );
                    g_object_unref( icon );

                    gtk_button_set_image( GTK_BUTTON( button ), image );
                }
                gtk_widget_show( button );
                gtk_widget_set_tooltip_text( button, "This is a normal, text and icon button" );

                // combobox model
                GtkListStore* model( gtk_list_store_new( 2, GDK_TYPE_PIXBUF, G_TYPE_STRING ) );
                const char* columns[] =
                {
                    "New",
                    "Open",
                    "Save"
                };

                const char* icons[] =
                {
                    "document-new",
                    "document-open",
                    "document-save"
                };

                // store into model
                for( unsigned int i=0; i<3; i++ )
                {
                    GtkTreeIter iter;
                    gtk_list_store_append( model, &iter );

                    GdkPixbuf* icon = gtk_icon_theme_load_icon( theme, icons[i], 16, (GtkIconLookupFlags) 0, 0L );
                    gtk_list_store_set( model, &iter, 0, icon, 1, columns[i], -1 );
                    g_object_unref( icon );
                }

                GtkWidget* comboBox( gtk_combo_box_new() );
                gtk_combo_box_set_model( GTK_COMBO_BOX( comboBox ), GTK_TREE_MODEL( model ) );
                g_object_unref( model );

                {
                    // pixbuf renderer
                    GtkCellRenderer* cell = gtk_cell_renderer_pixbuf_new();
                    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( comboBox ), cell, FALSE );
                    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( comboBox ), cell,
                        "pixbuf", 0,
                        NULL );
                }

                {
                    // text renderer
                    GtkCellRenderer* cell( gtk_cell_renderer_text_new() );
                    gtk_cell_layout_pack_start( GTK_CELL_LAYOUT( comboBox ), cell, FALSE );
                    gtk_cell_layout_set_attributes( GTK_CELL_LAYOUT( comboBox ), cell, "text", 1, NULL );
                }

                gtk_combo_box_set_active( GTK_COMBO_BOX( comboBox ), 0 );

                gtk_table_attach( GTK_TABLE( table ), comboBox, 2, 3, 1, 2, GTK_FILL, GTK_FILL, 2, 0 );
                gtk_widget_show( comboBox );
                gtk_widget_set_tooltip_text( comboBox, "This is a normal, text and icon combo box" );

            }


        }

        {
            // toolbar
            GtkWidget* frame( gtk_frame_new( "Toolbuttons" ) );
            gtk_box_pack_start( GTK_BOX( mainWidget ), frame, false, true, 0 );
            gtk_widget_show( frame );

            // inner box
            GtkWidget* vbox( gtk_vbox_new( false, 0 ) );
            gtk_box_set_spacing( GTK_BOX( vbox ), 4 );
            gtk_container_set_border_width( GTK_CONTAINER( vbox ), 4 );
            gtk_container_add( GTK_CONTAINER( frame ), vbox );
            gtk_widget_show( vbox );

            _toolbar = gtk_toolbar_new();
            gtk_box_pack_start( GTK_BOX( vbox ), _toolbar, false, true, 0 );
            gtk_widget_show( _toolbar );

            // toolbuttons
            GtkToolItem* toolButton;

            gtk_toolbar_insert( GTK_TOOLBAR(_toolbar ), toolButton = gtk_tool_button_new_from_stock( GTK_STOCK_NEW ), 0 );
            gtk_widget_set_tooltip_markup( GTK_WIDGET( toolButton ), "New" );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            gtk_toolbar_insert( GTK_TOOLBAR( _toolbar ), toolButton = gtk_tool_button_new_from_stock( GTK_STOCK_OPEN ), 1 );
            gtk_widget_set_tooltip_markup( GTK_WIDGET( toolButton ), "Open" );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            gtk_toolbar_insert( GTK_TOOLBAR( _toolbar ), toolButton = gtk_tool_button_new_from_stock( GTK_STOCK_SAVE ), 2 );
            gtk_widget_set_tooltip_markup( GTK_WIDGET( toolButton ), "Save" );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            gtk_toolbar_insert( GTK_TOOLBAR( _toolbar ), toolButton = gtk_tool_button_new_from_stock( GTK_STOCK_CUT ), 2 );
            gtk_widget_set_tooltip_markup( GTK_WIDGET( toolButton ), "Cut selection" );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            gtk_toolbar_insert( GTK_TOOLBAR( _toolbar ), toolButton = gtk_tool_button_new_from_stock( GTK_STOCK_COPY ), 2 );
            gtk_widget_set_tooltip_markup( GTK_WIDGET( toolButton ), "Copy selection" );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            gtk_toolbar_insert( GTK_TOOLBAR( _toolbar ), toolButton = gtk_tool_button_new_from_stock( GTK_STOCK_PASTE ), 2 );
            gtk_widget_set_tooltip_markup( GTK_WIDGET( toolButton ), "Paste clipboard" );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            gtk_toolbar_insert( GTK_TOOLBAR( _toolbar ), toolButton = gtk_toggle_tool_button_new_from_stock( GTK_STOCK_DIALOG_AUTHENTICATION ), 2 );
            gtk_toggle_tool_button_set_active( GTK_TOGGLE_TOOL_BUTTON( toolButton ), true );
            gtk_widget_show( GTK_WIDGET( toolButton ) );

            // table for text position and icon size
            GtkWidget* table = gtk_table_new( 2, 3, false );
            gtk_container_set_border_width( GTK_CONTAINER( table ), 4 );
            gtk_table_set_row_spacings( GTK_TABLE( table ), 4 );
            gtk_box_pack_start( GTK_BOX( vbox ), table, false, true, 0 );
            gtk_widget_show( table );

            {

                // text position combobox
                GtkWidget* label( 0L );

                gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Text position: " ), 0, 1, 0, 1, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
                gtk_widget_show( label );

                GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
                const char* columns[] =
                {
                    "Icons Only",
                    "Text Only",
                    "Text Alongside Icons",
                    "Text Under Icons"
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

                gtk_table_attach( GTK_TABLE( table ), comboBox, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 2, 0 );
                gtk_widget_show( comboBox );

                // connection
                _toolBarStyleChangedId.connect( G_OBJECT( comboBox ), "changed", G_CALLBACK( toolBarStyleChanged ), this );

            }

            {

                // icon size combobox
                GtkWidget* label( 0L );

                gtk_table_attach( GTK_TABLE( table ), label = gtk_label_new( "Icon size: " ), 0, 1, 1, 2, GTK_FILL, GTK_FILL, 2, 0  );
                gtk_misc_set_alignment( GTK_MISC( label ), 1, 0.5 );
                gtk_widget_show( label );

                GtkListStore* model( gtk_list_store_new( 1, G_TYPE_STRING ) );
                const char* columns[] =
                {
                    "Small",
                    "Medium",
                    "Large",
                    "Huge"
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

                gtk_combo_box_set_active( GTK_COMBO_BOX( comboBox ), 2 );

                gtk_table_attach( GTK_TABLE( table ), comboBox, 1, 2, 1, 2, GTK_FILL, GTK_FILL, 2, 0 );
                gtk_widget_show( comboBox );

                // connection
                _iconSizeChangedId.connect( G_OBJECT( comboBox ), "changed", G_CALLBACK( iconSizeChanged ), this );

            }


        }


        // checkboxes and radiobuttons
        GtkWidget* hbox( gtk_hbox_new( false, 0 ) );
        gtk_box_set_spacing( GTK_BOX( hbox ), 4 );
        gtk_box_pack_start( GTK_BOX( mainWidget ), hbox, false, true, 0 );
        gtk_widget_show( hbox );

        {
            // checkboxes
            GtkWidget* frame( gtk_frame_new( "Checkboxes" ) );
            gtk_box_pack_start( GTK_BOX( hbox ), frame, true, true, 0 );
            gtk_widget_show( frame );

            GtkWidget* vbox( gtk_vbox_new( false, 0 ) );
            gtk_container_set_border_width( GTK_CONTAINER( vbox ), 4 );
            gtk_container_add( GTK_CONTAINER( frame ), vbox );
            gtk_widget_show( vbox );

            GtkWidget* checkbutton;
            gtk_box_pack_start( GTK_BOX( vbox ), checkbutton = gtk_check_button_new_with_label( "Off" ), false, true, 0 );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( checkbutton ), false );
            gtk_widget_show( checkbutton );

            gtk_box_pack_start( GTK_BOX( vbox ), checkbutton = gtk_check_button_new_with_label( "Partial" ), false, true, 0 );
            gtk_toggle_button_set_inconsistent( GTK_TOGGLE_BUTTON( checkbutton ), true );
            gtk_widget_show( checkbutton );

            gtk_box_pack_start( GTK_BOX( vbox ), checkbutton = gtk_check_button_new_with_label( "On" ), false, true, 0 );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( checkbutton ), true );
            gtk_widget_show( checkbutton );

        }

        {
            // radio buttons
            GtkWidget* frame( gtk_frame_new( "Radiobuttons" ) );
            gtk_box_pack_start( GTK_BOX( hbox ), frame, true, true, 0 );
            gtk_widget_show( frame );

            GtkWidget* vbox( gtk_vbox_new( false, 0 ) );
            gtk_container_add( GTK_CONTAINER( frame ), vbox );
            gtk_container_set_border_width( GTK_CONTAINER( vbox ), 4 );
            gtk_widget_show( vbox );

            GtkWidget* radiobutton;
            gtk_box_pack_start( GTK_BOX( vbox ), radiobutton = gtk_radio_button_new_with_label( 0L, "First Choice" ), false, true, 0 );
            gtk_widget_show( radiobutton );

            gtk_box_pack_start( GTK_BOX( vbox ), radiobutton = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( radiobutton ), "Second Choice" ), false, true, 0 );
            gtk_widget_show( radiobutton );

            gtk_box_pack_start( GTK_BOX( vbox ), radiobutton = gtk_radio_button_new_with_label_from_widget( GTK_RADIO_BUTTON( radiobutton ), "Third Choice" ), false, true, 0 );
            gtk_widget_show( radiobutton );
        }

    }

    //____________________________________________________
    ButtonDemoWidget::~ButtonDemoWidget( void )
    {}

    //____________________________________________________
    void ButtonDemoWidget::toolBarStyleChanged( GtkComboBox* comboBox, gpointer data )
    {

        GtkToolbar* toolbar( GTK_TOOLBAR( static_cast<ButtonDemoWidget*>( data )->_toolbar ) );
        switch( gtk_combo_box_get_active( comboBox ) )
        {
            default:
            case 0: gtk_toolbar_set_style( toolbar, GTK_TOOLBAR_ICONS ); break;
            case 1: gtk_toolbar_set_style( toolbar, GTK_TOOLBAR_TEXT ); break;
            case 2: gtk_toolbar_set_style( toolbar, GTK_TOOLBAR_BOTH_HORIZ ); break;
            case 3: gtk_toolbar_set_style( toolbar, GTK_TOOLBAR_BOTH ); break;
        }

    }

    //____________________________________________________
    void ButtonDemoWidget::iconSizeChanged( GtkComboBox* comboBox, gpointer data )
    {

        GtkToolbar* toolbar( GTK_TOOLBAR( static_cast<ButtonDemoWidget*>( data )->_toolbar ) );
        switch( gtk_combo_box_get_active( comboBox ) )
        {
            default:
            case 0: gtk_toolbar_set_icon_size( toolbar, GTK_ICON_SIZE_MENU ); break;
            case 1: gtk_toolbar_set_icon_size( toolbar, GTK_ICON_SIZE_SMALL_TOOLBAR ); break;
            case 2: gtk_toolbar_set_icon_size( toolbar, GTK_ICON_SIZE_LARGE_TOOLBAR ); break;
            case 3: gtk_toolbar_set_icon_size( toolbar, GTK_ICON_SIZE_DND ); break;
        }

    }

}
