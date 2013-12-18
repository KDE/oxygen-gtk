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

#include "oxygenscrolledwindowdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"
#include "../oxygencairocontext.h"
#include "oxygenanimations.h"
#include "../oxygenstyle.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    void ScrolledWindowData::connect( GtkWidget* widget )
    {
        assert( GTK_IS_SCROLLED_WINDOW( widget ) );
        assert( !_target );

        // store target
        _target = widget;

        // register scrollbars
        GtkScrolledWindow* scrolledWindow( GTK_SCROLLED_WINDOW( widget ) );

        if( GtkWidget* hScrollBar = gtk_scrolled_window_get_hscrollbar( scrolledWindow ) )
        { registerChild( hScrollBar ); }

        if( GtkWidget* vScrollBar = gtk_scrolled_window_get_vscrollbar( scrolledWindow ) )
        { registerChild( vScrollBar ); }

        // check child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        if( !child ) return;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ScrolledWindowData::connect -"
            << " child: " << child << " (" << G_OBJECT_TYPE_NAME( child ) << ")"
            << std::endl;
        #endif

        if( GTK_IS_TREE_VIEW( child ) || GTK_IS_TEXT_VIEW( child ) || GTK_IS_ICON_VIEW( child ) )
        {

            registerChild( child );

        } else {

            // list widget types for which scrolled window needs register
            static const char* widgetTypes[] = { "ExoIconView", "FMIconContainer", 0L };
            for( unsigned int i = 0; widgetTypes[i]; i++ )
            {
                if( Gtk::g_object_is_a( G_OBJECT( child ), widgetTypes[i] ) )
                {
                    registerChild( child );
                    break;
                }
            }

        }

    }

    //_____________________________________________
    void ScrolledWindowData::disconnect( GtkWidget* )
    {
        _target = 0;
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); ++iter )
        { iter->second.disconnect( iter->first ); }

        _childrenData.clear();
    }

    //________________________________________________________________________________
    void ScrolledWindowData::setHovered( GtkWidget* widget, bool value )
    {

        bool oldHover( hovered() );
        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter != _childrenData.end() ) iter->second._hovered = value;
        else return;

        // need to schedule repaint of the whole widget
        if( oldHover != hovered() && _target ) gtk_widget_queue_draw( _target );

    }

    //________________________________________________________________________________
    void ScrolledWindowData::setFocused( GtkWidget* widget, bool value )
    {

        bool oldFocus( focused() );
        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter != _childrenData.end() ) iter->second._focused = value;
        else return;

        // need to schedule repaint of the whole widget
        if( oldFocus != focused() && _target ) gtk_widget_queue_draw( _target );

    }

    //_____________________________________________
    void ScrolledWindowData::registerChild( GtkWidget* widget )
    {
        // make sure widget is not already in map
        if( _childrenData.find( widget ) == _childrenData.end() )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::ScrolledWindowData::registerChild -"
                << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
            #endif

            // adjust event mask
            gtk_widget_add_events( widget, GDK_ENTER_NOTIFY_MASK|GDK_LEAVE_NOTIFY_MASK|GDK_FOCUS_CHANGE_MASK );

            // allocate new Hover data
            ChildData data;
            data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
            data._leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );
            data._focusInId.connect( G_OBJECT(widget), "focus-in-event", G_CALLBACK( focusInNotifyEvent ), this );
            data._focusOutId.connect( G_OBJECT(widget), "focus-out-event", G_CALLBACK( focusOutNotifyEvent ), this );

            // and insert in map
            _childrenData.insert( std::make_pair( widget, data ) );

            // set initial focus
            setFocused( widget, gtk_widget_has_focus( widget ) );

            // set initial hover
            const bool enabled( gtk_widget_get_state( widget ) != GTK_STATE_INSENSITIVE );

            // on connection, needs to check whether mouse pointer is in widget or not
            // to have the proper initial value of the hover flag
            if( enabled && gtk_widget_get_window( widget ) )
            {

                gint xPointer,yPointer;
                gdk_window_get_pointer( gtk_widget_get_window( widget ), &xPointer, &yPointer, 0L );
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                const GdkRectangle rect( Gtk::gdk_rectangle( 0, 0, allocation.width, allocation.height ) );
                setHovered( widget, Gtk::gdk_rectangle_contains( &rect, xPointer, yPointer ) );

            } else setHovered( widget, false );

        }

    }

    //________________________________________________________________________________
    void ScrolledWindowData::unregisterChild( GtkWidget* widget )
    {

        // loopup in hover map
        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ScrolledWindowData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        iter->second.disconnect( widget );
        _childrenData.erase( iter );

    }

    //________________________________________________________________________________
    #if OXYGEN_DEBUG
    void ScrolledWindowData::ChildData::disconnect( GtkWidget* widget )
    #else
    void ScrolledWindowData::ChildData::disconnect( GtkWidget* )
    #endif
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ScrolledWindowData::ChildData::disconnect -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        _destroyId.disconnect();
        _enterId.disconnect();
        _leaveId.disconnect();
        _focusInId.disconnect();
        _focusOutId.disconnect();
        _hovered = false;
        _focused = false;

    }

    //____________________________________________________________________________________________
    gboolean ScrolledWindowData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::ScrolledWindowData::childDestroyNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<ScrolledWindowData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::enterNotifyEvent( GtkWidget* widget, GdkEventCrossing* event, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrolledWindowData::enterNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        if( !(event->state & (GDK_BUTTON1_MASK|GDK_BUTTON2_MASK) ) )
        { static_cast<ScrolledWindowData*>( data )->setHovered( widget, true ); }

        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing* event, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrolledWindowData::leaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        if( !(event->state & (GDK_BUTTON1_MASK|GDK_BUTTON2_MASK) ) )
        { static_cast<ScrolledWindowData*>( data )->setHovered( widget, false ); }

        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::focusInNotifyEvent( GtkWidget* widget, GdkEvent*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrolledWindowData::focusInNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ScrolledWindowData*>( data )->setFocused( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::focusOutNotifyEvent( GtkWidget* widget, GdkEvent*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ScrolledWindowData::focusOutNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ScrolledWindowData*>( data )->setFocused( widget, false );
        return FALSE;
    }

}
