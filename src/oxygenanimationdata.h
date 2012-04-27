#ifndef oxygenanimationdata_h
#define oxygenanimationdata_h

/*
* oxygenanimationdata.h
* animation modes
* -------------------
*
* Copyright (c) 2012 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* Largely inspired from Qtcurve style
* Copyright (C) Craig Drummond, 2003 - 2010 craig.p.drummond@gmail.com
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
