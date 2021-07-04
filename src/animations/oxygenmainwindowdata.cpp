/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
        _locked = false;
        _configureId.connect( G_OBJECT(widget), "configure-event", G_CALLBACK( configureNotifyEvent ), this);
    }

    //________________________________________________________________________________
    void MainWindowData::disconnect( GtkWidget* widget )
    {
        _target = 0L;

        // reset timeout and locked flag
        _timer.stop();
        _locked = false;

        _configureId.disconnect();
    }

    //________________________________________________________________________________
    void MainWindowData::updateSize( int width, int height )
    {

        if( width == _width && height == _height ) return;
        _width = width;
        _height = height;

        // schedule delayed timeOut
        if( !_timer.isRunning() )
        {

            _timer.start( 50, (GSourceFunc)delayedUpdate, this );
            _locked = false;

        } else _locked = true;

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

            // if target is invalid, reset timeout and return
            data._locked = false;
            return FALSE;

        } else if( data._locked ) {

            // if locked, reset the flag and re-run timer
            data._locked = false;
            return TRUE;

        } else {

            // otherwise, trigger update
            gtk_widget_queue_draw( data._target );
            return FALSE;

        }

    }

}
