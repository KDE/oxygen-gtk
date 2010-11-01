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

#include <gtk/gtk.h>
#include <algorithm>

namespace Oxygen
{
    class TreeViewData: public HoverData
    {

        public:

        //! constructor
        TreeViewData( void ):
            _target(0L),
            _motionId(-1),
            _fullWidth( false )
        {}

        //! destructor
        virtual ~TreeViewData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! full width flag
        void setFullWidth( bool value )
        { _fullWidth = value; }

        //! true if cell info is hovered
        bool isCellHovered( const Gtk::CellInfo& cellInfo ) const
        { return isCellHovered( cellInfo, _fullWidth ); }

        //! true if cell info is hovered
        bool isCellHovered( const Gtk::CellInfo& cellInfo, bool fullWidth ) const
        { return ( fullWidth || cellInfo.sameColumn( _cellInfo ) ) && cellInfo.samePath( _cellInfo ); }

        protected:

        //! set mouse over state
        virtual bool setHovered( GtkWidget* widget, bool value )
        {
            if( !HoverData::setHovered( widget, value ) ) return false;
            if( !value ) clearPosition();
            return true;
        }

        //! update pointer position
        void updatePosition( GtkWidget*, int x, int y );

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
                _widget( 0L ),
                _destroyId( -1 ),
                _styleChangeId( -1 ),
                _valueChangedId( -1 )
            {}

            //! destructor
            virtual ~ScrollBarData( void )
            {}

            //! disconnect all signals
            void disconnect( void );

            GtkWidget* _widget;
            int _destroyId;
            int _styleChangeId;
            int _valueChangedId;
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
        static void childStyleChangeNotifyEvent( GtkWidget*, GtkStyle*, gpointer );
        static void childValueChanged( GtkRange*, gpointer );
        static gboolean motionNotifyEvent( GtkWidget*, GdkEventMotion*, gpointer);
        //@}

        private:

        GtkWidget* _target;

        //! callbacks ids
        int _motionId;

        //! true if hover works on full width
        bool _fullWidth;

        //!@name keep track of the hovered path and column
        Gtk::CellInfo _cellInfo;

        //! vertical scrollbar data
        ScrollBarData _vScrollBar;

        //! horizontal scrollbar data
        ScrollBarData _hScrollBar;

    };

}

#endif
