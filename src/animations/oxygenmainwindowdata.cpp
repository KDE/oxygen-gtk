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

#include "oxygenmainwindowdata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //________________________________________________________________________________
    void MainWindowData::connect( GtkWidget* widget )
    {
        _target = widget;
        _timeOutCount = 0;
        _configureId = g_signal_connect( G_OBJECT(widget), "configure-event", (GCallback)configureNotifyEvent, this);
    }

    //________________________________________________________________________________
    void MainWindowData::disconnect( GtkWidget* widget )
    {
        _target = 0L;
        _timeOutCount = 0;
        g_signal_handler_disconnect(G_OBJECT(widget), _configureId );
    }

    //________________________________________________________________________________
    void MainWindowData::updateSize( int width, int height )
    {

        if( width == _width && height == _height ) return;
        _width = width;
        _height = height;

        // schedule delayed timeOut
        ++_timeOutCount;
        g_timeout_add( 50, (GSourceFunc)delayedUpdate, this );

    }

    //________________________________________________________________________________
    gboolean MainWindowData::configureNotifyEvent(GtkWidget* widget, GdkEventConfigure* event, gpointer data )
    {
        static_cast<MainWindowData*>(data)->updateSize( event->width, event->height );
        return FALSE;
    }

    //________________________________________________________________________________
    gboolean MainWindowData::delayedUpdate( gpointer pointer )
    {

        MainWindowData& data( *static_cast<MainWindowData*>(pointer) );
        if( !data._target )
        {
            // if target is invalid, reset timeOut counts and return
            data._timeOutCount = 0;

        } else if( data._timeOutCount > 0 ) {

            // decrement time out, and schedule update if it reaches 0
            --data._timeOutCount;
            if( !data._timeOutCount )
            { gtk_widget_queue_draw( data._target ); }

        }

        return FALSE;
    }

}
