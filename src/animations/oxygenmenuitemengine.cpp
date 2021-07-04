/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenmenuitemengine.h"
#include <iostream>

namespace Oxygen
{

    //_______________________________________________________________
    bool MenuItemEngine::registerMenu( GtkWidget* parent )
    {

        // check widget
        if( !GTK_IS_MENU( parent ) ) return false;

        // keep track of added children
        bool found( false );

        // get children
        GList* children( gtk_container_get_children( GTK_CONTAINER( parent ) ) );
        for( GList *child = g_list_first( children ); child; child = g_list_next( child ) )
        {
            if( !GTK_IS_MENU_ITEM( child->data ) ) continue;
            GtkWidget* widget( gtk_bin_get_child( GTK_BIN( child->data ) ) );
            if( registerWidget( widget ) ) found = true;
        }

        // free list of children
        if( children ) g_list_free( children );

        return found;

    }

}
