#ifndef oxygentreeviewdata_h
#define oxygentreeviewdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
#include "../oxygengtkcellinfo.h"
#include "../oxygengtkutils.h"
#include "oxygenhoverdata.h"
#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <algorithm>

namespace Oxygen
{
    class TreeViewData: public HoverData
    {

        public:

        //! constructor
        TreeViewData( void ):
            _cursor(0L),
            _target(0L),
            _fullWidth( false ),
            _x(-1),
            _y(-1),
            _dirty( false )
        {}

        //! destructor
        virtual ~TreeViewData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! set cursor
        void setCursor( GdkCursor* cursor )
        {
            if( _cursor == cursor ) return;
            _cursor = cursor;
            updateColumnsCursor();
        }

        //! full width flag
        void setFullWidth( bool value )
        { _fullWidth = value; }

        //! true when hovered cell needs update
        bool isDirty( void ) const
        { return _dirty; }

        //! update hovered cell using stored position
        void updateHoveredCell( void );

        //! true if cell info is hovered
        bool isCellHovered( const Gtk::CellInfo& cellInfo ) const
        { return isCellHovered( cellInfo, _fullWidth ); }

        //! true if cell info is hovered
        bool isCellHovered( const Gtk::CellInfo& cellInfo, bool fullWidth ) const
        { return hovered() && (fullWidth || cellInfo.sameColumn( _cellInfo ) ) && cellInfo.samePath( _cellInfo ); }

        protected:

        //! mark as dirty
        /* returns true if dirty state changed */
        bool setDirty( bool value )
        {
            if( _dirty == value ) return false;
            _dirty = value;
            return true;
        }

        //! set mouse over state
        virtual bool setHovered( GtkWidget* widget, bool value );

        //! update columns cursor
        void updateColumnsCursor( void ) const;

        //! update hovered cell based on pointer position
        void updatePosition( GtkWidget*, int x, int y );

        //! update hovered cell based on previous pointer position
        void updatePosition( GtkWidget* widget )
        { updatePosition( widget, _x, _y ); }

        //! update pointer position
        void clearPosition( GtkWidget* = 0L );

        //! repaint selection
        void triggerRepaint( void );

        //! handles scrollbar value change
        class ScrollBarData
        {
            public:

            //! constructor
            ScrollBarData( void ):
                _widget( 0L )
            {}

            //! destructor
            virtual ~ScrollBarData( void )
            {}

            //! disconnect all signals
            void disconnect( void );

            GtkWidget* _widget;
            Signal _destroyId;
            Signal _valueChangedId;
        };

        //!@name child (scrollbars) handling
        //@{
        void registerScrollBars( GtkWidget* );
        void registerChild( GtkWidget*, ScrollBarData& );
        void unregisterChild( GtkWidget* );
        //@}

        //!@name static callbacks
        //@{
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );
        static void childValueChanged( GtkRange*, gpointer );
        static void columnsChanged( GtkTreeView*, gpointer );
        static gboolean motionNotifyEvent( GtkWidget*, GdkEventMotion*, gpointer );
        //@}

        private:

        //! cursor
        /*! associated to columns, for resize */
        GdkCursor* _cursor;

        //! target widget
        GtkWidget* _target;

        //!@name callbacks ids
        //@{
        Signal _motionId;
        Signal _columnsChangedId;
        //@}

        //! true if hover works on full width
        bool _fullWidth;

        //! keep track of the hovered path and column
        Gtk::CellInfo _cellInfo;

        /*!
        keep last position (window_bin coordinates) used to find
        hovered cell
        */
        int _x;
        int _y;

        //! true when hovered cell needs to be updated
        bool _dirty;

        //! vertical scrollbar data
        ScrollBarData _vScrollBar;

        //! horizontal scrollbar data
        ScrollBarData _hScrollBar;

    };

}

#endif
