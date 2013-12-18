#ifndef oxygenshadowconfiguration_h
#define oxygenshadowconfiguration_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
        { return _colorGroup; }

        //! enability
        bool isEnabled() const
        { return _enabled; }

        //! shadow size
        double shadowSize() const
        { return _shadowSize; }

        //! horizontal offset
        double horizontalOffset() const
        { return _horizontalOffset; }

        //! vertical offset
        double verticalOffset() const
        { return _verticalOffset; }

        //! inner color
        ColorUtils::Rgba innerColor() const
        { return _innerColor; }

        //! outer color
        ColorUtils::Rgba outerColor() const
        { return _outerColor; }

        //! initialize from options
        void initialize( const OptionMap& );

        private:

        //! color group
        Palette::Group _colorGroup;

        //! enability
        bool _enabled;

        //! shadow size
        double _shadowSize;

        //! horizontal offset
        double _horizontalOffset;

        //! vertical offset
        double _verticalOffset;

        //! inner color
        ColorUtils::Rgba _innerColor;

        //! outer color
        ColorUtils::Rgba _outerColor;

        //! use outer color
        bool _useOuterColor;

        //! streamer (for debugging)
        friend std::ostream& operator << (std::ostream&, const ShadowConfiguration& );

    };
}

#endif
