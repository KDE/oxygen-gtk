/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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
