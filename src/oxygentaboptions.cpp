/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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

#include "oxygentaboptions.h"
#include "oxygenstyleoptions.h"
#include "oxygengtkutils.h"

#include <cmath>

namespace Oxygen
{

    //______________________________________________________________________
    TabOptions::TabOptions( GtkWidget* widget, GtkStateType state, GtkPositionType position, int x, int y, int w, int h )
    {

        // strange: all tabs but the current one are painted with the active flag
        if( state != GTK_STATE_ACTIVE ) (*this) |= CurrentTab;

        // get allocated size
        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
        int borderWidth( GTK_IS_CONTAINER( widget ) ? gtk_container_get_border_width( GTK_CONTAINER( widget ) ):0 );


        // this simple comparison seems robust enough and much simpler
        // than any other implementation
        switch( position )
        {
            default:
            case GTK_POS_TOP:
            case GTK_POS_BOTTOM:
            if( x == allocation.x + borderWidth ) (*this) |= FirstTabAligned;
            if( x+w == allocation.x + allocation.width - borderWidth ) (*this) |= LastTabAligned;
            break;

            case GTK_POS_LEFT:
            case GTK_POS_RIGHT:
            if( y == allocation.y + borderWidth ) (*this) |= FirstTabAligned;
            if( y+h == allocation.y + allocation.height - borderWidth ) (*this) |= LastTabAligned;
            break;
        }

    }

}


