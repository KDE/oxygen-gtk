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
#include "oxygenstyle.h"

namespace Oxygen
{

    //_________________________________________________________
    ShadowConfiguration::ShadowConfiguration( Palette::Group group ):
        colorGroup_(group),
        enabled_(true)
    {
        assert(group==Palette::Active||group==Palette::Inactive);

        // load values not present in oxygenrc
        if( group==Palette::Active )
        {
            horizontalOffset_=0;
        } else
        {
            horizontalOffset_=0;
        }

        // now load config file
        OptionMap& oxygen=Style::instance().settings().oxygen();

        if(group==Palette::Active)
        {

            innerColor_=ColorUtils::Rgba::fromKdeOption( oxygen.getValue( "[ActiveShadow]", "InnerColor", "112,241,255" ) );
            outerColor_=ColorUtils::Rgba::fromKdeOption( oxygen.getValue( "[ActiveShadow]", "OuterColor", "84,167,240" ) );

            shadowSize_=oxygen.getOption( "[ActiveShadow]","Size" ).toVariant<double>(40);
            verticalOffset_=oxygen.getOption( "[ActiveShadow]","VerticalOffset" ).toVariant<double>(0.1);
            useOuterColor_=oxygen.getOption( "[ActiveShadow]","UseOuterColor" ).toVariant<std::string>("true") == "true";

        } else
        {
            innerColor_=ColorUtils::Rgba::fromKdeOption( oxygen.getValue( "[InactiveShadow]", "InnerColor", "0,0,0" ) );
            outerColor_=ColorUtils::Rgba::fromKdeOption( oxygen.getValue( "[InactiveShadow]", "OuterColor", "0,0,0" ) );

            shadowSize_=oxygen.getOption( "[InactiveShadow]","Size" ).toVariant<double>(40);
            verticalOffset_=oxygen.getOption( "[InactiveShadow]","VerticalOffset" ).toVariant<double>(0.2);
            useOuterColor_=oxygen.getOption( "[InactiveShadow]", "UseOuterColor" ).toVariant<std::string>("false") == "true";
        }

    }

}
