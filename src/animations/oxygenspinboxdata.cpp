/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
*
* the spinbox data code is largely inspired from the gtk redmond engine
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

#include "oxygenspinboxdata.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{

    //________________________________________________________________________________
    void SpinBoxData::updateMouseOver( GtkWidget* widget )
    {

        int xPointer,yPointer;
        gdk_window_get_pointer(widget->window,&xPointer,&yPointer,NULL);
        GdkRectangle rect = { 0, 0, widget->allocation.width, widget->allocation.height };
        bool hovered( Gtk::gdk_rectangle_contains( &rect, xPointer, yPointer ) );

//         std::cout
//             << "Oxygen::SpinBoxData::updateMouseOver -"
//             << " (" << xPointer << "," << yPointer << ")"
//             << " " << widget->allocation
//             << " hovered: " << hovered
//             << std::endl;

        if( hovered == _hovered ) return;

        // update flag and trigger repaint
        _hovered = hovered;
        gtk_widget_queue_draw( widget );

        return;
    }

}
