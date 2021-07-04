#ifndef oxygenwindecooptions_h
#define oxygenwindecooptions_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenwindecobutton.h"

namespace Oxygen
{

    //! window decoration options
    namespace WinDeco
    {

        //! window decoration options
        enum Option
        {
            Maximized=1<<0,
            Shaded=1<<2,
            Resizable=1<<3,
            Active=1<<4,
            Alpha=1<<5,
            DrawAlphaChannel=1<<6,
        };

        //! window decoration options
        typedef unsigned long Options;

        enum Metric
        {
            BorderLeft=0,
            BorderRight,
            BorderBottom,
            // BorderTop includes title and resize handle heights
            BorderTop,
            ButtonSpacing,
            ButtonMarginTop,
            ButtonMarginBottom,
            ShadowLeft,
            ShadowTop,
            ShadowRight,
            ShadowBottom,
            MetricsCount
        };

        //! get frame dimensions
        int getMetric(Metric wm);

        //! get button size
        // all buttons are of the same size in oxygen, so no argument in internal function
        int getButtonSize();

    }

}

#endif
