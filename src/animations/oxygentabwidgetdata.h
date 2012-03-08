#ifndef oxygentabwidgetdata_h
#define oxygentabwidgetdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <vector>
#include <map>

namespace Oxygen
{

    //! detects and stores tab widget hovered tab
    class TabWidgetData
    {

        public:

        //! constructor
        TabWidgetData( void ):
            _target(0L),
            _hoveredTab(-1),
            _dragInProgress( false ),
            _dirty( false )
        {}

        //! destructor
        virtual ~TabWidgetData( void )
        { disconnect( _target ); }

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //!@name modifiers
        //@{

        //! update tab rectangle
        /* this is used to decide when a tab is hovered or not */
        void updateTabRect( GtkWidget*, int, const GdkRectangle& );

        //! update hovered tab
        void updateHoveredTab( GtkWidget* = 0L );

        //! true when drag is in progress
        void setDragInProgress( bool value )
        { _dragInProgress = value; }

        //! toggle dirty state
        void toggleDirty( void )
        { setDirty( !isDirty() ); }

        //! mark as dirty
        void setDirty( bool );

        //@}

        //@name accessors
        //@{

        //! true if hovered
        int hoveredTab( void ) const
        { return _hoveredTab; }

        //! true when drag is in progress
        bool dragInProgress( void ) const
        { return _dragInProgress; }

        //! true if is dirty
        bool isDirty( void ) const
        { return _dirty; }

        //! returns true if provided point is in one tab of the widget
        bool isInTab( int x, int y ) const;

        //@]

        protected:

        //! set current tab
        void setHoveredTab( GtkWidget*, int );

        //! child registration
        //@{

        void updateRegisteredChildren( GtkWidget* = 0L );
        void registerChild( GtkWidget* );
        void unregisterChild( GtkWidget* );

        //@}

        //!@name static callbacks
        //@{
        static gboolean motionNotifyEvent( GtkWidget*, GdkEventMotion*, gpointer );
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static void pageAddedEvent( GtkNotebook*, GtkWidget*, guint, gpointer );
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );
        static gboolean childCrossingNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static void childAddedEvent( GtkContainer*, GtkWidget*, gpointer );
        //@}

        private:

        //! default tabRect size
        GdkRectangle defaultRect( void ) const
        {
            GdkRectangle out = {0, 0, -1, -1};
            return out;
        }

        //! target widget
        GtkWidget* _target;

        //!@name callbacks IDs
        //@{
        Signal _motionId;
        Signal _leaveId;
        Signal _pageAddedId;
        //@}

        //! index of currently hovered tab
        int _hoveredTab;

        //! true when there is a drag in progress
        bool _dragInProgress;

        //! true when tabbar is dirty
        /*! a repaint is triggered of the full tabbar when set to true.
        This forces the tabbar base to be redrawn event if the selected tab
        has not been primarily damaged */
        bool _dirty;

        //! store rectangles matching tabs
        typedef std::vector<GdkRectangle> RectangleList;
        RectangleList _tabRects;

        //! child data
        /*!
        one must keep track of the tab widgets children enter/leave event
        to properly update tab hover because some tabs have embedded children.
        This is notably the case for gimp, nautilus (in tabbed mode), etc.
        */
        class ChildData
        {
            public:

            //! constructor
            ChildData( void )
            {}

            //! destructor
            virtual ~ChildData( void )
            {}

            //! disconnect all signals
            void disconnect( void );

            Signal _destroyId;
            Signal _addId;
            Signal _enterId;
            Signal _leaveId;
        };

        //! map registered children and corresponding data
        typedef std::map<GtkWidget*, ChildData> ChildDataMap;
        ChildDataMap _childrenData;

    };

}

#endif
