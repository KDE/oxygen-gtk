/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    the hover data code is largely inspired from the gtk redmond engine

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenhoverdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"

#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void HoverData::connect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::HoverData::connect - " << widget << " (" << (widget ? G_OBJECT_TYPE_NAME( widget ):"null") << ")" << std::endl;
        #endif

        const bool enabled( gtk_widget_get_state( widget ) != GTK_STATE_INSENSITIVE );

        // on connection, needs to check whether mouse pointer is in widget or not
        // to have the proper initial value of the hover flag
        if( enabled )
        {

            gint xPointer,yPointer;
            gdk_window_get_pointer( gtk_widget_get_window( widget ), &xPointer, &yPointer, 0L);
            const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
            const GdkRectangle rect( Gtk::gdk_rectangle( 0, 0, allocation.width, allocation.height ) );
            setHovered( widget, Gtk::gdk_rectangle_contains( &rect, xPointer, yPointer ) );

        } else {

            setHovered( widget, false );

        }

        // register callbacks
        _enterId.connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
        _leaveId.connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );
    }

    //________________________________________________________________________________
    void HoverData::disconnect( GtkWidget* widget )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::HoverData::disconnect - " << widget << " (" << (widget ? G_OBJECT_TYPE_NAME( widget ):"null") << ")" << std::endl;
        #endif

        _enterId.disconnect();
        _leaveId.disconnect();
    }

    //________________________________________________________________________________
    gboolean HoverData::enterNotifyEvent(GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr << "HoverData::enterNotifyEvent - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        static_cast<HoverData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean HoverData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::HoverData::leaveNotifyEvent - " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")" << std::endl;
        #endif

        static_cast<HoverData*>( data )->setHovered( widget, false );
        return FALSE;
    }

}
