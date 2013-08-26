/*
* oxygenwindowmanager.cpp
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

#include "oxygenwindowmanager.h"
#include "oxygenpropertynames.h"
#include "oxygenstyle.h"
#include "config.h"

namespace Oxygen
{


    //_________________________________________________
    WindowManager::WindowManager( void ):
        _useWMMoveResize( true ),
        _cursorLoaded( false ),
        _cursor( 0L ),
        _oldCursor( 0L ),
        _dragMode( Full ),
        _hooksInitialized( false ),
        _dragAboutToStart( false ),
        _dragInProgress( false ),
        _dragDistance( 4 ),
        _dragDelay( 500 ),
        _widget( 0L ),
        _lastRejectedEvent( 0L ),
        _x(-1),
        _y(-1),
        _globalX(-1),
        _globalY(-1),
        _time(0)
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WindowManager::WindowManager" << std::endl;
        #endif

        // black list
        initializeBlackList();

    }

    //_________________________________________________
    WindowManager::~WindowManager( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WindowManager::~WindowManager" << std::endl;
        #endif

        _styleSetHook.disconnect();
        _buttonReleaseHook.disconnect();
        _map.disconnectAll();
        _map.clear();

        // cursor
        if( _cursor ) gdk_cursor_unref( _cursor );

    }

    //_________________________________________________
    void WindowManager::initializeHooks( void )
    {

        if( _hooksInitialized ) return;
        if( _dragMode!=Disabled )
        {
            _styleSetHook.connect( "style-set", (GSignalEmissionHook)styleSetHook, this );
            _buttonReleaseHook.connect( "button-release-event", (GSignalEmissionHook)buttonReleaseHook, this );
        }
        _hooksInitialized = true;

    }

    //_________________________________________________
    bool WindowManager::registerWidget( GtkWidget* widget )
    {

        // load cursor if needed
        if( !_cursorLoaded )
        {
            assert( !_cursor );
            GdkDisplay *display( gtk_widget_get_display( widget ) );
            _cursor = gdk_cursor_new_from_name( display, "all-scroll" );
            _cursorLoaded = true;
        }

        if( _map.contains( widget ) ) return false;

        // check against black listed typenames
        if( widgetIsBlackListed( widget ) )
        {
            registerBlackListWidget( widget );
            return false;
        }

        // check blocking property
        if( g_object_get_data( G_OBJECT( widget ), PropertyNames::noWindowGrab ) )
        {
            registerBlackListWidget( widget );
            return false;
        }

        // Window with no decorations (set by app), let window manage it self
        if( GTK_IS_WINDOW( widget ) && !gtk_window_get_decorated( GTK_WINDOW( widget ) ) )
        {
            registerBlackListWidget( widget );
            return false;
        }

        // widgets used in tabs also must be ignored (happens, unfortunately)
        GtkWidget* parent( gtk_widget_get_parent( widget ) );
        if( GTK_IS_NOTEBOOK( parent ) && Gtk::gtk_notebook_is_tab_label( GTK_NOTEBOOK( parent ), widget ) )
        { return false; }

        /*
        check event mask (for now we only need to do that for GtkWindow)
        The idea is that if the window has been set to receive button_press and button_release events
        (which is not done by default), it likely means that it does something with such events,
        in which case we should not use them for grabbing
        */
        if(
            ( GTK_IS_WINDOW( widget ) || GTK_IS_VIEWPORT( widget ) ) &&
            ( gtk_widget_get_events ( widget ) &
            ( GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK ) ) )
        {
            registerBlackListWidget( widget );
            return false;
        }

        // check for blacklisted parent
        if( widgetHasBlackListedParent( widget ) )
        { return false; }

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::WindowManager::registerWidget -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " " << gtk_widget_get_name( widget )
            << std::endl;
        #endif

        // Force widget to listen to relevant events
        gtk_widget_add_events( widget,
            GDK_BUTTON_RELEASE_MASK |
            GDK_BUTTON_PRESS_MASK   |
            GDK_LEAVE_NOTIFY_MASK   |
            GDK_BUTTON1_MOTION_MASK );

        // allocate new Data object
        Data& data( _map.registerWidget( widget ) );

        // connect signals
        if( _dragMode != Disabled ) connect( widget, data );
        return true;

    }

    //_________________________________________________
    void WindowManager::unregisterWidget( GtkWidget* widget )
    {
        if( !_map.contains( widget ) ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WindowManager::unregisterWidget - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _map.value( widget ).disconnect( widget );
        _map.erase( widget );

        // reset drag is the current drag target
        if( _widget == widget ) resetDrag();

    }

    //_________________________________________________
    bool WindowManager::registerBlackListWidget( GtkWidget* widget )
    {

        // make sure that widget is not already connected
        if( _blackListWidgets.find( widget ) != _blackListWidgets.end() ) return false;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WindowManager::registerBlackListWidget - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

      // connect destroy signal and insert in map
        Signal destroyId;
        destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( wmBlackListDestroy ), this );
        _blackListWidgets.insert( std::make_pair( widget, destroyId ) );
        return true;

    }

    //_________________________________________________
    void WindowManager::unregisterBlackListWidget( GtkWidget* widget )
    {

        WidgetMap::iterator iter( _blackListWidgets.find( widget ) );
        if( iter == _blackListWidgets.end() ) return;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WindowManager::unregisterBlackListWidget - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        iter->second.disconnect();
        _blackListWidgets.erase( widget );

    }

    //_________________________________________________
    void WindowManager::setDragMode( WindowManager::Mode mode )
    {
        if( mode == _dragMode ) return;

        // connect/disconnect all data in map, based on new and old mode
        if( mode == Disabled ) { _map.disconnectAll(); }
        else if( _dragMode == Disabled )
        {
            DataMap<Data>::Map& map( _map.map() );
            for( DataMap<Data>::Map::iterator iter = map.begin(); iter != map.end(); ++iter )
            { connect( iter->first, iter->second ); }
        }

        // assign new mode
        _dragMode = mode;

    }

    //_________________________________________________
    gboolean WindowManager::wmDestroy( GtkWidget* widget, gpointer data )
    {
        static_cast<WindowManager*>(data)->unregisterWidget( widget );
        return false;
    }

    //_________________________________________________
    gboolean WindowManager::wmBlackListDestroy( GtkWidget* widget, gpointer data )
    {
        static_cast<WindowManager*>(data)->unregisterBlackListWidget( widget );
        return false;
    }

    //_________________________________________________
    gboolean WindowManager::wmButtonPress(GtkWidget *widget, GdkEventButton* event, gpointer data )
    {

        if( event->type == GDK_BUTTON_PRESS && event->button == Gtk::LeftButton )
        {

            const bool accepted( static_cast<WindowManager*>(data)->canDrag( widget, event ) );
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::WindowManager::wmButtonPress -"
                << " event: " << event
                << " widget: " << widget
                << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << " " << gtk_widget_get_name( widget )
                << " accepted: " << accepted
                << std::endl;
            #endif

            return accepted;

        } else return false;

    }

    //_________________________________________________
    gboolean WindowManager::wmLeave(GtkWidget*, GdkEventCrossing*, gpointer data )
    {
        WindowManager& manager( *static_cast<WindowManager*>( data ) );
        return (gboolean) ( manager.useWMMoveResize() && manager.resetDrag() );
    }

    //_________________________________________________
    gboolean WindowManager::wmMotion( GtkWidget *widget, GdkEventMotion* event, gpointer data )
    { return (gboolean) static_cast<WindowManager*>(data)->startDrag( widget, event ); }

    //_________________________________________________________________
    gboolean WindowManager::startDelayedDrag( gpointer data )
    {
        static_cast<WindowManager*>( data )->startDrag();
        return FALSE;
    }

    //_________________________________________________________________
    gboolean WindowManager::styleSetHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // never register widgets that are possibly applets
        if( Gtk::gtk_widget_is_applet( widget ) ) return TRUE;

        // cast data to window manager
        WindowManager &manager( *static_cast<WindowManager*>(data ) );


        bool registered( false );
        if( GTK_IS_WINDOW( widget ) ||
            GTK_IS_VIEWPORT( widget ) ||
            GTK_IS_TOOLBAR( widget ) ||
            GTK_IS_MENU_BAR( widget ) ||
            GTK_IS_NOTEBOOK( widget ) )
        {

            // top-level windows
            registered = manager.registerWidget( widget );

        } else if(
            Gtk::gtk_button_is_in_path_bar(widget) &&
            Gtk::g_object_is_a( G_OBJECT( gtk_widget_get_parent( widget ) ),  "GtkPathBar" ) ) {

            // path bar widgets
            registered = manager.registerWidget( widget );

        }

        #if OXYGEN_DEBUG
        if( registered )
        {
            std::cerr
                << "Oxygen::WindowManager::styleSetHook -"
                << " registering: " << widget
                << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
        }
        #else
        // silence the compiler
        (void)registered;
        #endif

        return TRUE;

    }

    //_________________________________________________________________
    gboolean WindowManager::buttonReleaseHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // cast data to window manager
        WindowManager &manager( *static_cast<WindowManager*>(data ) );

        // check mode
        if( manager._dragMode == Disabled ) return TRUE;

        // check if drag is in progress, and reset if yes
        if( manager._dragAboutToStart || manager._dragInProgress )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::WindowManager::buttonReleaseHook -"
                << " widget: " << widget
                << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << " " << gtk_widget_get_name( widget )
                << std::endl;
            #endif

            // reset cursor
            if( !manager._useWMMoveResize && manager._dragInProgress )
            { manager.unsetCursor( widget ); }

            manager.resetDrag();

        }

        return TRUE;
    }

    //_________________________________________________________________
    bool WindowManager::startDrag( GtkWidget* widget, GdkEventMotion* event )
    {

        // make sure drag is enabled
        if( !_dragAboutToStart ) return false;

        // check displacement with respect to drag start, as long as not already started
        if( !_dragInProgress )
        {
            const int distance( abs( _globalX - int(event->x_root) ) + abs( _globalY - int(event->y_root) ) );

            if( distance > 0 && _timer.isRunning() ) _timer.stop();
            if( distance < _dragDistance ) return false;

        }

        // start drag from current position
        if( _useWMMoveResize )
        {

            return startDrag( widget, int(event->x_root), int(event->y_root), event->time );

        } else {

            if( !_dragInProgress )
            {
                setCursor( widget );
                _dragInProgress = true;
            }

            GtkWindow* topLevel = GTK_WINDOW( gtk_widget_get_toplevel( widget ) );
            int wx, wy;
            gtk_window_get_position( topLevel, &wx, &wy );
            gtk_window_move( topLevel, wx + event->x - _x, wy + event->y - _y );
            return true;

        }

    }

    //_________________________________________________________________
    bool WindowManager::startDrag( GtkWidget* widget, int x, int y, guint32 time )
    {

        // create xevent and send.
        if( _useWMMoveResize )
        {

            _dragInProgress = true;
            GtkWindow* topLevel = GTK_WINDOW( gtk_widget_get_toplevel( widget ) );
            gtk_window_begin_move_drag( topLevel, Gtk::LeftButton, x, y, time );
            resetDrag();

        } else if( !_dragInProgress ) {

            _dragInProgress = true;
            setCursor( widget );

        }

        return true;

    }

    //_________________________________________________________________
    void WindowManager::setCursor( GtkWidget* widget )
    {

        GdkWindow* window( gtk_widget_get_window( gtk_widget_get_toplevel( widget ) ) );
        _oldCursor = gdk_window_get_cursor( window );
        gdk_window_set_cursor( window, _cursor );
    }

    //_________________________________________________________________
    void WindowManager::unsetCursor( GtkWidget* widget )
    {
        GdkWindow* window( gtk_widget_get_window( gtk_widget_get_toplevel( widget ) ) );
        gdk_window_set_cursor( window, _oldCursor );
    }

    //_________________________________________________
    bool WindowManager::resetDrag( void )
    {

        _widget = 0L;
        _lastRejectedEvent = 0L;
        _x = -1;
        _y = -1;
        _globalX = -1;
        _globalY = -1;
        _time = 0;

        // stop timer
        if( _timer.isRunning() ) _timer.stop();

        if( _dragAboutToStart || _dragInProgress )
        {

            _dragAboutToStart = false;
            _dragInProgress = false;
            return true;

        } else return false;

    }

    //_________________________________________________
    bool WindowManager::canDrag( GtkWidget* widget, GdkEventButton* event )
    {

        if( _dragMode == Disabled ) return false;
        else if( !_dragAboutToStart && withinWidget(widget, event ) && useEvent( widget, event ) )
        {

            // store widget and event position
            _widget = widget;
            _x = int(event->x);
            _y = int(event->y);
            _globalX = int(event->x_root);
            _globalY = int(event->y_root);
            _time = event->time;

            // start timer
            if( _timer.isRunning() ) _timer.stop();
            _timer.start( _dragDelay, (GSourceFunc)startDelayedDrag, this );

            // enable drag and accept
            _dragAboutToStart = true;
            return true;

        } else {

            // mark event as rejected
            _lastRejectedEvent = event;
            return false;
        }

    }

    //_________________________________________________
    bool WindowManager::withinWidget( GtkWidget* widget, GdkEventButton* event ) const
    {

        // get top level widget
        GtkWidget* topLevel( gtk_widget_get_toplevel( widget ) );
        if( !topLevel ) return true;

        // get top level window;
        GdkWindow *window( gtk_widget_get_window( topLevel ) );
        if( !window ) return true;

        // translate widget position to topLevel
        int wx(0);
        int wy(0);
        gtk_widget_translate_coordinates( widget, topLevel, wx, wy, &wx, &wy );

        // translate to absolute coordinates
        int nx(0);
        int ny(0);
        gdk_window_get_origin( window, &nx, &ny );
        wx += nx;
        wy += ny;

        // get widget size.
        GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );

        // translate event position in 'local' coordinates with respect to widget's window
        const int xLocal  = int(event->x_root) - wx + allocation.x;
        const int yLocal  = int(event->y_root) - wy + allocation.y;

        if( GTK_IS_NOTEBOOK( widget ) )
        {

            GtkAllocation tabbarRect;
            Gtk::gtk_notebook_get_tabbar_rect( GTK_NOTEBOOK( widget ), &tabbarRect );

            // compare to event local position
            if( !Gtk::gdk_rectangle_contains( &tabbarRect, xLocal, yLocal ) ) return false;
            else if( !Style::instance().animations().tabWidgetEngine().contains( widget ) ) return false;
            else return !Style::instance().animations().tabWidgetEngine().isInTab( widget, xLocal, yLocal );

        } else {

            // compare to event position
            return Gtk::gdk_rectangle_contains( &allocation, xLocal, yLocal );

        }

    }

    //_________________________________________________
    bool WindowManager::useEvent( GtkWidget* widget, GdkEventButton* event )
    {

        // check against mode
        if( _dragMode == Disabled ) return false;
        if( _dragMode == Minimal && !( GTK_IS_TOOLBAR( widget ) || GTK_IS_MENU_BAR( widget ) ) ) return false;
        if( _lastRejectedEvent && event == _lastRejectedEvent ) return false;

        // always accept if widget is not a container
        if( !GTK_IS_CONTAINER( widget ) ) return true;

        // if widget is a notebook, accept if there is no hovered tab
        bool useEvent( true );
        if( GTK_IS_NOTEBOOK( widget ) )
        {

            useEvent =
                ( !Gtk::gtk_notebook_has_visible_arrows( GTK_NOTEBOOK( widget ) ) ) &&
                Style::instance().animations().tabWidgetEngine().contains( widget ) &&
                Style::instance().animations().tabWidgetEngine().hoveredTab( widget ) == -1 &&
                childrenUseEvent( widget, event, false );

        } else {

            if( GTK_IS_WINDOW( widget ) )
            {

                // check hint
                const GdkWindowTypeHint hint( gtk_window_get_type_hint( GTK_WINDOW( widget ) ) );
                if( hint == GDK_WINDOW_TYPE_HINT_UTILITY ) return false;

            }

            useEvent = childrenUseEvent( widget, event, false );

        }

        return useEvent;

    }

    //_________________________________________________
    bool WindowManager::childrenUseEvent( GtkWidget* widget, GdkEventButton* event, bool inNoteBook ) const
    {
        // accept, by default
        bool usable = true;

        // get children and check
        GList *children = gtk_container_get_children( GTK_CONTAINER( widget ) );
        for( GList *child = g_list_first( children ); child && usable ; child = g_list_next( child ) )
        {

            // cast child to GtkWidget
            if( !GTK_IS_WIDGET( child->data ) ) continue;
            GtkWidget* childWidget( GTK_WIDGET( child->data ) );

            // check widget state and type
            if( gtk_widget_get_state( childWidget ) == GTK_STATE_PRELIGHT )
            {

                // if widget is prelight, we don't need to check where event happen,
                // any prelight widget indicate we can't do a move
                usable = false;
                continue;

            }

            GdkWindow *window = gtk_widget_get_window( childWidget );
            if( !( window && gdk_window_is_visible( window ) ) )
            { continue; }

            if( GTK_IS_NOTEBOOK( childWidget ) ) inNoteBook = true;

            if( !( event && withinWidget( childWidget, event ) ) )
            { continue; }

            // do not propagate if widget's cursor is currently modified,
            // since it is usually indicative of mouse action
            GdkCursor* cursor = gdk_window_get_cursor( window );
            if( cursor && gdk_cursor_get_cursor_type( cursor ) != GDK_ARROW )
            { usable = false; }

            // check special cases for which grab should not be enabled
            if( usable && (
                widgetIsBlackListed( childWidget ) ||
                ( GTK_IS_NOTEBOOK( widget ) && Gtk::gtk_notebook_is_tab_label( GTK_NOTEBOOK( widget ), childWidget ) ) ||
                ( GTK_IS_BUTTON( childWidget ) && gtk_widget_get_state( childWidget ) != GTK_STATE_INSENSITIVE ) ||
                ( gtk_widget_get_events ( childWidget ) & (GDK_BUTTON_PRESS_MASK|GDK_BUTTON_RELEASE_MASK) ) ||
                ( GTK_IS_MENU_ITEM( childWidget ) ) ||
                ( GTK_IS_SCROLLED_WINDOW( childWidget ) && ( !inNoteBook || gtk_widget_is_focus( childWidget ) ) ) ) )
            { usable = false; }

            // if child is a container and event has been accepted so far, also check it, recursively
            if( usable && GTK_IS_CONTAINER( childWidget ) )
            { usable = childrenUseEvent( childWidget, event, inNoteBook); }

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::WindowManager::childrenUseEvent -"
                << " event: " << event
                << " widget: " << childWidget
                << " (" << G_OBJECT_TYPE_NAME( childWidget ) << ")"
                << " " << gtk_widget_get_name( childWidget )
                << " usable: " << usable
                << std::endl;
            #endif

        }

        // free list
        if( children ) g_list_free( children );

        return usable;
    }

    //_________________________________________________
    bool WindowManager::widgetHasBlackListedParent( GtkWidget* widget ) const
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::WindowManager::widgetHasBlackListedParent - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        // loop over widget parent
        for( GtkWidget* parent = gtk_widget_get_parent( widget ); parent; parent = gtk_widget_get_parent( parent ) )
        { if( _blackListWidgets.find( parent ) != _blackListWidgets.end() ) return true; }

        return false;
    }

    //_________________________________________________
    void WindowManager::initializeBlackList( void )
    {
        // clear list
        _blackList.clear();
        _blackList.push_back( "GtkScale" );
        _blackList.push_back( "GimpColorBar" );
        _blackList.push_back( "GladeDesignLayout" );
        _blackList.push_back( "GooCanvas" );
        _blackList.push_back( "GtkPizza" );
        _blackList.push_back( "MetaFrames" );
        _blackList.push_back( "SPHRuler" );
        _blackList.push_back( "SPVRuler" );
        _blackList.push_back( "GtkPlug" );
    }

    //________________________________________________________________________________
    void WindowManager::connect( GtkWidget* widget, WindowManager::Data& data )
    {
        data._destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( wmDestroy ), this );
        data._pressId.connect( G_OBJECT( widget ), "button-press-event", G_CALLBACK( wmButtonPress ), this );
        data._leaveId.connect( G_OBJECT( widget ), "leave-notify-event", G_CALLBACK( wmLeave ), this );
        data._motionId.connect( G_OBJECT( widget ), "motion-notify-event", G_CALLBACK( wmMotion ), this );
    }

    //_________________________________________________
    void WindowManager::Data::disconnect( GtkWidget* )
    {

        _leaveId.disconnect();
        _destroyId.disconnect();
        _pressId.disconnect();
        _motionId.disconnect();

    }

}
