/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
        _innerShadowsEnabled( true ),
        _hooksInitialized( false )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Animations::Animations" << std::endl;
        #endif

        // create engines
        registerEngine( _backgroundHintEngine = new BackgroundHintEngine( this ) );
        registerEngine( _comboEngine = new ComboEngine( this ) );
        registerEngine( _comboBoxEngine = new ComboBoxEngine( this ) );
        registerEngine( _comboBoxEntryEngine = new ComboBoxEntryEngine( this ) );
        registerEngine( _dialogEngine = new DialogEngine( this ) );
        registerEngine( _flatWidgetEngine = new FlatWidgetEngine( this ) );
        registerEngine( _groupBoxEngine = new GroupBoxEngine( this ) );
        registerEngine( _groupBoxLabelEngine = new GroupBoxLabelEngine( this ) );
        registerEngine( _hoverEngine = new HoverEngine( this ) );
        registerEngine( _mainWindowEngine = new MainWindowEngine( this ) );
        registerEngine( _menuItemEngine = new MenuItemEngine( this ) );
        registerEngine( _panedEngine = new PanedEngine( this ) );
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
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Animations::~Animations" << std::endl;
        #endif

        // delete all engines
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { delete *iter; }

        // disconnect all signals from map
        for( WidgetMap::iterator iter = _allWidgets.begin(); iter != _allWidgets.end(); iter++ )
        { iter->second.disconnect(); }

        // clear hooks
        _sizeAllocationHook.disconnect();
        _realizationHook.disconnect();
        _innerShadowHook.disconnect();

    }

    //_________________________________________
    void Animations::initialize( const QtSettings& settings )
    {

        const bool animationsEnabled( settings.animationsEnabled() );

        // pass animations configuration to engines
        _widgetStateEngine->setApplicationName( settings.applicationName() );
        _widgetStateEngine->setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        _widgetStateEngine->setDuration( settings.genericAnimationsDuration() );

        _arrowStateEngine->setApplicationName( settings.applicationName() );
        _arrowStateEngine->setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        _arrowStateEngine->setDuration( settings.genericAnimationsDuration() );

        _scrollBarStateEngine->setApplicationName( settings.applicationName() );
        _scrollBarStateEngine->setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        _scrollBarStateEngine->setDuration( settings.genericAnimationsDuration() );

        _tabWidgetStateEngine->setApplicationName( settings.applicationName() );
        _tabWidgetStateEngine->setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        _tabWidgetStateEngine->setDuration( settings.genericAnimationsDuration() );

        _treeViewStateEngine->setApplicationName( settings.applicationName() );
        _treeViewStateEngine->setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        _treeViewStateEngine->setDuration( settings.genericAnimationsDuration() );

        _menuBarStateEngine->setApplicationName( settings.applicationName() );
        _menuBarStateEngine->setAnimationsEnabled( animationsEnabled && (settings.menuBarAnimationType() != None) );
        _menuBarStateEngine->setFollowMouse( settings.menuBarAnimationType() == FollowMouse );
        _menuBarStateEngine->setDuration( settings.menuBarAnimationsDuration() );
        _menuBarStateEngine->setFollowMouseAnimationsDuration( settings.menuBarFollowMouseAnimationsDuration() );

        _menuStateEngine->setApplicationName( settings.applicationName() );
        _menuStateEngine->setEnabled( animationsEnabled && (settings.menuAnimationType() != None) );
        _menuStateEngine->setFollowMouse( settings.menuAnimationType() == FollowMouse );
        _menuStateEngine->setDuration( settings.menuAnimationsDuration() );
        _menuStateEngine->setFollowMouseAnimationsDuration( settings.menuFollowMouseAnimationsDuration() );

        // for now, only Fade animations mode is supported for toolbar animations
        _toolBarStateEngine->setApplicationName( settings.applicationName() );
        _toolBarStateEngine->setEnabled( animationsEnabled && (settings.toolBarAnimationType() != None) );
        _toolBarStateEngine->setFollowMouse(  settings.toolBarAnimationType() == FollowMouse );
        _toolBarStateEngine->setDuration( settings.genericAnimationsDuration() );
        _toolBarStateEngine->setFollowMouseAnimationsDuration( settings.toolBarAnimationsDuration() );

        // background hint engine
        _backgroundHintEngine->setUseBackgroundGradient( settings.useBackgroundGradient() );

    }

    //____________________________________________________________________________________________
    void Animations::initializeHooks( void )
    {
        if( _hooksInitialized ) return;

        // https://bugzilla.gnome.org/show_bug.cgi?id=643416
        #if ENABLE_INNER_SHADOWS_HACK
        if(!getenv("OXYGEN_DISABLE_INNER_SHADOWS_HACK"))
        { _innerShadowHook.connect( "realize", (GSignalEmissionHook)innerShadowHook, this ); }
        #endif

        _sizeAllocationHook.connect( "size-allocate", (GSignalEmissionHook)sizeAllocationHook, this );
        _realizationHook.connect( "realize", (GSignalEmissionHook)realizationHook, this );

        _hooksInitialized = true;
    }

    //____________________________________________________________________________________________
    bool Animations::registerWidget( GtkWidget* widget )
    {

        if( _allWidgets.find( widget ) != _allWidgets.end() ) return false;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Animations::registerWidget - " << widget << " (" << (widget ? G_OBJECT_TYPE_NAME( widget ):"0x0") << ")" << std::endl;
        #endif

        Signal destroyId;
        destroyId.connect( G_OBJECT( widget ), "destroy", G_CALLBACK( destroyNotifyEvent ), this );
        _allWidgets.insert( std::make_pair( widget, destroyId ) );
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
        iter->second.disconnect();

        // erase from map
        _allWidgets.erase( widget );

        // erase from all maps
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { (*iter)->unregisterWidget( widget ); }

    }

    //____________________________________________________________________________________________
    void Animations::setEnabled( bool value )
    {

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
    gboolean Animations::sizeAllocationHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // cast data
        Animations& animations( *static_cast<Animations*>(data) );

        // groupbox labels
        #if ENABLE_GROUPBOX_HACK
        if( animations.groupBoxLabelEngine().contains( widget ) )
        {
            animations.groupBoxLabelEngine().adjustSize( widget );
            return TRUE;
        }
        #endif

        #if ENABLE_COMBOBOX_LIST_RESIZE
        // comboboxes
        if( !GTK_IS_WINDOW( widget ) ) return TRUE;

        GtkWindow* window( GTK_WINDOW( widget ) );
        if( gtk_window_get_type_hint( window ) != GDK_WINDOW_TYPE_HINT_COMBO ) return TRUE;

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
        int uglyShadowWidth=!Gtk::gdk_default_screen_is_composited();
        gtk_window_move( window, targetX + comboAllocation.x + 3 - uglyShadowWidth, y );

        const GtkAllocation widgetAllocation( Gtk::gtk_widget_get_allocation( widget ) );
        gtk_widget_set_size_request( widget, comboAllocation.width - 6 + 2*uglyShadowWidth, widgetAllocation.height );
        #endif

        return TRUE;

    }

    //____________________________________________________________________________________________
    gboolean Animations::innerShadowHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        #if GTK_CHECK_VERSION(2,24,2)

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // check enabled state
        Animations& animations( *static_cast<Animations*>(data) );
        if( !animations.innerShadowsEnabled() ) return TRUE;

        // blacklist
        if( Gtk::g_object_is_a( G_OBJECT( widget ), "SwtFixed" ) ) return TRUE;
        if( Gtk::g_object_is_a( G_OBJECT( widget ), "GtkPizza" ) ) return TRUE;

        GtkWidget* parent(gtk_widget_get_parent(widget));
        if( !GTK_IS_SCROLLED_WINDOW( parent ) ) return TRUE;

        GtkWidget* child(gtk_bin_get_child(GTK_BIN(parent)));
        if(child!=widget) return TRUE;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::Animations::innerShadowHook -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME(widget) << ")"
            << " parent: " << parent << " (" << G_OBJECT_TYPE_NAME(parent) << ")"
            << " widget path: " << Gtk::gtk_widget_path( widget )
            << " isTreeView: " << (GTK_IS_TREE_VIEW(widget)?"true":"false")
            << " isTextView: " << (GTK_IS_TEXT_VIEW(widget)?"true":"false")
            << std::endl;
        #endif

        // force shadow type on known windows
        if( Gtk::gtk_scrolled_window_force_sunken( parent ) )
        { gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( parent ), GTK_SHADOW_IN ); }

        animations.innerShadowEngine().registerWidget( parent );
        animations.innerShadowEngine().registerChild( parent, widget );

        #endif  // Gtk version
        return TRUE;

    }

    //____________________________________________________________________________________________
    gboolean Animations::realizationHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        if( GTK_IS_NOTEBOOK( widget ) )
        { gtk_notebook_set_show_border( GTK_NOTEBOOK(widget), FALSE ); }

        #if ENABLE_GROUPBOX_HACK
        if( GTK_IS_LABEL( widget ) &&  GTK_IS_FRAME( gtk_widget_get_parent( widget ) ) )
        {

            GtkFrame *frame( GTK_FRAME( gtk_widget_get_parent( widget ) ) );
            if( widget == gtk_frame_get_label_widget( frame ) && !Gtk::gtk_widget_find_parent( widget, "GtkPizza" ) )
            {
                #if OXYGEN_DEBUG
                std::cout
                    << "Oxygen::Animations::realizationHook -"
                    << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                    << " parent: " << frame << " (" << G_OBJECT_TYPE_NAME( frame ) << ")"
                    << std::endl;
                #endif

                // modify alignment
                gtk_frame_set_label_align( frame, 0.5, 0.0 );
                gtk_frame_set_shadow_type( frame, GTK_SHADOW_OUT );

                // register to engine
                Animations& animations( *static_cast<Animations*>(data) );
                animations.groupBoxLabelEngine().registerWidget( widget );
                animations.groupBoxLabelEngine().adjustSize( widget );

            }

        }
        #endif

        return TRUE;

    }

}
