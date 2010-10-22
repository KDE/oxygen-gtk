#ifndef oxygengtkcellinfo_h
#define oxygengtkcellinfo_h
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

#include <gdk/gdk.h>
#include <gtk/gtk.h>

namespace Gtk
{

    //! stores information for a given treeview cell
    class CellInfo
    {

        public:

        //! empty constructor
        explicit CellInfo( void ):
            _path( 0L ),
            _column( 0L )
        {}

        //! copy constructor
        explicit CellInfo( const CellInfo& other ):
            _path( other._path ? gtk_tree_path_copy( other._path ):0L ),
            _column( other._column )
        {}

        //! construct from tree view and position
        explicit CellInfo( GtkTreeView* treeView, int x, int y ):
            _path(0L),
            _column(0L)
        { gtk_tree_view_get_path_at_pos( treeView, x,y, &_path, &_column, 0L, 0L ); }

        //! destructor
        virtual ~CellInfo( void )
        { if( _path ) gtk_tree_path_free( _path ); }

        //! assignment operator
        CellInfo& operator = (const CellInfo& other )
        {
            if( _path ) gtk_tree_path_free( _path );
            _path = other._path ? gtk_tree_path_copy( other._path ):0L;
            _column = other._column;
            return *this;
        }

        //! equal to operator
        bool operator == (const CellInfo& other ) const
        {
            if( _column != other._column ) return false;
            else if( _path ) return other._path && !gtk_tree_path_compare( _path, other._path );
            else return !other._path;
        }

        //! clear
        void clear( void )
        {
            if( _path ) gtk_tree_path_free( _path );
            _path = 0L;
            _column = 0L;
        }

        //! true if valid
        bool isValid( void ) const
        { return _path && _column; }

        //! returns true if column is the one that contains expander
        bool isExpanderColumn( GtkTreeView* tree_view ) const
        { return _column == gtk_tree_view_get_expander_column( tree_view ); }

        //! return path depth
        int depth( void ) const
        { return _path ? gtk_tree_path_get_depth( _path ):-1;  }

        //! background rect
        GdkRectangle backgroundRect( GtkTreeView* treeView ) const
        {
            GdkRectangle out = {0, 0, -1, -1 };
            if( treeView && isValid() )
            { gtk_tree_view_get_background_area( treeView, _path, _column, &out ); }

            return out;

        }

        private:

        //! path
        GtkTreePath* _path;

        //! column
        GtkTreeViewColumn* _column;

    };

}

#endif
