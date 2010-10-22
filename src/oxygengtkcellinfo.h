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

#include "oxygenflags.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

namespace Gtk
{

    //! stores information for a given treeview cell
    /*!
    note: treeview itself is not stored as a member of the object, to avoid carying dangling pointers
    around. As a consequence it must be passed as argument of all methods that would need it. This
    implicitely requires that the same treeview is used in constructor and accessors, although it would
    not crash if not the case
    */
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

        //!@name accessors
        //@{

        //! true if valid
        bool isValid( void ) const
        { return _path && _column; }

        //! returns true if column is the one that contains expander
        bool isExpanderColumn( GtkTreeView* tree_view ) const
        { return _column == gtk_tree_view_get_expander_column( tree_view ); }

        //! returns true if path has parent
        bool hasParent( GtkTreeView* ) const;

        //! returns true if path has children
        bool hasChildren( GtkTreeView* ) const;

        //! returns true if path is last at its given level
        bool isLast( GtkTreeView* ) const;

        //! return path depth
        int depth( void ) const
        { return _path ? gtk_tree_path_get_depth( _path ):-1;  }

        //! background rect
        GdkRectangle backgroundRect( GtkTreeView* ) const;

        //@}

        private:

        //! path
        GtkTreePath* _path;

        //! column
        GtkTreeViewColumn* _column;

    };

    //! cell info flags
    /*! stores relevent information from cell info needed for painting */
    class CellInfoFlags
    {

        public:

        //! constructor
        explicit CellInfoFlags( void ):
            _depth( -1 ),
            _expanderSize(0),
            _levelIndent(0)
            {}

        //! flags
        enum CellFlag
        {
            HasParent = 1<<0,
            HasChildren = 1<<1,
            IsLast = 1<<2
        };

        //! constructor from CellInfo
        explicit CellInfoFlags( GtkTreeView* treeView, const CellInfo& cellInfo ):
            _depth( cellInfo.depth() ),
            _expanderSize(0),
            _levelIndent(gtk_tree_view_get_level_indentation(treeView))
        {
            if( cellInfo.hasParent( treeView ) ) _flags |= HasParent;
            if( cellInfo.hasChildren( treeView ) ) _flags |= HasChildren;
            if( cellInfo.isLast( treeView ) ) _flags |= IsLast;

            gtk_widget_style_get( GTK_WIDGET( treeView ), "expander-size", &_expanderSize, NULL );

        }

        //! flags
        typedef Oxygen::Flags<CellFlag> CellFlags;
        CellFlags _flags;

        //! depth
        int _depth;
        int _expanderSize;
        int _levelIndent;

    };

}

#endif
