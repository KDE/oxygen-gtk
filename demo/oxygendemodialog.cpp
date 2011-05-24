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

#include "oxygendemodialog.h"
#include "oxygeninputdemowidget.h"
#include "oxygenbuttondemowidget.h"
#include "oxygenframedemowidget.h"
#include "oxygenlistdemowidget.h"
#include "oxygensliderdemowidget.h"
#include "oxygentabdemowidget.h"

#include <iostream>
#include <sstream>

namespace Oxygen
{

    //_____________________________________________
    DemoDialog::DemoDialog( void )
    {

        // create main widget
        _mainWidget = gtk_window_new( GTK_WINDOW_TOPLEVEL );
        gtk_window_set_default_size( GTK_WINDOW( _mainWidget ), 630, 500 );
        gtk_window_set_title( GTK_WINDOW( _mainWidget ), "Oxygen-gtk Demo" );
        gtk_container_set_border_width( GTK_CONTAINER( _mainWidget ), 10 );

        // vertical container
        GtkWidget* vbox( gtk_vbox_new( false, 5 ) );
        gtk_container_add( GTK_CONTAINER( _mainWidget ), vbox );
        gtk_widget_show( vbox );

        GtkWidget* iconView(0L);
        {
            // first horizontal container
            GtkWidget* hbox( gtk_hbox_new( false, 8 ) );
            gtk_box_pack_start( GTK_BOX( vbox ), hbox, true, true, 0 );
            gtk_widget_show( hbox );

            // iconview model
            _model = gtk_list_store_new( 2, GDK_TYPE_PIXBUF, G_TYPE_STRING );

            // inconview
            GtkWidget* scrolledWindow( gtk_scrolled_window_new( 0L, 0L ) );
            gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_SHADOW_IN );
            gtk_scrolled_window_set_policy( GTK_SCROLLED_WINDOW( scrolledWindow ), GTK_POLICY_NEVER, GTK_POLICY_NEVER );
            gtk_box_pack_start( GTK_BOX( hbox ), scrolledWindow, false, true, 0 );
            gtk_widget_show( scrolledWindow );

            iconView = gtk_icon_view_new_with_model( GTK_TREE_MODEL( _model ) );
            gtk_icon_view_set_pixbuf_column( GTK_ICON_VIEW( iconView ), 0 );
            gtk_icon_view_set_text_column( GTK_ICON_VIEW( iconView ), 1 );
            gtk_icon_view_set_columns( GTK_ICON_VIEW( iconView ), 1 );

            gtk_icon_view_set_item_width( GTK_ICON_VIEW( iconView ), 108 );
            gtk_icon_view_set_spacing( GTK_ICON_VIEW( iconView ), 0 );
            gtk_icon_view_set_margin( GTK_ICON_VIEW( iconView ), 0 );
            gtk_icon_view_set_column_spacing( GTK_ICON_VIEW( iconView ), 0 );
            gtk_icon_view_set_row_spacing( GTK_ICON_VIEW( iconView ), 0 );
            gtk_icon_view_set_selection_mode( GTK_ICON_VIEW( iconView ), GTK_SELECTION_BROWSE );

            // get list of renderers, find text renderer and make font format bold
            GList* cells( gtk_cell_layout_get_cells( GTK_CELL_LAYOUT( iconView ) ) );
            for( GList *cell = g_list_first( cells ); cell; cell = g_list_next( cell ) )
            {
                if( !GTK_IS_CELL_RENDERER_TEXT( cell->data ) ) continue;

                // create pango attributes list
                PangoAttrList* attributes( pango_attr_list_new() );
                pango_attr_list_insert( attributes, pango_attr_weight_new( PANGO_WEIGHT_BOLD ) );

                GValue val = { 0, };
                g_value_init(&val, PANGO_TYPE_ATTR_LIST );
                g_value_set_boxed( &val, attributes );
                g_object_set_property( G_OBJECT( cell->data ), "attributes", &val );

                pango_attr_list_unref( attributes );

            }

            if( cells ) g_list_free( cells );

            // connect signals
            _selectionChangedId.connect( G_OBJECT(iconView), "selection-changed", G_CALLBACK( selectionChanged ), this );

            gtk_container_add( GTK_CONTAINER( scrolledWindow ), iconView );
            gtk_widget_show( iconView );

            // notebook
            _notebook = gtk_notebook_new();
            gtk_notebook_set_show_tabs( GTK_NOTEBOOK( _notebook ), false );
            gtk_box_pack_start( GTK_BOX( hbox ), _notebook, true, true, 0 );
            gtk_widget_show( _notebook );

        }

        {
            // statusbar
            GtkWidget* statusBar( gtk_hbox_new( false, 2 ) );
            gtk_box_pack_start( GTK_BOX( vbox ), statusBar, false, true, 0 );
            gtk_widget_show( statusBar );

            // enable checkbox
            _stateButton = gtk_check_button_new_with_label( "Enabled" );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( _stateButton ), true );
            gtk_box_pack_start( GTK_BOX( statusBar ), _stateButton, false, false, 0 );
            gtk_widget_show( _stateButton );

