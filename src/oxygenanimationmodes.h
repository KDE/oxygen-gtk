#ifndef oxygenanimationmodes_h
#define oxygenanimationmodes_h

/*
* oxygenanimationmodes.h
* animation modes
* -------------------
*
* SPDX-FileCopyrightText: 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* Largely inspired from Qtcurve style
* SPDX-FileCopyrightText: 2003 - 2010 Craig Drummond <craig.p.drummond@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenflags.h"

namespace Oxygen
{

    //! animation type
    /*! used for menubars, toolbars and menus */
    enum AnimationType
    {
        NoAnimation,
        Fade,
        FollowMouse
    };

    //! animated widget type
    /*!
    used for engines that keep track of previously(fade-out) and
    currently (fade-in) animated objects
    */
    enum WidgetType
    {
        AnimationCurrent,
        AnimationPrevious
    };

    //! animation mode
    enum AnimationMode
    {
        AnimationNone = 0,
        AnimationHover = 1<<0,
        AnimationFocus = 1<<1,
    };

    OX_DECLARE_FLAGS( AnimationModes, AnimationMode )
    OX_DECLARE_OPERATORS_FOR_FLAGS( AnimationModes )

}

#endif
