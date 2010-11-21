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

#include "oxygenscrolledwindowdata.h"
#include "../oxygengtkutils.h"

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

        // check child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );

        if( GTK_IS_TREE_VIEW( child ) )
        {

            registerChild( child );

            // also find header widgets
            GList* children( gtk_container_get_children( GTK_CONTAINER( child ) ) );
            for( GList* child = g_list_first(children); child; child = g_list_next(child) )
            {

                std::cout << "Oxygen::ScrolledWindowData::connect - child: " << G_OBJECT_TYPE_NAME( widget ) << std::endl;
                if( GTK_IS_BUTTON( child->data ) )
                { registerChild( GTK_WIDGET( child->data ) ); }
            }

            if( children ) g_list_free( children );

        } else {

            static const char* widgetTypes[] = { "ExoIconView", "GeditView", "FMIconContainer", 0L };
            for( unsigned int i = 0; widgetTypes[i]; i++ )
            {
                if( Gtk::gtk_object_is_a( G_OBJECT( child ), widgetTypes[i] ) )
                {
                    registerChild( child );
                    break;
                }
            }

        }

    }

    //_____________________________________________
    void ScrolledWindowData::disconnect( GtkWidget* widget )
    {
        _target = 0;
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); iter++ )
        { iter->second.disconnect(); }

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
            std::cout
                << "Oxygen::ScrolledWindowData::registerChild -"
                << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
            #endif

            // allocate new Hover data
            ChildData data;
            data._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._styleChangeId.connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
            data._enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
            data._leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );
            data._focusInId.connect( G_OBJECT(widget), "focus-in-event", G_CALLBACK( focusInNotifyEvent ), this );
            data._focusOutId.connect( G_OBJECT(widget), "focus-out-event", G_CALLBACK( focusOutNotifyEvent ), this );

            if( GTK_IS_CONTAINER( widget ) )
            { data._addId.connect( G_OBJECT(widget), "add", G_CALLBACK( childAddedEvent ), this ); }

            // and insert in map
            _childrenData.insert( std::make_pair( widget, data ) );

            // set initial focus
            setFocused( widget, gtk_widget_has_focus( widget ) );

            // set initial hover
            const bool enabled( gtk_widget_get_state( widget ) != GTK_STATE_INSENSITIVE );

            // on connection, needs to check whether mouse pointer is in widget or not
            // to have the proper initial value of the hover flag
            if( enabled )
            {

                gint xPointer,yPointer;
                gdk_window_get_pointer(widget->window,&xPointer,&yPointer, 0L);
                GdkRectangle rect = { 0, 0, widget->allocation.width, widget->allocation.height };
                setHovered( widget, Gtk::gdk_rectangle_contains( &rect, xPointer, yPointer ) );
            }

        }

    }

    //________________________________________________________________________________
    void ScrolledWindowData::unregisterChild( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cout
            << "Oxygen::ScrolledWindowData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        // loopup in hover map
        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;

        iter->second.disconnect();
        _childrenData.erase( iter );

    }

    //________________________________________________________________________________
    void ScrolledWindowData::ChildData::disconnect( void )
    {

        _destroyId.disconnect();
        _styleChangeId.disconnect();
        _enterId.disconnect();
        _leaveId.disconnect();
        _focusInId.disconnect();
        _focusOutId.disconnect();
        _addId.disconnect();
        _hovered = false;

    }

    //____________________________________________________________________________________________
    gboolean ScrolledWindowData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cout
            << "Oxygen::ScrolledWindowData::childDestroyNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<ScrolledWindowData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void ScrolledWindowData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<ScrolledWindowData*>(data)->unregisterChild( widget ); }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::enterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ScrolledWindowData::enterNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ScrolledWindowData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ScrolledWindowData::leaveNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ScrolledWindowData*>( data )->setHovered( widget, false );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ScrolledWindowData::focusInNotifyEvent( GtkWidget* widget, GdkEvent*, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ScrolledWindowData::focusInNotifyEvent -"
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
        std::cout << "Oxygen::ScrolledWindowData::focusOutNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        static_cast<ScrolledWindowData*>( data )->setFocused( widget, false );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void ScrolledWindowData::childAddedEvent( GtkContainer* parent, GtkWidget* widget, gpointer data )
    {
        std::cout << "ScrolledWindowData::childAddedEvent - " << G_OBJECT_TYPE_NAME( widget ) << std::endl;
        return;
    }

}
