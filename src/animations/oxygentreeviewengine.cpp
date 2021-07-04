/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

            // load cursor if needed
            if( !_cursorLoaded )
            {
                assert( !_cursor );
                GdkDisplay *display( gtk_widget_get_display( widget ) );
                _cursor = gdk_cursor_new_from_name( display, "row-resize" );
                _cursorLoaded = true;
            }

            // pass cursor to data
            data().value( widget ).setCursor( _cursor );

        }

        return true;

    }


}
