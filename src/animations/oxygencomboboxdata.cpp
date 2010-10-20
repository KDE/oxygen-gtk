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
    {}

    //________________________________________________________________________________
    void ComboBoxData::disconnect( GtkWidget* widget )
    {
        _entry.disconnect();
        _button.disconnect();
    }

    //________________________________________________________________________________
    void ComboBoxData::setButton( GtkWidget* widget )
    {
        if( _button._widget == widget ) return;
        assert( !_button._widget );

        _button._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _button._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
        _button._buttonPressId = g_signal_connect( G_OBJECT(widget), "button-press-event", (GCallback)childButtonPressEvent, this );
        _button._buttonReleaseId = g_signal_connect( G_OBJECT(widget), "button-release-event", (GCallback)childButtonReleaseEvent, this );
        _button._widget = widget;
    }

    //________________________________________________________________________________
    void ComboBoxData::setEntry( GtkWidget* widget )
    {
        if( _entry._widget == widget ) return;
        assert( !_entry._widget );
        _entry._destroyId = g_signal_connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _entry._styleChangeId = g_signal_connect( G_OBJECT(widget), "style-set", G_CALLBACK( childStyleChangeNotifyEvent ), this );
        _entry._buttonPressId = g_signal_connect( G_OBJECT(widget), "button-press-event", (GCallback)childButtonPressEvent, this );
        _entry._buttonReleaseId = g_signal_connect( G_OBJECT(widget), "button-release-event", (GCallback)childButtonReleaseEvent, this );
        _entry._widget = widget;
    }

    //________________________________________________________________________________
    void ComboBoxData::setPressed( GtkWidget* widget, bool value )
    {
        const bool oldPressed( pressed() );
        if( widget == _entry._widget ) _entry._pressed = value;
        else if( widget == _button._widget ) _button._pressed = value;
        else return;

        if( oldPressed != pressed() )
        {
            // trigger repaint
            if( _button._widget ) gtk_widget_queue_draw( gtk_widget_get_parent( _button._widget ) );
            else if( _entry._widget ) gtk_widget_queue_draw( gtk_widget_get_parent( _entry._widget ) );
        }

    }

    //________________________________________________________________________________
    void ComboBoxData::unregisterChild( GtkWidget* widget )
    {

        if( widget == _button._widget ) _button.disconnect();
        else if( widget == _entry._widget ) _entry.disconnect();
        return;

    }

    //________________________________________________________________________________
    void ComboBoxData::Data::disconnect( void )
    {
        if( !_widget ) return;
        g_signal_handler_disconnect(G_OBJECT(_widget), _destroyId );
        g_signal_handler_disconnect(G_OBJECT(_widget), _styleChangeId );
        g_signal_handler_disconnect(G_OBJECT(_widget), _buttonPressId );
        g_signal_handler_disconnect(G_OBJECT(_widget), _buttonReleaseId );
        _widget = 0L;
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
    gboolean ComboBoxData::childButtonPressEvent( GtkWidget* widget, GdkEventButton* event, gpointer data)
    {
        if( event && event->button == Gtk::LeftMouseButton )
        { static_cast<ComboBoxData*>(data)->setPressed( widget, true ); }
        return FALSE;
    }

    //____________________________________________________________________________________________
    gboolean ComboBoxData::childButtonReleaseEvent( GtkWidget* widget, GdkEventButton* event, gpointer data)
    {
        if( event && event->button == Gtk::LeftMouseButton )
        { static_cast<ComboBoxData*>(data)->setPressed( widget, false ); }
        return FALSE;
    }

}
