#ifndef oxygenwindowmanager_h
#define oxygenwindowmanager_h

/*
* oxygenwindowmanager.h
* pass some window mouse press/release/move event actions to window manager
* -------------------
*
* Copyright (c) 2010 Cédric Bellegarde <gnumdk@gmail.com>
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* Largely inspired from Qtcurve style
* Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
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

#include "oxygendatamap.h"
#include "oxygengtkutils.h"
#include "oxygenhook.h"
#include "oxygensignal.h"
#include "oxygentimer.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <algorithm>
#include <string>
#include <vector>

namespace Oxygen
{

    class WindowManager
    {

        public:

        //! constructor
        WindowManager( void );

        //! destructor
        virtual ~WindowManager();

        //! initialize hooks
        void initializeHooks( void );

        //! window grab mode
        enum Mode
        {
            Disabled,
            Minimal,
            Full
        };

        //! window grab mode
        void setMode( Mode mode );

        //! drag distance
        void setDragDistance( int value )
        { _dragDistance = value; }

        //! drag delay
        void setDragDelay( int value )
        { _dragDelay = value; }

        protected:

        //!@name callbacks
        //@{

        //! on motion event
        static gboolean wmMotion( GtkWidget*, GdkEventMotion*, gpointer );

        //! on button press
        static gboolean wmButtonPress( GtkWidget*, GdkEventButton*, gpointer );

        //! on mouse leave
        static gboolean wmLeave(GtkWidget*, GdkEventCrossing*, gpointer );

        //! on window destroy
        static gboolean wmDestroy( GtkWidget*, gpointer );

        //! on window destroy
        static gboolean wmBlackListDestroy(  GtkWidget*, gpointer );

        //! delayed drag
        static gboolean startDelayedDrag( gpointer );

        //! style-set hook
        static gboolean styleSetHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //! mouse button release event hook
        static gboolean buttonReleaseHook( GSignalInvocationHint*, guint, const GValue*, gpointer );

        //@}

        //! register widget
        /*! returns true if widget is effictively registered */
        bool registerWidget( GtkWidget* );

        //! unregister widget
        void unregisterWidget( GtkWidget* );

        //! register blacklist widget
        bool registerBlackListWidget( GtkWidget* );

        //! unregister blacklist widget
        void unregisterBlackListWidget( GtkWidget* );

        //! start dragging widget
        bool startDrag( GtkWidget*, GdkEventMotion* );

        //! start dragging widget
        bool startDrag( GtkWidget*, int, int );

        //! start dragging widget
        void startDrag( void )
        { if( _drag && _widget ) startDrag( _widget, _x, _y ); }

        //! finish dragging widget
        bool finishDrag( void );

        //! return true if window is dragable
        bool isWindowDragWidget( GtkWidget*, GdkEventButton* );

        //! return true if event happen in widget
        bool withinWidget( GtkWidget*, GdkEventButton* ) const;

        //! return true if event is a usable drag event
        bool useEvent( GtkWidget*, GdkEventButton* );

        //! return true if event is a usable drag event
        /*! for containers, children are also checked. The method is recursive */
        bool childrenUseEvent( GtkWidget*, GdkEventButton*, bool ) const;

        //! used to decide whether a widget is black-listed
        class BlackListFTor
        {
            public:

            //! constructor
            BlackListFTor( GObject* object ):
                _object( object )
                {}

            //! predicate
            bool operator() (const std::string& objectName ) const
            { return Gtk::g_object_is_a( _object, objectName ); }

            private:

            //! object
            GObject* _object;

        };

        //! initialize black list
        /*! stores list of widgets for which you do not want the window to be grabbed */
        void initializeBlackList( void );

        //! return true if widget is black listed for grabbing
        bool widgetIsBlackListed( GtkWidget* widget ) const
        { return std::find_if( _blackList.begin(), _blackList.end(), BlackListFTor( G_OBJECT( widget ) ) ) != _blackList.end(); }

        //! return true if widget has a black listed parent
        bool widgetHasBlackListedParent( GtkWidget* widget ) const;

        //! stores connections
        class Data
        {
            public:

            //! constructor
            Data( void )
            {}

            //! destructor
            virtual ~Data( void )
            {}

            //! connect
            /*! it is needed to insert in DataMap, but does nothing */
            virtual void connect( GtkWidget* )
            {}

            //! disconnect
            virtual void disconnect( GtkWidget* );

            //!@name signals
            //@{
            Signal _leaveId;
            Signal _destroyId;
            Signal _pressId;
            Signal _motionId;
            //@}

        };

        //! perform connections between widget and data
        void connect( GtkWidget*, Data& );

        private:

        //! drag mode
        Mode _mode;

        //! true when hooks are initialized
        bool _hooksInitialized;

        //! style set hook. Used to register widgets
        Hook _styleSetHook;

        //! mouse button release event hook
        Hook _buttonReleaseHook;

        //! timer
        Timer _timer;

        //! true if in drag mode
        bool _drag;

        //! drag distance
        int _dragDistance;

        //! drag delay
        int _dragDelay;

        //! active widget
        GtkWidget* _widget;

        //! keep track of the last rejected button event to reject it again if passed to some parent widget
        /*! this spares some time (by not processing the same event twice), and prevents some bugs */
        GdkEventButton* _lastRejectedEvent;

        //! drag position
        int _x;
        int _y;

        //! widget typenames black-list
        std::vector<std::string> _blackList;

        //! widget black-list
        typedef std::map< GtkWidget*, Signal > WidgetMap;
        WidgetMap _blackListWidgets;


        //! map widgets to data structure
        DataMap<Data> _map;

    };

}

#endif
