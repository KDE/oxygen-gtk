/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* MainWindow prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc.
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
*
* the mainwindow data code is largely inspired from the gtk redmond engine
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
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void MainWindowData::connect( GtkWidget* widget )
    { _configureId = g_signal_connect( G_OBJECT(widget), "configure-event", (GCallback)configureNotifyEvent, this); }

    //________________________________________________________________________________
    void MainWindowData::disconnect( GtkWidget* widget )
    { g_signal_handler_disconnect(G_OBJECT(widget), _configureId ); }

    //________________________________________________________________________________
    void MainWindowData::updateSize( GtkWidget* widget, int width, int height )
    {

        if( width == _width && height == _height ) return;
        _width = width;
        _height = height;

        // std::cout << "Oxygen::MainWindowData::updateSize - (" << _width << "," << _height << ")" << std::endl;

        // trigger update

    }

    //________________________________________________________________________________
    gboolean MainWindowData::configureNotifyEvent(GtkWidget* widget, GdkEventConfigure* event, gpointer data )
    {

        static_cast<MainWindowData*>(data)->updateSize( widget, event->width, event->height );
        return FALSE;
    }

}
