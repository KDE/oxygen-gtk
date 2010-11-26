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
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to
* deal in the Software without restriction, including without limitation the
* rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
* sell copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
* IN THE SOFTWARE.
*/

#include "oxygensignal.h"
#include "oxygentimer.h"
#include "oxygendatamap.h"

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <map>

namespace Oxygen
{

    class WindowManager
    {

        public:

        //! constructor
        WindowManager( void );

        //! destructor
        virtual ~WindowManager();

        //! register widget
        virtual void registerWidget( GtkWidget* );

        //! unregister widget
        virtual void unregisterWidget( GtkWidget* );

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

        //! on button release
        static gboolean wmButtonRelease(GtkWidget*, GdkEventButton*, gpointer );

        //! on mouse leave
        static gboolean wmLeave(GtkWidget*, GdkEventCrossing*, gpointer );

        //! on style change
        static gboolean wmStyleSet( GtkWidget*, GtkStyle*, gpointer );

        //! on window destroy
        static gboolean wmDestroy( GtkWidget*, gpointer );

        //! delayed drag
        static gboolean startDelayedDrag( gpointer );

        //@}

        //! start dragging widget
        bool startDrag( GtkWidget*, GdkEventMotion* );

        //! start dragging widget
        bool startDrag( GtkWidget*, int, int );

        //! start dragging widget
        void startDrag( void )
        { if( _drag && _widget ) startDrag( _widget, _x, _y ); }

        //! finish dragging widget
        bool finishDrag( GtkWidget* );

        //! return true if window is dragable
        bool isWindowDragWidget( GtkWidget*, GdkEventButton* );

        //! return true if event happen in widget
        bool withinWidget( GtkWidget*, GdkEventButton* );

        //! return true if event is a usable drag event
        bool useEvent( GtkWidget*, GdkEventButton* );

        //! return true if event is a usable drag event
        /*! for containers, children are also checked. The method is recursive */
        bool childrenUseEvent( GtkWidget*, GdkEventButton* );

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
            Signal _releaseId;
            Signal _motionId;
            Signal _styleId;
            //@}

        };

        //! perform connections between widget and data
        void connect( GtkWidget*, Data& );

        private:

        //! drag mode
        Mode _mode;

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

        //! drag position
        int _x;
        int _y;

        //! map widgets to data structure
        DataMap<Data> _map;

    };

}

#endif
