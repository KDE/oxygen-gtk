#ifndef oxygenshadowconfiguration_h
#define oxygenshadowconfiguration_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* inspired notably from kdelibs/kdeui/color/kcolorutils.h
* Copyright (C) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
* Copyright (C) 2007 Thomas Zander <zander@kde.org>
* Copyright (C) 2007 Zack Rusin <zack@kde.org>
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

#include "oxygenoptionmap.h"
#include "oxygenpalette.h"

namespace Oxygen
{
    //! shadow configuration (not including colors since they reside in _palette)
    class ShadowConfiguration
    {
        public:
            //! default constructor
            ShadowConfiguration( Palette::Group );

            //! destructor
            virtual ~ShadowConfiguration()
            {}

            Palette::Group colorGroup() const
            { return colorGroup_; }

            //! enability
            bool isEnabled() const
            { return enabled_; }

            //! shadow size
            double shadowSize()
            { return shadowSize_; }

            //! horizontal offset
            double horizontalOffset()
            { return horizontalOffset_; }

            //! vertical offset
            double verticalOffset()
            { return verticalOffset_; }

            //! inner color
            ColorUtils::Rgba innerColor()
            { return innerColor_; }

            //! outer color
            ColorUtils::Rgba outerColor()
            { return outerColor_; }

            //! initialize from options
            void initialize( const OptionMap& );

            private:

            //! color group
            Palette::Group colorGroup_;

            //! enability
            bool enabled_;

            //! shadow size
            double shadowSize_;

            //! horizontal offset
            double horizontalOffset_;

            //! vertical offset
            double verticalOffset_;

            //! inner color
            ColorUtils::Rgba innerColor_;

            //! outer color
            ColorUtils::Rgba outerColor_;

            //! use outer color
            bool useOuterColor_;
    };
}

#endif
