//////////////////////////////////////////////////////////////////////////////
// oxygenwindowmanager.cpp
// pass some window mouse press/release/move event actions to window manager
// -------------------
//
// Copyright (c) 2010 Cédric Bellegarde <gnumdk@gmail.com>
//
// Largely inspired from Qtcurve style
// Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
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

#include <string>

#include "oxygenwindowmanager.h"
#include "oxygenstyle.h"

namespace Oxygen
{
    static bool drag = false;
    
    //_________________________________________________
    WindowManager::WindowManager( GtkWidget *widget )
    {
        // setup window management
        if( !g_object_get_data( G_OBJECT( widget ), "OXYGEN_WM_MOVE_HACK_SET" ) ) 
        {

            // Force widget to listen to events
            gtk_widget_add_events( widget, GDK_BUTTON_RELEASE_MASK | 
                                            GDK_BUTTON_PRESS_MASK   |
                                            GDK_LEAVE_NOTIFY_MASK   | 
                                            GDK_BUTTON1_MOTION_MASK );

            g_object_set_data( G_OBJECT( widget ), "OXYGEN_WM_MOVE_HACK_SET", ( gpointer )1 );
            
            _leaveId.connect( G_OBJECT( widget ), "leave-notify-event", G_CALLBACK( wmButtonRelease ), NULL );
            _destroyId.connect( G_OBJECT( widget ), "destroy-event", G_CALLBACK( wmDestroy ), this );
            _styleId.connect( G_OBJECT( widget ), "style-set", G_CALLBACK( wmStyleSet ), this );
            _pressId.connect( G_OBJECT( widget ), "button-press-event", G_CALLBACK( wmButtonPress ), widget );
            _releaseId.connect( G_OBJECT( widget ), "button-release-event", G_CALLBACK( wmButtonRelease ), widget );
            _motionId.connect( G_OBJECT( widget ), "motion-notify-event", G_CALLBACK( wmMotion ), NULL );
            _widget = widget;
        }
        else
            _widget = NULL;
    }
    
    //_________________________________________________
    WindowManager::~WindowManager()
    {
        if( _widget )
        {
            _leaveId.disconnect();
            _destroyId.disconnect();
            _styleId.disconnect();
            _pressId.disconnect();
            _releaseId.disconnect();
            g_object_steal_data( G_OBJECT( _widget ), "OXYGEN_WM_MOVE_HACK_SET" );
        }
    }
    
    //_________________________________________________
    bool WindowManager::isValid()
    {
        return _widget != NULL;
    }
    
    //_________________________________________________
    bool WindowManager::isWindowDragWidget( GtkWidget *widget, GdkEventButton *event )
    {
        return withinWidget(widget, event )  && useEvent( widget, event );
    }
    
    //_________________________________________________
    bool WindowManager::withinWidget( GtkWidget *widget, GdkEventButton *event )
    {
        GtkAllocation alloc;
        GdkWindow *window = gtk_widget_get_parent_window( widget );
        int nx = 0,
            ny = 0;

        if( !window )
            return true;
            
        /* Need to get absolute co-ordinates... */
#if GTK_CHECK_VERSION(2, 18, 0)
        gtk_widget_get_allocation( widget, &alloc );
#else
        alloc = widget->allocation;
#endif
        gdk_window_get_origin(window, &nx, &ny );
        alloc.x += nx;
        alloc.y += ny;

        return alloc.x <= event->x_root && alloc.y <= event->y_root &&
            (alloc.x + alloc.width) > event->x_root && (alloc.y + alloc.height) > event->y_root;  
    }
    
