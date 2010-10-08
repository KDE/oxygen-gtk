/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
*
* the combobox data code is largely inspired from the gtk redmond engine
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
    void ComboBoxData::disconnect( GtkWidget* )
    {
        // disconnect signal
        if( _entry._widget ) g_signal_handler_disconnect(G_OBJECT(_entry._widget), _entry._leaveId );
        if( _button._widget ) g_signal_handler_disconnect(G_OBJECT(_button._widget), _button._leaveId );
    }

    //________________________________________________________________________________
    void ComboBoxData::setButton( GtkWidget* value )
    {
        if( _button._widget == value ) return;
        assert( !_button._widget );
        _button._widget = value;
        _button._leaveId = g_signal_connect( G_OBJECT(value), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
    }

    //________________________________________________________________________________
    void ComboBoxData::setEntry( GtkWidget* value )
    {
        if( _entry._widget == value ) return;
        assert( !_entry._widget );
        _entry._widget = value;
        _entry._leaveId = g_signal_connect( G_OBJECT(value), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
    }

    //________________________________________________________________________________
    void ComboBoxData::updateMouseOver( GtkWidget* widget )
    {

        int xPointer,yPointer;
        gdk_window_get_pointer(widget->window,&xPointer,&yPointer,NULL);
        bool hovered( Gtk::gdk_rectangle_contains( &widget->allocation, xPointer, yPointer ) );
        if( hovered == _hovered ) return;

        // update flag and trigger repaint
        _hovered = hovered;
        gtk_widget_queue_draw( widget );

        return;
    }

    //________________________________________________________________________________
    gboolean ComboBoxData::leaveNotifyEvent( GtkWidget *widget, GdkEventCrossing *event, gpointer )
    {
        // simply triggers repaint
        gtk_widget_queue_draw( widget );
        return FALSE;
    }

}
