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

namespace Oxygen
{

    //_____________________________________________
    DemoDialog::DemoDialog( void )
    {

        // create main widget
        _mainWidget = gtk_window_new( GTK_WINDOW_TOPLEVEL );
        gtk_window_set_title( GTK_WINDOW( _mainWidget ), "Oxygen-gtk Demo" );

        // vertical container
        GtkWidget* vbox( gtk_vbox_new( false, 5 ) );
        gtk_container_add( GTK_CONTAINER( _mainWidget ), vbox );
        gtk_widget_show( vbox );

        {
            // first horizontal container
            GtkWidget* hbox( gtk_hbox_new( false, 5 ) );
            gtk_box_pack_start( GTK_BOX( vbox ), hbox, true, true, 0 );
            gtk_widget_show( hbox );

            // inconview
            GtkWidget* frame( gtk_frame_new(0L) );
            gtk_frame_set_shadow_type( GTK_FRAME( frame ), GTK_SHADOW_IN );
            gtk_widget_set_size_request( frame, 100, 400 );
            gtk_box_pack_start( GTK_BOX( hbox ), frame, false, true, 0 );
            gtk_widget_show( frame );

            GtkWidget* iconView( gtk_icon_view_new() );
            gtk_container_add( GTK_CONTAINER( frame ), iconView );
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
            GtkWidget* button( gtk_check_button_new_with_label( "Enabled" ) );
            gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( button ), true );
            gtk_box_pack_start( GTK_BOX( statusBar ), button, false, false, 0 );
            gtk_widget_show( button );

            // close button
            button = gtk_button_new_from_stock( GTK_STOCK_OK );
            gtk_box_pack_end( GTK_BOX( statusBar ), button, false, true, 0 );
            gtk_widget_show( button );

        }

        addPage( new InputDemoWidget() );

    }

    //_____________________________________________
    void DemoDialog::addPage( DemoWidget* page )
    {
        int index( gtk_notebook_append_page( GTK_NOTEBOOK( _notebook ), page->mainWidget(), 0L ) );
        gtk_widget_show( page->mainWidget() );
        _pages.insert( std::make_pair( index, page ) );
    }

    //_____________________________________________
    DemoDialog::~DemoDialog( void )
    {}

}