    //_________________________________________________
    bool WindowManager::useEvent( GtkWidget *widget, GdkEventButton *event )
    {
        bool usable = true;

        // check if there is an hovered tab
        if( GTK_IS_NOTEBOOK( widget ) )
        {
            if( Style::instance().animations().tabWidgetEngine().hoveredTab( widget ) != -1 )
            {
                usable = false;
            }
        }
        // need to check all children that may be listening to event
        else if(GTK_IS_CONTAINER( widget ) )
        {
            GList *containers = NULL;
            containers = g_list_prepend( containers, widget );
            
            while( g_list_length( containers ) )
            {
                GtkContainer *c = GTK_CONTAINER( g_list_nth_data(containers, 0) );
                GList *children = gtk_container_get_children( GTK_CONTAINER( c ) );
                for( GList *child = g_list_first( children ); child && usable ; child = g_list_next( child ) )
                {
                    if( child->data )
                    {
                        if( GTK_IS_CONTAINER( child->data ) )
                        {
                            containers = g_list_prepend( containers, child->data );
                        }
                        
                        // if widget is prelight, we don't need to check where event happen,
                        // any prelight widget indicate we can't do a move
                        if( GTK_WIDGET_STATE( GTK_WIDGET( child->data ) ) == GTK_STATE_PRELIGHT )
                        {
                            usable = false;
                        }
                        // if event happen in widget
                        // check not a notebook: event in but notebook don't get it...
                        else if( GTK_IS_WIDGET( child->data ) && !GTK_IS_NOTEBOOK ( child->data ) && 
                                    event && withinWidget( GTK_WIDGET( child->data ), event ) )
                        {
                            // here deal with notebook: widget may be not visible
                            GdkWindow *window = gtk_widget_get_window( GTK_WIDGET ( child->data ) );
                            if( window && gdk_window_is_visible ( window ) )
                            {
                                // widget listening to press event
                                if( gtk_widget_get_events ( GTK_WIDGET( child->data ) ) & GDK_BUTTON_PRESS_MASK )
                                {
                                    usable = false;
                                }
                                // deal with menu item, GtkMenuItem only listen to
                                // GDK_BUTTON_PRESS_MASK when state == GTK_STATE_PRELIGHT
                                // so previous check are invalids :(
                                //
                                // same for ScrolledWindow, they do not send motion events
                                // to parents so not usable
                                else if( GTK_IS_MENU_ITEM( G_OBJECT( child->data ) ) ||
                                            GTK_IS_SCROLLED_WINDOW( G_OBJECT( child->data ) ) )
                                {
                                    usable = false;
                                }
                            }
                        }
                    }
                }
                if( children )
                    g_list_free( children );
                containers = g_list_remove( containers, c );
            }
        }
        return usable;
    }
    
    //_________________________________________________
    bool WindowManager::wmMove( GtkWidget *widget, int x, int y )
    {
        XEvent     xev;
        GtkWindow  *topLevel = GTK_WINDOW( gtk_widget_get_toplevel( widget ) );
        GdkWindow  *window = gtk_widget_get_window( GTK_WIDGET( topLevel ) );
        GdkDisplay *display = gtk_widget_get_display( GTK_WIDGET( topLevel ) );
        GdkWindow  *root = gdk_screen_get_root_window( gtk_window_get_screen( topLevel ) );
        
        xev.xclient.type = ClientMessage;
        xev.xclient.message_type = gdk_x11_get_xatom_by_name_for_display(display, "_NET_WM_MOVERESIZE");
        xev.xclient.display = GDK_DISPLAY_XDISPLAY(display);
        xev.xclient.window = GDK_WINDOW_XID(window);
        xev.xclient.format = 32;
        xev.xclient.data.l[0] = x;
        xev.xclient.data.l[1] = y;
        xev.xclient.data.l[2] = 8; // NET::Move
        xev.xclient.data.l[3] = Button1;
        xev.xclient.data.l[4] = 0;
        XUngrabPointer(GDK_DISPLAY_XDISPLAY(display), CurrentTime);

        XSendEvent(GDK_DISPLAY_XDISPLAY(display), GDK_WINDOW_XID(root), False,
                SubstructureRedirectMask | SubstructureNotifyMask, &xev);
        
        // force a release as some widgets miss it...
        wmButtonRelease( widget, NULL, NULL );
        
        return true;
    }
    
    //_________________________________________________
    bool WindowManager::wmMotion( GtkWidget *widget, GdkEventMotion *event, gpointer user_data )
    {
        if( drag )
        {
            wmMove( widget, event->x_root, event->y_root );
            return true;
        }
        return false;
    }
    
    //_________________________________________________
    bool WindowManager::wmButtonPress(GtkWidget *widget, GdkEventButton *event, gpointer user_data )
    {
        if( event->type == GDK_BUTTON_PRESS && event->button == 1  &&
            isWindowDragWidget( widget, event ) )
        {  
            drag = true;
            return true;
        }
        return false;
    }
    
    //_________________________________________________
    bool WindowManager::wmButtonRelease(GtkWidget *widget, GdkEventButton *event, gpointer user_data )
    {
        if( drag )
        {
            gtk_grab_remove(widget);
            gdk_pointer_ungrab( CurrentTime );
            drag = false;
            return true;
        }
        return false;
    }
    
    //_________________________________________________
    bool WindowManager::wmStyleSet( GtkWidget *widget, GtkStyle *style, gpointer user_data )
    {
        delete static_cast<WindowManager*>(user_data);
        return false;
    }
    
    //_________________________________________________
    bool WindowManager::wmDestroy( GtkWidget *widget, GtkStyle *style, gpointer user_data )
    {
        delete static_cast<WindowManager*>(user_data);
        return false;
    }
}