            _toggleEnableStateId.connect( G_OBJECT(_stateButton), "toggled", G_CALLBACK( toggleEnableState ), this );

            // widget direction checkbox
            GtkWidget* button( gtk_check_button_new_with_label( "Right to left layout" ) );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( button ), false );
            gtk_box_pack_start( GTK_BOX( statusBar ), button, false, false, 0 );
            gtk_widget_show( button );

            _toggleWidgetDirectionId.connect( G_OBJECT(button), "toggled", G_CALLBACK( toggleWidgetDirection ), 0L );

            // button box
            #if GTK_CHECK_VERSION( 3, 0, 0 )
            GtkWidget* buttonBox( gtk_button_box_new( GTK_ORIENTATION_HORIZONTAL) );
            #else
            GtkWidget* buttonBox( gtk_hbutton_box_new() );
            #endif

            gtk_button_box_set_layout( GTK_BUTTON_BOX( buttonBox ), GTK_BUTTONBOX_END );
            gtk_box_pack_end( GTK_BOX( statusBar ), buttonBox, true, true, 0 );
            gtk_widget_show( buttonBox );

            // close button
            button = gtk_button_new_from_stock( GTK_STOCK_OK );
            gtk_box_pack_end( GTK_BOX( buttonBox ), button, false, true, 0 );
            gtk_widget_show( button );

            g_signal_connect( G_OBJECT(button), "clicked", G_CALLBACK( gtk_main_quit ), 0L );

        }

        addPage( new InputDemoWidget() );
        addPage( new TabDemoWidget() );
        addPage( new ButtonDemoWidget() );
        addPage( new ListDemoWidget() );
        addPage( new FrameDemoWidget() );
        addPage( _sliderDemoWidget = new SliderDemoWidget() );

        // select first raw
        GtkTreePath *path( gtk_tree_path_new_from_indices(0, -1 ) );
        gtk_icon_view_select_path( GTK_ICON_VIEW( iconView ), path );

    }

    //_____________________________________________
    DemoDialog::~DemoDialog( void )
    {}

    //_____________________________________________
    void DemoDialog::addPage( DemoWidget* page )
    {
        // get icon
        GdkPixbuf* icon( 0L );
        if( !page->iconName().empty() )
        {

            // TODO: should get this icon size from options
            GtkIconTheme* theme( gtk_icon_theme_get_default() );
            icon = gtk_icon_theme_load_icon( theme, page->iconName().c_str(), 32, (GtkIconLookupFlags) 0, 0L );

        }

        // insert in list
        GtkTreeIter iter;
        gtk_list_store_append( _model, &iter );
        gtk_list_store_set( _model, &iter, 0, icon, 1, page->name().c_str(), -1 );

        // add to notebook
        int index( gtk_notebook_append_page( GTK_NOTEBOOK( _notebook ), page->mainWidget(), 0L ) );
        gtk_widget_show( page->mainWidget() );

        _pages.insert( std::make_pair( index, page ) );
    }

    //_____________________________________________
    void DemoDialog::selectionChanged( GtkIconView* view, gpointer data )
    {
        // get pointer to dialog
        DemoDialog& dialog( *static_cast<DemoDialog*>( data ) );

        // get selection
        GList* selection = gtk_icon_view_get_selected_items( view );
        if( !selection ) return;

        // get first child
        GtkTreePath* path( static_cast<GtkTreePath*>( g_list_first( selection )->data ) );
        const int page( gtk_tree_path_get_indices( path )[0] );
        gtk_notebook_set_current_page( GTK_NOTEBOOK( dialog._notebook ), page );
        g_list_free( selection );

        // store enable state
        const bool enabled( gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( dialog._stateButton ) ) );

        // get matching page
        DemoWidget* widget( dialog._pages[page] );
        widget->setEnabled( enabled );
        if( widget == dialog._sliderDemoWidget ) dialog._sliderDemoWidget->startPulse();
        else dialog._sliderDemoWidget->stopPulse();

        // update window title
        std::ostringstream what;
        what << widget->name() << " - Oxygen-gtk Demo";
        gtk_window_set_title( GTK_WINDOW( dialog._mainWidget ), what.str().c_str() );
    }

    //_____________________________________________
    void DemoDialog::toggleEnableState( GtkToggleButton* button, gpointer data )
    {

        // get pointer to dialog
        DemoDialog& dialog( *static_cast<DemoDialog*>( data ) );

        // store enable state
        const bool enabled( gtk_toggle_button_get_active( button ) );

        // get current page
        const int page( gtk_notebook_get_current_page( GTK_NOTEBOOK( dialog._notebook ) ) );

        // get matching page
        DemoWidget* widget( dialog._pages[page] );

        // set state
        widget->setEnabled( enabled );

        // trigger repaint
        gtk_widget_queue_draw( widget->mainWidget() );

    }

    //_____________________________________________
    void DemoDialog::toggleWidgetDirection( GtkToggleButton* button, gpointer data )
    { gtk_widget_set_default_direction( gtk_toggle_button_get_active( button ) ? GTK_TEXT_DIR_RTL:GTK_TEXT_DIR_LTR ); }
}
