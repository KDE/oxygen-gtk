#ifndef oxygenwindowmanager_h
#define oxygenwindowmanager_h

//////////////////////////////////////////////////////////////////////////////
// oxygenwindowmanager.h
// pass some window mouse press/release/move event actions to window manager
// -------------------
//
// Copyright (c) 2010 Cédric Bellegarde <gnumdk@gmail.com>
//
// Largely inspired from Qtcurve style
// Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//////////////////////////////////////////////////////////////////////////////

#include "animations/oxygensignal.h"

#include <gdk/gdk.h>
#include <gtk/gtk.h>

namespace Oxygen
{      
    class WindowManager
    {
        public:
            
        //! constructor, will delete itself, don't try to delete if isValid()
        WindowManager( GtkWidget *widget );
        
        //! destructor
        ~WindowManager();

        //! return if object is correctly constructed, if not, you need to delete it
        bool isValid();

        protected:
        
        //! on motion event
        static bool wmMotion( GtkWidget *widget, GdkEventMotion *event, gpointer user_data );
        
        //! on button press
        static bool wmButtonPress( GtkWidget *widget, GdkEventButton *event, gpointer user_data );
        
        //! on button release
        static bool wmButtonRelease(GtkWidget *widget, GdkEventButton *event, gpointer user_data );
        
        //! on style change
        static bool wmStyleSet( GtkWidget *widget, GtkStyle *style, gpointer user_data );
        
        //! on window destroy
        static bool wmDestroy( GtkWidget *widget, GtkStyle *style, gpointer user_data );

        private:
        
        //! return true if window is dragable
        static bool isWindowDragWidget( GtkWidget *widget, GdkEventButton *event );
        
        //! return true if event happen in widget
        static bool withinWidget( GtkWidget *widget, GdkEventButton *event );
                    
        //! return true if event is a usable drag event
        static bool useEvent( GtkWidget *widget, GdkEventButton *event );
        
        //! move widget at position
        static bool wmMove( GtkWidget *widget, int x, int y );
         
        GtkWidget *_widget;
        Signal _leaveId;
        Signal _destroyId;
        Signal _pressId;
        Signal _releaseId;
        Signal _motionId;
        Signal _styleId;
    };
}
#endif