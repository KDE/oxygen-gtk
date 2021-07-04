/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
