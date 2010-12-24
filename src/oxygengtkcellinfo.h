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

#include <vector>
#include <iostream>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

namespace Oxygen
{
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
            CellInfo( const CellInfo& other ):
                _path( other._path ? gtk_tree_path_copy( other._path ):0L ),
                _column( other._column )
            {}

            //! construct from tree view and position
            /*! unfortunately the path retrieval does not always work because x and y must be positive */
            explicit CellInfo( GtkTreeView* treeView, int x, int y ):
                _path(0L),
                _column(0L)
            { gtk_tree_view_get_path_at_pos( treeView, x, y, &_path, &_column, 0L, 0L ); }

            //! construct from tree view and rectangle
            explicit CellInfo( GtkTreeView* treeView, int x, int y, int w, int h );

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
            { return sameColumn( other ) && samePath( other ); }

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

            //! returns true if column is the first one
            bool isFirstColumn( GtkTreeView* tree_view ) const
            { return _column && _column == gtk_tree_view_get_column( tree_view, 0 ); }

            //! returns true if column is the one that contains expander
            bool isExpanderColumn( GtkTreeView* treeView ) const
            { return _column == gtk_tree_view_get_expander_column( treeView ); }

            //! returs true if column is let of expander column
            bool isLeftOfExpanderColumn( GtkTreeView* ) const;

            //! returns true if path has parent
            bool hasParent( GtkTreeView* ) const;

            //! parent cell
            CellInfo parent( void ) const;

            //! returns true if path has children
            bool hasChildren( GtkTreeView* ) const;

            //! returns true if path is last at its given level
            bool isLast( GtkTreeView* ) const;

            //! return path depth
            unsigned int depth( void ) const
            { return _path ? (unsigned int) gtk_tree_path_get_depth( _path ):0;  }

            //! true if column match
            bool sameColumn( const CellInfo& other ) const
            { return _column == other._column; }

            //! true if path match
            bool samePath( const CellInfo& other ) const
            {
                if( _path ) return other._path && !gtk_tree_path_compare( _path, other._path );
                else return !other._path;
            }

            //! background rect
            GdkRectangle backgroundRect( GtkTreeView* ) const;

            //@}

            private:

            //! path
            GtkTreePath* _path;

            //! column
            GtkTreeViewColumn* _column;

            //! streamer
            friend std::ostream& operator << (std::ostream& out, const CellInfo& info )
            {

                if( info.isValid() )
                {
                    gchar* path( gtk_tree_path_to_string( info._path ) );
                    out << path;
                    g_free( path );
                } else out << "invalid";
                return out;
            }

        };

        //! cell info flags
        /*! stores relevant information from cell info needed for painting */
        class CellInfoFlags
        {

            public:

            //! constructor
            explicit CellInfoFlags( void ):
                _depth(0),
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
            explicit CellInfoFlags( GtkTreeView* treeView, const CellInfo& cellInfo );

            //! flags
            typedef Flags<CellFlag> CellFlags;
            CellFlags _flags;

            //! depth
            unsigned int _depth;
            int _expanderSize;
            int _levelIndent;

            //! keep track of whether this cell, or parents are last in hierarchy
            std::vector<bool> _isLast;
        };

    }
}
#endif
