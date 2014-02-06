/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenwidgetsizedata.h"
#include "../oxygengtkutils.h"

namespace Oxygen
{

    //___________________________________________________________________________________________________________
    WidgetSizeData::ChangedFlags WidgetSizeData::update()
    {

        ChangedFlags changed;

        // get size and check
        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( _target ) );
        const int& width( allocation.width );
        const int& height( allocation.height );
        if( width != _width || height != _height )
        {
            _width = width;
            _height = height;
            changed |= SizeChanged;
        }

        const bool alpha( Gtk::gtk_widget_has_rgba( _target ) );
        if( alpha != _alpha )
        {
            _alpha = alpha;
            changed |= AlphaChanged;
        }

        return changed;
    }

}
