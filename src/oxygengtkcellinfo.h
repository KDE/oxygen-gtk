#ifndef oxygengtkcellinfo_h
#define oxygengtkcellinfo_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
                _column( -1 )
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
                _column(-1)
            {
                GtkTreeViewColumn *column( 0L );
                gtk_tree_view_get_path_at_pos( treeView, x, y, &_path, &column, 0L, 0L );
                _column = indexOfColumn( treeView, column );
            }

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

            //! equal to operator
            bool operator != (const CellInfo& other ) const
            { return !( sameColumn( other ) && samePath( other )); }

            //! clear
            void clear( void )
            {
                if( _path ) gtk_tree_path_free( _path );
                _path = 0L;
                _column = -1;
            }

            //!@name accessors
            //@{

            //! true if valid
            bool isValid( void ) const
            { return _path && _column >= 0; }

            //! returns true if column is the last one
            bool isLastVisibleColumn( GtkTreeView* ) const;

            //! returns true if column is the first one
            bool isFirstVisibleColumn( GtkTreeView* ) const;

            //! returns true if column is the one that contains expander
            bool isExpanderColumn( GtkTreeView* treeView ) const
            { return _column >= 0 && _column == indexOfColumn( treeView, gtk_tree_view_get_expander_column( treeView ) ); }

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

            //! column index
            gint _column;

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

            static gint indexOfColumn( GtkTreeView*, GtkTreeViewColumn* );

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
                IsLast = 1<<2,
                Reversed = 1<<3
            };

            OX_DECLARE_FLAGS( CellFlags, CellFlag );

            //! constructor from CellInfo
            explicit CellInfoFlags( GtkTreeView* treeView, const CellInfo& cellInfo );

            //! flags
            CellFlags _flags;

            //! depth
            unsigned int _depth;
            int _expanderSize;
            int _levelIndent;

            //! keep track of whether this cell, or parents are last in hierarchy
            std::vector<bool> _isLast;
        };

    }

    OX_DECLARE_OPERATORS_FOR_FLAGS( Gtk::CellInfoFlags::CellFlags );

}
#endif
