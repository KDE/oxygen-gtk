/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygengtkcellinfo.h"

namespace Gtk
{

    //____________________________________________________________________________
    bool CellInfo::hasParent( GtkTreeView* treeView ) const
    {
        // check treeview and path
        if( !( treeView && _path ) ) return false;

        // get model
        GtkTreeModel* model( gtk_tree_view_get_model( treeView ) );
        if( !model ) return false;

        // get iterator
        GtkTreeIter iter;
        if( !gtk_tree_model_get_iter( model, &iter, _path ) ) return false;

        GtkTreeIter parent;
        return gtk_tree_model_iter_parent( model, &parent, &iter );

    }

    //____________________________________________________________________________
    bool CellInfo::hasChildren( GtkTreeView* treeView ) const
    {
        // check treeview and path
        if( !( treeView && _path ) ) return false;

        // get model
        GtkTreeModel* model( gtk_tree_view_get_model( treeView ) );
        if( !model ) return false;

        // get iterator
        GtkTreeIter iter;
        if( !gtk_tree_model_get_iter( model, &iter, _path ) ) return false;
        return gtk_tree_model_iter_has_child( model, &iter );

    }

    //____________________________________________________________________________
    bool CellInfo::isLast( GtkTreeView* treeView ) const
    {
       // check treeview and path
        if( !( treeView && _path ) ) return false;

        // get model
        GtkTreeModel* model( gtk_tree_view_get_model( treeView ) );
        if( !model ) return false;

        // get iterator
        GtkTreeIter iter;
        if( !gtk_tree_model_get_iter( model, &iter, _path ) ) return false;
        return !gtk_tree_model_iter_next( model, &iter );
    }

    //____________________________________________________________________________
    GdkRectangle CellInfo::backgroundRect( GtkTreeView* treeView ) const
    {
        GdkRectangle out = {0, 0, -1, -1 };
        if( treeView && isValid() )
        { gtk_tree_view_get_background_area( treeView, _path, _column, &out ); }

        return out;

    }


}
