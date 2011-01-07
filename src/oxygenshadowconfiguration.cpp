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

#include <cassert>

namespace Oxygen
{

    //_________________________________________________________
    ShadowConfiguration::ShadowConfiguration( Palette::Group group ):
        colorGroup_(group),
        enabled_(true)
    {
        assert(group==Palette::Active||group==Palette::Inactive);

        if( colorGroup_ == Palette::Active )
        {

            shadowSize_ = 40;
            horizontalOffset_ = 0;
            verticalOffset_ = 0.1;

            innerColor_ = ColorUtils::Rgba( 0.44, 0.94, 1.0 );
            outerColor_ = ColorUtils::Rgba( 0.33, 0.64, 0.94 );
            useOuterColor_ = true;

        } else {

            shadowSize_ = 40;
            horizontalOffset_ = 0;
            verticalOffset_ = 0.2;

            innerColor_ = ColorUtils::Rgba::black();
            outerColor_ = innerColor_;
            useOuterColor_ = false;

        }

    }


    //_________________________________________________________
    void ShadowConfiguration::initialize( const OptionMap& options )
    {

        if( colorGroup_ == Palette::Active)
        {

            innerColor_ = ColorUtils::Rgba::fromKdeOption( options.getValue( "[ActiveShadow]", "InnerColor", "112,241,255" ) );
            outerColor_ = ColorUtils::Rgba::fromKdeOption( options.getValue( "[ActiveShadow]", "OuterColor", "84,167,240" ) );

            shadowSize_ = options.getOption( "[ActiveShadow]","Size" ).toVariant<double>(40);
            verticalOffset_ = options.getOption( "[ActiveShadow]","VerticalOffset" ).toVariant<double>(0.1);
            useOuterColor_ = options.getOption( "[ActiveShadow]","UseOuterColor" ).toVariant<std::string>("true") == "true";

        } else {

            innerColor_ = ColorUtils::Rgba::fromKdeOption( options.getValue( "[InactiveShadow]", "InnerColor", "0,0,0" ) );
            outerColor_ = ColorUtils::Rgba::fromKdeOption( options.getValue( "[InactiveShadow]", "OuterColor", "0,0,0" ) );

            shadowSize_ = options.getOption( "[InactiveShadow]","Size" ).toVariant<double>(40);
            verticalOffset_ = options.getOption( "[InactiveShadow]","VerticalOffset" ).toVariant<double>(0.2);
            useOuterColor_ = options.getOption( "[InactiveShadow]", "UseOuterColor" ).toVariant<std::string>("false") == "true";

        }

    }

}
