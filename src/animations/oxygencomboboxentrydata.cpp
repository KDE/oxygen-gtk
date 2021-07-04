/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygencomboboxentrydata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>
#include <iostream>
#include <cassert>

namespace Oxygen
{

    //________________________________________________________________________________
    void ComboBoxEntryData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ComboBoxEntryData::connect - widget: " << widget << std::endl;
        #endif

        _list = 0L;

        HoverData::disconnect( widget );
        _entry.disconnect();
        _button.disconnect();
    }

    //________________________________________________________________________________
    void ComboBoxEntryData::setButton( GtkWidget* widget )
    {
        if( _button._widget == widget ) return;
        assert( !_button._widget );

        _button._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _button._enterId.connect( G_OBJECT(widget), "enter-notify-event", (GCallback)enterNotifyEvent, this );
        _button._leaveId.connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
        _button._toggledId.connect( G_OBJECT(widget), "toggled", G_CALLBACK( childToggledEvent ), this );
        _button._widget = widget;
    }

    //________________________________________________________________________________
    void ComboBoxEntryData::setEntry( GtkWidget* widget )
    {
        if( _entry._widget == widget ) return;
        assert( !_entry._widget );

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ComboBoxEntryData::setEntry - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        _entry._destroyId.connect( G_OBJECT(widget), "destroy", G_CALLBACK( childDestroyNotifyEvent ), this );
        _entry._enterId.connect( G_OBJECT(widget), "enter-notify-event", (GCallback)enterNotifyEvent, this );
        _entry._leaveId.connect( G_OBJECT(widget), "leave-notify-event", (GCallback)leaveNotifyEvent, this );
        _entry._widget = widget;
    }

    //________________________________________________________________________________
    void ComboBoxEntryData::setPressed( GtkWidget* widget, bool value )
    {
        if( widget == _button._widget ) _button._pressed = value;
        return;
    }

    //________________________________________________________________________________
    bool ComboBoxEntryData::setHovered( GtkWidget* widget, bool value )
    {
        const bool oldHovered( hovered() );
        if( widget == _entry._widget ) _entry._hovered = value;
        else if( widget == _button._widget ) _button._hovered = value;
        else HoverData::setHovered( widget, value );

        if( oldHovered != hovered() )
        {

            // trigger repaint
            if( _button._widget ) gtk_widget_queue_draw( gtk_widget_get_parent( _button._widget ) );
            else if( _entry._widget ) gtk_widget_queue_draw( gtk_widget_get_parent( _entry._widget ) );
            return true;

        } else return false;

    }

    //________________________________________________________________________________
    void ComboBoxEntryData::unregisterChild( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ComboBoxEntryData::unregisterChild - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        if( widget == _button._widget ) _button.disconnect();
        else if( widget == _entry._widget ) _entry.disconnect();
        return;

    }

    //________________________________________________________________________________
    void ComboBoxEntryData::Data::disconnect( void )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ComboBoxEntryData::Data::disconnect - " << _widget << " (" << (_widget ? G_OBJECT_TYPE_NAME( _widget ):"null") << ")" << std::endl;
        #endif

        if( !_widget ) return;
        _destroyId.disconnect();
        _enterId.disconnect();
        _leaveId.disconnect();
        _toggledId.disconnect();
        _hovered = false;
        _focus = false;
        _pressed = false;
        _widget = 0L;
    }

    //____________________________________________________________________________________________
    gboolean ComboBoxEntryData::childDestroyNotifyEvent( GtkWidget* widget, gpointer data )
    {
        static_cast<ComboBoxEntryData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //____________________________________________________________________________________________
    void ComboBoxEntryData::childToggledEvent( GtkWidget* widget, gpointer data)
    {
        if( GTK_IS_TOGGLE_BUTTON( widget ) )
        { static_cast<ComboBoxEntryData*>(data)->setPressed( widget, gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( widget ) ) ); }
        return;
    }
}
