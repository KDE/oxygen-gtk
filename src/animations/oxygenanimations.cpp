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
        _innerShadowsEnabled( true ),
        _hooksInitialized( false )
    {

        // create engines
        registerEngine( _backgroundHintEngine = new BackgroundHintEngine( this ) );
        registerEngine( _comboBoxEngine = new ComboBoxEngine( this ) );
        registerEngine( _comboBoxEntryEngine = new ComboBoxEntryEngine( this ) );
        registerEngine( _dialogEngine = new DialogEngine( this ) );
        registerEngine( _groupBoxEngine = new GroupBoxEngine( this ) );
        registerEngine( _groupBoxLabelEngine = new GroupBoxLabelEngine( this ) );
        registerEngine( _hoverEngine = new HoverEngine( this ) );
        registerEngine( _mainWindowEngine = new MainWindowEngine( this ) );
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

        // delete all engines
        for( BaseEngine::List::iterator iter = _engines.begin(); iter != _engines.end(); ++iter )
        { delete *iter; }

        // clear hooks
        _comboBoxEntryHook.disconnect();
        _backgroundHintHook.disconnect();
        _sizeAllocationHook.disconnect();
        _realizationHook.disconnect();

    }

    //_________________________________________
    void Animations::initialize( const QtSettings& settings )
    {

        const bool animationsEnabled( settings.animationsEnabled() );

        // inner shadows enabled state
        setInnerShadowsEnabled( true );

        // pass animations configuration to engines
        widgetStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        widgetStateEngine().setDuration( settings.genericAnimationsDuration() );

        arrowStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        arrowStateEngine().setDuration( settings.genericAnimationsDuration() );

        scrollBarStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        scrollBarStateEngine().setDuration( settings.genericAnimationsDuration() );

        tabWidgetStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        tabWidgetStateEngine().setDuration( settings.genericAnimationsDuration() );

        treeViewStateEngine().setEnabled( animationsEnabled && settings.genericAnimationsEnabled() );
        treeViewStateEngine().setDuration( settings.genericAnimationsDuration() );

        menuBarStateEngine().setAnimationsEnabled( animationsEnabled && (settings.menuBarAnimationType() != None) );
        menuBarStateEngine().setFollowMouse( settings.menuBarAnimationType() == FollowMouse );
        menuBarStateEngine().setDuration( settings.menuBarAnimationsDuration() );
        menuBarStateEngine().setFollowMouseAnimationsDuration( settings.menuBarFollowMouseAnimationsDuration() );

        menuStateEngine().setEnabled( animationsEnabled && (settings.menuAnimationType() != None) );
        menuStateEngine().setFollowMouse( settings.menuAnimationType() == FollowMouse );
        menuStateEngine().setDuration( settings.menuAnimationsDuration() );
        menuStateEngine().setFollowMouseAnimationsDuration( settings.menuFollowMouseAnimationsDuration() );

        // for now, only Fade animations mode is supported for toolbar animations
        toolBarStateEngine().setEnabled( animationsEnabled && (settings.toolBarAnimationType() != None) );
        toolBarStateEngine().setFollowMouse(  settings.toolBarAnimationType() == FollowMouse );
        toolBarStateEngine().setDuration( settings.genericAnimationsDuration() );
        toolBarStateEngine().setFollowMouseAnimationsDuration( settings.toolBarAnimationsDuration() );

    }

    //____________________________________________________________________________________________
    void Animations::initializeHooks( void )
    {
        if( _hooksInitialized ) return;

        #if ENABLE_COMBOBOX_ENTRY_HACK
        _comboBoxEntryHook.connect( "size-allocate", (GSignalEmissionHook)comboBoxEntryHook, this );
        #endif

        _backgroundHintHook.connect( "realize", (GSignalEmissionHook)backgroundHintHook, this );

        #if ENABLE_INNER_SHADOWS_HACK
        _innerShadowHook.connect( "realize", (GSignalEmissionHook)innerShadowHook, this );
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
    gboolean Animations::sizeAllocationHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;

        // groupbox labels
        if( static_cast<Animations*>( data )->groupBoxLabelEngine().contains( widget ) )
        {
            static_cast<Animations*>( data )->groupBoxLabelEngine().adjustSize( widget );
            return TRUE;
        }

        # if ENABLE_COMBOBOX_LIST_RESIZE
        // comboboxes
        if( !GTK_IS_WINDOW( widget ) ) return TRUE;

        GtkWindow* window( GTK_WINDOW( widget ) );
        if( gtk_window_get_type_hint( window ) != GDK_WINDOW_TYPE_HINT_COMBO ) return TRUE;

        Animations& animations( *static_cast<Animations*>(data) );
        GtkWidget* combobox = animations.comboBoxEngine().find( widget );
        if( !combobox ) combobox = animations.comboBoxEntryEngine().find( widget );
        if( !combobox ) return true;

        int w, h;
        gtk_window_get_size( window, &w, &h );

        gint sourceX, sourceY;
        gint targetX, targetY;
        gtk_window_get_position( window, &sourceX, &sourceY );
        gdk_window_get_origin( gtk_widget_get_window( combobox ), &targetX, &targetY );

        // store allocations
        const GtkAllocation comboAllocation( Gtk::gtk_widget_get_allocation( combobox ) );
        const GtkAllocation widgetAllocation( Gtk::gtk_widget_get_allocation( widget ) );

        const bool widthChanged( widgetAllocation.width != comboAllocation.width - 6 );
        const bool positionChanged( sourceX != targetX + comboAllocation.x + 3 );

        #if OXYGEN_DEBUG
        std::cerr
            << "Animations::comboBoxHook -"
            << " widget: " << widget
            << " widthChanged: " << widthChanged
            << " positionChanged: " << positionChanged
            << std::endl;
        #endif

        // perform move-resize
        if( widthChanged && positionChanged )
        {

            gdk_window_move_resize(
                gtk_widget_get_window( widget ),
                targetX + comboAllocation.x + 3, sourceY,
                comboAllocation.width - 6, widgetAllocation.height );

        } else if( widthChanged ) {

            gdk_window_resize(
                gtk_widget_get_window( widget ),
                comboAllocation.width - 6, widgetAllocation.height );

        } else if( positionChanged ) {

            gdk_window_move(
                gtk_widget_get_window( widget ),
                targetX + comboAllocation.x + 3, sourceY );

        }

        #endif

        return true;

    }

    //____________________________________________________________________________________________
    gboolean Animations::comboBoxEntryHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {

        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return false;

        GtkWidget* parent( 0L );
        if( GTK_IS_ENTRY( widget ) && (parent = Gtk::gtk_parent_combobox_entry( widget ) ) )
        {
            // TODO: remove when bug is fixed upstream
            GtkAllocation rect( Gtk::gtk_widget_get_allocation( widget ) );
            GtkAllocation parentRect( Gtk::gtk_widget_get_allocation( parent ) );
            if( rect.y != parentRect.y )
            {
                GtkAllocation newRect( Gtk::gdk_rectangle( rect.x, parentRect.y, rect.width, parentRect.height ) );
                gtk_widget_size_allocate( widget, &newRect );
            }

        }

        return true;

    }

    //____________________________________________________________________________________________
    gboolean Animations::backgroundHintHook( GSignalInvocationHint*, guint, const GValue* params, gpointer data )
    {
        // get widget from params
        GtkWidget* widget( GTK_WIDGET( g_value_get_object( params ) ) );

        // check type
        if( !GTK_IS_WIDGET( widget ) ) return FALSE;
        if( !GTK_IS_WINDOW( widget ) ) return TRUE;

        Animations& animations( *static_cast<Animations*>(data) );
        animations.backgroundHintEngine().registerWidget( widget );

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

        if( Gtk::gtk_combobox_is_tree_view( widget ) ) return TRUE;
        if( Gtk::g_object_is_a( G_OBJECT( widget ), "SwtFixed" ) ) return TRUE;

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
            gtk_notebook_set_show_border( GTK_NOTEBOOK(widget), FALSE );

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
                static_cast<Animations*>( data )->groupBoxLabelEngine().registerWidget( widget );
                static_cast<Animations*>( data )->groupBoxLabelEngine().adjustSize( widget );

            }

        }

        return TRUE;

    }

}
