#ifndef oxygenanimationdata_h
#define oxygenanimationdata_h

/*
* oxygenanimationdata.h
* animation modes
* -------------------
*
* SPDX-FileCopyrightText: 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* Largely inspired from Qtcurve style
* SPDX-FileCopyrightText: 2003-2010 Craig Drummond <craig.p.drummond@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenanimationmodes.h"

namespace Oxygen
{

    //! invalid opacity
    static const double OpacityInvalid = -1;

    class AnimationData
    {

        public:

        //! constructor
        explicit AnimationData( double opacity = OpacityInvalid, AnimationMode mode = AnimationNone ):
            _opacity( opacity ),
            _mode( mode )
        {}

        double _opacity;
        AnimationMode _mode;

    };

}

#endif
