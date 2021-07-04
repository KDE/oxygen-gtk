#ifndef oxygenmetrics_h
#define oxygenmetrics_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
