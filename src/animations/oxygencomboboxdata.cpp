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

#include "oxygencomboboxdata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <iostream>
#include <cassert>

namespace Oxygen
{

    //________________________________________________________________________________
    void ComboBoxData::connect( GtkWidget* widget )
    {
        _target = widget;
        _enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );
        adjustCell( widget );
    }

    //________________________________________________________________________________
    void ComboBoxData::disconnect( GtkWidget* widget )
    {

        _target = 0L;
        g_signal_handler_disconnect(G_OBJECT(widget), _enterId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
        _button.disconnect();

        // disconnect all children
        for( HoverDataMap::iterator iter = _hoverData.begin(); iter != _hoverData.end(); ++iter )
        { unregisterChild( iter->first ); }

    }

    //________________________________________________________________________________
    void ComboBoxData::setButton( GtkWidget* widget )
    {
        if( _button._widget == widget ) return;
        assert( !_button._widget );

        _button._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _button._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
        _button._toggledId = g_signal_connect( G_OBJECT(widget), "toggled", G_CALLBACK( childToggledEvent ), this );
        _button._widget = widget;

        registerChild( widget, false );

    }


    //________________________________________________________________________________
    void ComboBoxData::adjustCell( GtkWidget* widget )
    {

        GList* children( gtk_container_get_children( GTK_CONTAINER( widget ) ) );
        for( GList* child = g_list_first(children); child; child = g_list_next(child) )
        {
            if( GTK_IS_CELL_VIEW( child->data ) ) {
                gtk_cell_view_set_background_color( GTK_CELL_VIEW( child->data ), 0L );
                break;
            }
        }

        if( children ) g_list_free( children );
        return;

    }

    //________________________________________________________________________________
    void ComboBoxData::setPressed( GtkWidget* widget, bool value )
    {
        const bool oldPressed( pressed() );
        if( widget == _button._widget ) _button._pressed = value;
        else return;

        if( oldPressed != pressed() && _target ) gtk_widget_queue_draw( _target );

    }

    //________________________________________________________________________________
    void ComboBoxData::setHovered( GtkWidget* widget, bool value )
    {

        bool oldHover( hovered() );
        HoverDataMap::iterator iter( _hoverData.find( widget ) );
        if( iter != _hoverData.end() ) iter->second._hovered = value;
        else _hovered = value;

        // need to schedule repaint of the whole widget
        if( oldHover != hovered() && _target ) gtk_widget_queue_draw( _target );

    }

    //________________________________________________________________________________
    void ComboBoxData::registerChild( GtkWidget* widget, bool recursive )
    {

        // make sure widget is not already in map
        if( _hoverData.find( widget ) == _hoverData.end() )
        {

            #if OXYGEN_DEBUG
            std::cout
                << "Oxygen::ComboBoxData::registerChild -"
                << " " << widget << " " << G_OBJECT_TYPE_NAME( widget )
                << std::endl;
            #endif

            // allocate new Hover data
            HoverData data;
            data._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
            data._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
            data._enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
            data._leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );

            // and insert in map
            _hoverData.insert( std::make_pair( widget, data ) );

        }

        /*
        also insert widget's children, recursively.
        that should take care of buttons in tabs and other fancy stuff that applications mght do
        */
        if( recursive && GTK_IS_CONTAINER( widget ) )
        {

            GList *children( gtk_container_get_children( GTK_CONTAINER(widget) ) );
            for( GList* child = g_list_first(children); child; child = g_list_next(child) )
            { registerChild( GTK_WIDGET( child->data ) ); }

            if( children ) g_list_free( children );
        }

    }

    //________________________________________________________________________________
    void ComboBoxData::unregisterChild( GtkWidget* widget )
    {

        HoverDataMap::iterator iter( _hoverData.find( widget ) );
        if( iter == _hoverData.end() ) return;

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::ComboBoxData::unregisterChild - " << widget << std::endl;
        #endif

        iter->second.disconnect( widget );
        _hoverData.erase( iter );

    }

    //________________________________________________________________________________
    void ComboBoxData::ChildData::disconnect( GtkWidget* widget )
    {
        g_signal_handler_disconnect(G_OBJECT(widget), _destroyId );
        g_signal_handler_disconnect(G_OBJECT(widget), _styleChangeId );
        _destroyId = -1;
        _styleChangeId = -1;
    }

    //________________________________________________________________________________
    void ComboBoxData::ButtonData::disconnect( void )
    {
        if( !_widget ) return;
        if( _toggledId >= 0 ) g_signal_handler_disconnect(G_OBJECT(_widget), _toggledId );
        _toggledId = -1;
        _pressed = false;
        _focus = false;

        // base class
        ChildData::disconnect( _widget );
    }

    //________________________________________________________________________________
    void ComboBoxData::HoverData::disconnect( GtkWidget* widget )
    {
        g_signal_handler_disconnect(G_OBJECT(widget), _enterId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
        _enterId = -1;
        _leaveId = -1;
        _hovered = false;

        // base class
        ChildData::disconnect( widget );

    }

    //____________________________________________________________________________________________
    gboolean ComboBoxData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<ComboBoxData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void ComboBoxData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<ComboBoxData*>(data)->unregisterChild( widget ); }

    //____________________________________________________________________________________________
    void ComboBoxData::childToggledEvent( GtkWidget* widget, gpointer data)
    {
        if( GTK_IS_TOGGLE_BUTTON( widget ) )
        { static_cast<ComboBoxData*>(data)->setPressed( widget, gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) ); }
        return;
    }

    //________________________________________________________________________________
    gboolean ComboBoxData::enterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<ComboBoxData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ComboBoxData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<ComboBoxData*>( data )->setHovered( widget, false );
        return FALSE;
    }


}
