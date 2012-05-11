/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygenshadowconfiguration.h"
#include "config.h"

#include <cassert>

namespace Oxygen
{

    //_________________________________________________________
    ShadowConfiguration::ShadowConfiguration( Palette::Group group ):
        _colorGroup(group),
        _enabled(true)
    {
        assert(group==Palette::Active||group==Palette::Inactive);

        if( _colorGroup == Palette::Active )
        {

            _shadowSize = 40;
            _horizontalOffset = 0;
            _verticalOffset = 0.1;

            _innerColor = ColorUtils::Rgba( 0.44, 0.94, 1.0 );
            _outerColor = ColorUtils::Rgba( 0.33, 0.64, 0.94 );
            _useOuterColor = true;

        } else {

            _shadowSize = 40;
            _horizontalOffset = 0;
            _verticalOffset = 0.2;

            _innerColor = ColorUtils::Rgba::black();
            _outerColor = _innerColor;
            _useOuterColor = false;

        }

    }


    //_________________________________________________________
    void ShadowConfiguration::initialize( const OptionMap& options )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ShadowConfiguration::initialize - " << (_colorGroup == Palette::Active ? "Active": "Inactive" ) << std::endl;
        #endif

        if( _colorGroup == Palette::Active)
        {

            _innerColor = ColorUtils::Rgba::fromKdeOption( options.getValue( "[ActiveShadow]", "InnerColor", "112,241,255" ) );
            _outerColor = ColorUtils::Rgba::fromKdeOption( options.getValue( "[ActiveShadow]", "OuterColor", "84,167,240" ) );

            _shadowSize = options.getOption( "[ActiveShadow]","Size" ).toVariant<double>(40);
            _verticalOffset = options.getOption( "[ActiveShadow]","VerticalOffset" ).toVariant<double>(0.1);
            _useOuterColor = options.getOption( "[ActiveShadow]","UseOuterColor" ).toVariant<std::string>("true") == "true";

        } else {

            _innerColor = ColorUtils::Rgba::fromKdeOption( options.getValue( "[InactiveShadow]", "InnerColor", "0,0,0" ) );
            _outerColor = ColorUtils::Rgba::fromKdeOption( options.getValue( "[InactiveShadow]", "OuterColor", "0,0,0" ) );

            _shadowSize = options.getOption( "[InactiveShadow]","Size" ).toVariant<double>(40);
            _verticalOffset = options.getOption( "[InactiveShadow]","VerticalOffset" ).toVariant<double>(0.2);
            _useOuterColor = options.getOption( "[InactiveShadow]", "UseOuterColor" ).toVariant<std::string>("false") == "true";

        }

        if(!_useOuterColor)
            _outerColor=_innerColor;

    }

    //_________________________________________________________
    std::ostream& operator << (std::ostream& out, const ShadowConfiguration& configuration )
    {
        out << "Oxygen::ShadowConfiguration - (" << (configuration._colorGroup == Palette::Active ? "Active": "Inactive" ) << ")" << std::endl;
        out << "  enabled: " << (configuration._enabled ? "true":"false" ) << std::endl;
        out << "  size: " << configuration._shadowSize << std::endl;
        out << "  offset: " << configuration._verticalOffset << std::endl;
        out << "  innerColor: " << configuration._innerColor << std::endl;
        out << "  outerColor: ";
        if( configuration._useOuterColor ) out << "unused";
        else out <<  configuration._outerColor;
        out << std::endl;
        return out;
    }
}
