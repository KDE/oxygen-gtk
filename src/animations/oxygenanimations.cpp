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

#include "oxygenanimations.h"
#include "../oxygengtkutils.h"
#include "../oxygenqtsettings.h"
#include "../config.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //_________________________________________
    Animations::Animations( void ):
        _enabled( true ),
        _hooksInitialized( false )
    {

        // create engines
        registerEngine( _backgroundHintEngine = new BackgroundHintEngine( this ) );
        registerEngine( _comboEngine = new ComboEngine( this ) );
        registerEngine( _comboBoxEngine = new ComboBoxEngine( this ) );
        registerEngine( _comboBoxEntryEngine = new ComboBoxEntryEngine( this ) );
        registerEngine( _dialogEngine = new DialogEngine( this ) );
        registerEngine( _hoverEngine = new HoverEngine( this ) );
        registerEngine( _mainWindowEngine = new MainWindowEngine( this ) );
        registerEngine( _scrollBarEngine = new ScrollBarEngine( this ) );
        registerEngine( _scrolledWindowEngine = new ScrolledWindowEngine( this ) );
        registerEngine( _innerShadowEngine = new InnerShadowEngine( this ) );
        registerEngine( _tabWidgetEngine = new TabWidgetEngine( this ) );
        registerEngine( _treeViewEngine = new TreeViewEngine( this ) );
        registerEngine( _widgetSizeEngine = new WidgetSizeEngine( this ) );

        // animations specific engines
        registerEngine( _widgetStateEngine = new WidgetStateEngine( this ) );
        registerEngine( _arrowStateEngine = new ArrowStateEngine( this ) );
        registerEngine( _scrollBarStateEngine = new ScrollBarStateEngine( this ) );
        registerEngine( _tabWidgetStateEngine = new TabWidgetStateEngine( this ) );
        registerEngine( _treeViewStateEngine = new TreeViewStateEngine( this ) );
        registerEngine( _menuBarStateEngine = new MenuBarStateEngine( this ) );
        registerEngine( _menuStateEngine = new MenuStateEngine( this ) );
        registerEngine( _toolBarStateEngine = new ToolBarStateEngine( this ) );

    }

    //____________________________________________________________________________________________
    Animations::~Animations( void )
    {

        // delete all engines
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { delete *iter; }

        // clear hooks
        _comboBoxHook.disconnect();

        // FIXME: should we also clear the allWidgets list ?

    }

    //_________________________________________
    void Animations::initialize( const QtSettings& settings )
    {

        const bool animationsEnabled( settings.animationsEnabled() );

        // pass animations configuration to engines
        widgetStateEngine().setApplicationName( settings.applicationName() );
        widgetStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        widgetStateEngine().setDuration( settings.genericAnimationsDuration() );

        arrowStateEngine().setApplicationName( settings.applicationName() );
        arrowStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        arrowStateEngine().setDuration( settings.genericAnimationsDuration() );

        scrollBarStateEngine().setApplicationName( settings.applicationName() );
        scrollBarStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        scrollBarStateEngine().setDuration( settings.genericAnimationsDuration() );

        tabWidgetStateEngine().setApplicationName( settings.applicationName() );
        tabWidgetStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        tabWidgetStateEngine().setDuration( settings.genericAnimationsDuration() );

        treeViewStateEngine().setApplicationName( settings.applicationName() );
        treeViewStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        treeViewStateEngine().setDuration( settings.genericAnimationsDuration() );

        menuBarStateEngine().setApplicationName( settings.applicationName() );
        menuBarStateEngine().setAnimationsEnabled( animationsEnabled && (settings.menuBarAnimationType() != None) );
        menuBarStateEngine().setFollowMouse( settings.menuBarAnimationType() == FollowMouse );
        menuBarStateEngine().setDuration( settings.menuBarAnimationsDuration() );
        menuBarStateEngine().setFollowMouseAnimationsDuration( settings.menuBarFollowMouseAnimationsDuration() );

        menuStateEngine().setApplicationName( settings.applicationName() );
        menuStateEngine().setEnabled( animationsEnabled && (settings.menuAnimationType() != None) );
        menuStateEngine().setFollowMouse( settings.menuAnimationType() == FollowMouse );
        menuStateEngine().setDuration( settings.menuAnimationsDuration() );
        menuStateEngine().setFollowMouseAnimationsDuration( settings.menuFollowMouseAnimationsDuration() );

        // for now, only Fade animations mode is supported for toolbar animations
        toolBarStateEngine().setApplicationName( settings.applicationName() );
        toolBarStateEngine().setEnabled( animationsEnabled && (settings.toolBarAnimationType() != None) );
        toolBarStateEngine().setFollowMouse(  settings.toolBarAnimationType() == FollowMouse );
        toolBarStateEngine().setDuration( settings.genericAnimationsDuration() );
        toolBarStateEngine().setFollowMouseAnimationsDuration( settings.toolBarAnimationsDuration() );

    }

    //____________________________________________________________________________________________
    void Animations::initializeHooks( void )
    {
        if( _hooksInitialized ) return;

        # if ENABLE_COMBOBOX_LIST_RESIZE
        _comboBoxHook.connect( "size-allocate", (GSignalEmissionHook)comboBoxHook, this );
        #endif

        _innerShadowHook.connect( "realize", (GSignalEmissionHook)innerShadowHook, this );

        _hooksInitialized = true;
    }

    //____________________________________________________________________________________________
    bool Animations::registerWidget( GtkWidget* widget )
    {

        if( _allWidgets.find( widget ) != _allWidgets.end() ) return false;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Animations::registerWidget - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        WidgetData data;
        data._destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( destroyNotifyEvent ), this );
        _allWidgets.insert( std::make_pair( widget, data ) );
        return true;

    }

    //____________________________________________________________________________________________
    void Animations::unregisterWidget( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Animations::unregisterWidget - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        // find in map
        WidgetMap::iterator iter( _allWidgets.find( widget ) );
        assert( iter != _allWidgets.end() );

        // disconnect signal
        iter->second._destroyId.disconnect();

        // erase from map
        _allWidgets.erase( widget );

        // erase from all maps
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { (*iter)->unregisterWidget( widget ); }

    }

    //____________________________________________________________________________________________
    void Animations::setEnabled( bool value )
    {
        if( value == _enabled ) return;
        _enabled = value;
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { (*iter)->setEnabled( value ); }
    }

    //____________________________________________________________________________________________
    gboolean Animations::destroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<Animations*>(data)->unregisterWidget( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    gboolean Animations::comboBoxHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;
        if( !GTK_IS_WINDOW( widget ) ) return TRUE;

        GtkWindow* window( GTK_WINDOW( widget ) );
        if( gtk_window_get_type_hint( window ) != GDK_WINDOW_TYPE_HINT_COMBO ) return TRUE;

        Animations& animations( *static_cast<Animations*>(data) );
        GtkWidget* combobox = animations.comboBoxEngine().find( widget );
        if( !combobox ) combobox = animations.comboBoxEntryEngine().find( widget );
        if( !combobox ) combobox = animations.comboEngine().find( widget );
        if( !combobox ) return true;

        int w, h;
        gtk_window_get_size( window, &w, &h );

        gint targetX, dummy, y;
        gtk_window_get_position( window, &dummy, &y );
        gdk_window_get_origin( gtk_widget_get_window( combobox ), &targetX, &dummy );

        const GtkAllocation comboAllocation( Gtk::gtk_widget_get_allocation( combobox ) );
        gtk_window_move( window, targetX + comboAllocation.x + 3, y );

        const GtkAllocation widgetAllocation( Gtk::gtk_widget_get_allocation( widget ) );
        gtk_widget_set_size_request( widget, comboAllocation.width - 6, widgetAllocation.height );

        return TRUE;

    }

    //____________________________________________________________________________________________
    gboolean Animations::innerShadowHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {
        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;
        if( !GTK_IS_TREE_VIEW( widget ) && !GTK_IS_TEXT_VIEW(widget) ) return TRUE;
        if( Gtk::gtk_combobox_is_tree_view( widget ) ) return TRUE;

        GtkWidget* parent(gtk_widget_get_parent(widget));
        if( !GTK_IS_SCROLLED_WINDOW( parent ) ) return TRUE;

        #if OXYGEN_DEBUG
        std::cerr << "got " << G_OBJECT_TYPE_NAME(widget) << "; parent " << G_OBJECT_TYPE_NAME(parent) << "; ";
        gchar* widgetPath;
        gtk_widget_path( widget, 0L, &widgetPath, 0L);
        std::cerr << "widget path: " << widgetPath << "; Treeview? " << (GTK_IS_TREE_VIEW(widget)?"TRUE":"FALSE") <<"; Textview? " << (GTK_IS_TEXT_VIEW(widget)?"TRUE":"FALSE")<<std::endl;
        g_free( widgetPath );
        #endif

        static_cast<Animations*>(data)->innerShadowEngine().registerWidget( parent );
        static_cast<Animations*>(data)->innerShadowEngine().registerChild( parent, widget );

        return TRUE;

    }

}
