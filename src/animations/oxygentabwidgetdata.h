#ifndef oxygentabwidgetdata_h
#define oxygentabwidgetdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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

#include <gtk/gtkwidget.h>
#include <vector>

namespace Oxygen
{

    //! detects and stores tab widget hovered tab
    class TabWidgetData
    {

        public:

        //! constructor
        TabWidgetData( void ):
            _motionId(-1),
            _leaveId(-1),
            _hoveredTab( -1 )
        {}

        //! destructor
        virtual ~TabWidgetData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! update tab rectangle
        /* this is used to decide when a tab is hovered or not */
        void updateTabRect( GtkWidget*, int, const GdkRectangle& );

        //! update hovered tab
        void updateHoveredTab( GtkWidget* );

        //! true if hovered
        int hoveredTab( void ) const
        { return _hoveredTab; }

        protected:

        //! set current tab
        void setHoveredTab( GtkWidget* widget, int value )
        {
            if( _hoveredTab == value ) return;
            _hoveredTab = value;
            gtk_widget_queue_draw( widget );
            return;
        }

        //!@name static callbacks
        //@{
        static gboolean motionNotifyEvent( GtkWidget*, GdkEventMotion*, gpointer);
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        //@}

        private:

        //! default tabRect size
        GdkRectangle defaultRect( void ) const
        {
            GdkRectangle out = {0, 0, -1, -1};
            return out;
        }

        //! motion signal id
        int _motionId;

        //! leave signal id
        int _leaveId;

        //! index of hovered tab, if any
        int _hoveredTab;

        //! store rectangles matching tabs
        std::vector<GdkRectangle> _tabRects;

    };

}

#endif
