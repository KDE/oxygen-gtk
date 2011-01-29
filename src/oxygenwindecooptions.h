#ifndef oxygenwindecooptions_h
#define oxygenwindecooptions_h

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
