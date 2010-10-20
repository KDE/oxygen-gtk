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

#include "oxygencomboboxentrydata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <iostream>
#include <cassert>

namespace Oxygen
{

    //________________________________________________________________________________
    void ComboBoxEntryData::connect( GtkWidget* widget )
    {
        _enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", (GCallback)enterNotifyEvent, this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
    }

    //________________________________________________________________________________
    void ComboBoxEntryData::disconnect( GtkWidget* widget )
    {

        g_signal_handler_disconnect(G_OBJECT(widget), _enterId );
        g_signal_handler_disconnect(G_OBJECT(widget), _leaveId );
        _entry.disconnect();
        _button.disconnect();

    }

    //________________________________________________________________________________
    void ComboBoxEntryData::setButton( GtkWidget* widget )
    {
        if( _button._widget == widget ) return;
        assert( !_button._widget );

        _button._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _button._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
        _button._enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", (GCallback)enterNotifyEvent, this );
        _button._leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
        _button._widget = widget;
    }

    //________________________________________________________________________________
    void ComboBoxEntryData::setEntry( GtkWidget* widget )
    {
        if( _entry._widget == widget ) return;
        assert( !_entry._widget );
        _entry._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _entry._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
        _entry._enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", (GCallback)enterNotifyEvent, this );
        _entry._leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
        _entry._widget = widget;
    }

    //________________________________________________________________________________
    void ComboBoxEntryData::setHovered( GtkWidget* widget, bool value )
    {
        const bool oldHovered( hovered() );
        if( widget == _entry._widget ) _entry._hovered = value;
        else if( widget == _button._widget ) _button._hovered = value;
        else _hovered = value;

        if( oldHovered != hovered() )
        {
            // trigger repaint
            if( _button._widget ) gtk_widget_queue_draw( gtk_widget_get_parent( _button._widget ) );
            else if( _entry._widget ) gtk_widget_queue_draw( gtk_widget_get_parent( _entry._widget ) );
        }

    }

    //________________________________________________________________________________
    void ComboBoxEntryData::unregisterChild( GtkWidget* widget )
    {

        if( widget == _button._widget ) _button.disconnect();
        else if( widget == _entry._widget ) _entry.disconnect();
        return;

    }

    //________________________________________________________________________________
    void ComboBoxEntryData::Data::disconnect( void )
    {
        if( !_widget ) return;
        g_signal_handler_disconnect(G_OBJECT(_widget), _destroyId );
        g_signal_handler_disconnect(G_OBJECT(_widget), _styleChangeId );
        g_signal_handler_disconnect(G_OBJECT(_widget), _enterId );
        g_signal_handler_disconnect(G_OBJECT(_widget), _leaveId );
        _hovered = false;
        _focus = false;
        _widget = 0L;
    }

    //____________________________________________________________________________________________
    gboolean ComboBoxEntryData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<ComboBoxEntryData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void ComboBoxEntryData::childStyleChangeNotifyEvent( GtkWidget* widget, GtkStyle*, gpointer data )
    { static_cast<ComboBoxEntryData*>(data)->unregisterChild( widget ); }

    //________________________________________________________________________________
    gboolean ComboBoxEntryData::enterNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<ComboBoxEntryData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean ComboBoxEntryData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<ComboBoxEntryData*>( data )->setHovered( widget, false );
        return FALSE;
    }

}
