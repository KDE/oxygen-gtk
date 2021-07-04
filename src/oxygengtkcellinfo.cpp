/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygengtkcellinfo.h"
#include "oxygengeometry.h"
#include "oxygengtkutils.h"

#include <iostream>
#include <cassert>

namespace Oxygen
{

    //____________________________________________________________________________
    Gtk::CellInfo::CellInfo( GtkTreeView* treeView, int x, int y, int w, int h ):
        _path(0L),
        _column(-1)
    {
        GtkTreeViewColumn *column( 0L );

        /*
        four attempts are made to get the path from any corner of the rectangle passed in arguments.
        This is necessary to handle half-hidden cells
        */
        gtk_tree_view_get_path_at_pos( treeView, (gint)x+1, (gint)y+1, &_path, &column, 0L, 0L );
        if( !_path ) {
            gtk_tree_view_get_path_at_pos( treeView, (gint)x+1, (gint)y+h-1, &_path, &column, 0L, 0L );
            if( !_path ) {
                gtk_tree_view_get_path_at_pos( treeView, (gint)x+w-1, (gint)y+1, &_path, &column, 0L, 0L );
                if( !_path ) {
                    gtk_tree_view_get_path_at_pos( treeView, (gint)x+w-1, (gint)y+h-1, &_path, &column, 0L, 0L );
                    if( !_path ) return;
                }
            }
        }

        _column = indexOfColumn( treeView, column );
    }

    //____________________________________________________________________________
    bool Gtk::CellInfo::isLastVisibleColumn( GtkTreeView* treeView ) const
    {
        bool isLast( true );
        GList* columns( gtk_tree_view_get_columns( treeView ) );
        for( GList *child = g_list_nth( columns, _column ); ( child = g_list_next( child ) ); )
        {
            if( !GTK_IS_TREE_VIEW_COLUMN( child->data ) ) continue;
            GtkTreeViewColumn* column( GTK_TREE_VIEW_COLUMN( child->data ) );
            if( gtk_tree_view_column_get_visible( column ) )
            {
                isLast = false;
                break;
            }

        }

        if( columns ) g_list_free( columns );
        return isLast;
    }

    //____________________________________________________________________________
    bool Gtk::CellInfo::isFirstVisibleColumn( GtkTreeView* treeView ) const
    {
        bool isFirst( true );
        GList* columns( gtk_tree_view_get_columns( treeView ) );
        for( GList *child = g_list_nth( columns, _column ); ( child = g_list_previous( child ) ); )
        {
            if( !GTK_IS_TREE_VIEW_COLUMN( child->data ) ) continue;
            GtkTreeViewColumn* column( GTK_TREE_VIEW_COLUMN( child->data ) );
            if( gtk_tree_view_column_get_visible( column ) )
            {
                isFirst = false;
                break;
            }

        }

        if( columns ) g_list_free( columns );
        return isFirst;
    }

    //____________________________________________________________________________
    bool Gtk::CellInfo::isLeftOfExpanderColumn( GtkTreeView* treeView ) const
    {
        // check expander column
        GtkTreeViewColumn* expanderColumn( gtk_tree_view_get_expander_column( treeView ) );
        return expanderColumn && _column < indexOfColumn( treeView, expanderColumn );

    }

    //____________________________________________________________________________
    bool Gtk::CellInfo::hasParent( GtkTreeView* treeView ) const
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
    Gtk::CellInfo Gtk::CellInfo::parent( void ) const
    {
        CellInfo out;
        out._column = _column;

        // check path
        if( !_path ) return out;

        GtkTreePath* parent( gtk_tree_path_copy( _path ) );
        if( gtk_tree_path_up( parent ) ) out._path = parent;
        else gtk_tree_path_free( parent );

        return out;

    }

    //____________________________________________________________________________
    bool Gtk::CellInfo::hasChildren( GtkTreeView* treeView ) const
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
    bool Gtk::CellInfo::isLast( GtkTreeView* treeView ) const
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
    GdkRectangle Gtk::CellInfo::backgroundRect( GtkTreeView* treeView ) const
    {
        GdkRectangle out( Gtk::gdk_rectangle() );
        if( treeView && isValid() )
        { gtk_tree_view_get_background_area( treeView, _path, gtk_tree_view_get_column( treeView, _column ), &out ); }

        return out;

    }

    //____________________________________________________________________________
    gint Gtk::CellInfo::indexOfColumn( GtkTreeView* treeView, GtkTreeViewColumn* column )
    {
        GList* columns( gtk_tree_view_get_columns( treeView ) );
        if( !columns ) return -1;
        gint index( g_list_index( columns, column ) );
        g_list_free( columns );
        return index;
    }

    //____________________________________________________________________________
    Gtk::CellInfoFlags::CellInfoFlags( GtkTreeView* treeView, const CellInfo& cellInfo ):
        _depth( cellInfo.depth() ),
        _expanderSize(0),
        _levelIndent(gtk_tree_view_get_level_indentation(treeView))
    {
        if( cellInfo.hasParent( treeView ) ) _flags |= HasParent;
        if( cellInfo.hasChildren( treeView ) ) _flags |= HasChildren;
        if( cellInfo.isLast( treeView ) ) _flags |= IsLast;

        gtk_widget_style_get( GTK_WIDGET( treeView ), "expander-size", &_expanderSize, NULL );

        /*
        for every parent of the current cell, one needs to know whether or not
        it is the last one at its level, to render the tree lines properly
        */
        _isLast = std::vector<bool>(_depth, false);

        int index( _depth-1 );
        for( CellInfo parent = cellInfo; parent.isValid() && parent.depth() > 0; parent = parent.parent() )
        {
            assert( index >= 0 );
            _isLast[index] = parent.isLast( treeView );
            --index;
        }

    }

}
