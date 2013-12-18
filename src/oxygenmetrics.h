#ifndef oxygenmetrics_h
#define oxygenmetrics_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

namespace Oxygen
{

    //! metrics
    /*! these are copied from the old KStyle WidgetProperties */
    enum Metrics
    {

        /*
        checkbox. Do not change, unless
        changing the actual cached pixmap size
        */
        CheckBox_Size = 21,

        // slider groove height
        Slider_GrooveWidth = 7,

        // menu vertical offset
        Menu_VerticalOffset = 1,

        /*
        entries size margins. Do not change, unless
        changing the corresponding oxygen-entry-margins style
        in gtkrc
        */
        Entry_SideMargin = 3

    };
}

#endif
