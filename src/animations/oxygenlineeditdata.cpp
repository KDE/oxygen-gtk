/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* the lineedit data code is largely inspired from the gtk redmond engine
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

#include "oxygenlineeditdata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void LineEditData::connect( GtkWidget* widget )
    {
        _enterId = g_signal_connect( G_OBJECT(widget), "enter-notify-event", G_CALLBACK( enterNotifyEvent ), this );
        _leaveId = g_signal_connect( G_OBJECT(widget), "leave-notify-event", G_CALLBACK( leaveNotifyEvent ), this );
    }

    //________________________________________________________________________________
    void LineEditData::disconnect( GtkWidget* widget )
    {
        g_signal_handler_disconnect( G_OBJECT(widget), _enterId );
        g_signal_handler_disconnect( G_OBJECT(widget), _leaveId );
    }

    //________________________________________________________________________________
    gboolean LineEditData::enterNotifyEvent(GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<LineEditData*>( data )->setHovered( widget, true );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean LineEditData::leaveNotifyEvent( GtkWidget* widget, GdkEventCrossing*, gpointer data )
    {
        static_cast<LineEditData*>( data )->setHovered( widget, false );
        return FALSE;
    }

}
