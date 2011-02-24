/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* inspired notably from kdelibs/kdeui/colors/kcolorscheme.h
* Copyright (c) 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
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

#include "oxygencoloreffect.h"
#include "oxygencolorutils.h"
#include "oxygenoptionmap.h"

namespace Oxygen
{

    //__________________________________________________________________________________________
    ColorUtils::Effect::Effect( Palette::Group group, const OptionMap& options ):
        _intensityEffect( IntensityNoEffect ),
        _intensityEffectAmount(0),
        _colorEffect( ColorNoEffect ),
        _colorEffectAmount(0),
        _contrastEffect( ContrastNoEffect ),
        _contrastEffectAmount(0),
        _enabled( false )
    {

        // select section
        std::string section;
        switch( group )
        {
            case Palette::Inactive:
            section = "[ColorEffects:Inactive]";
            break;

            case Palette::Disabled:
            section = "[ColorEffects:Disabled]";
            break;

            default: return;
        }

        const bool disabled( group == Palette::Disabled );

        // intensity settings
        switch( options.getOption( section, "IntensityEffect" ).toInt( disabled ? IntensityDarken : IntensityNoEffect ) )
        {
            case 1: _intensityEffect = IntensityShade; break;
            case 2: _intensityEffect = IntensityDarken; break;
            case 3: _intensityEffect = IntensityLighten; break;
            default: _intensityEffect = IntensityNoEffect; break;

        }

        _intensityEffectAmount = options.getOption( section, "IntensityAmount" ).toVariant<double>( disabled ? 0.1 : 0.0 );

        // contrast effect
        switch( options.getOption( section, "ContrastEffect" ).toInt( disabled ? ContrastFade : ContrastTint ) )
        {
            case 1: _contrastEffect = ContrastFade; break;
            case 2: _contrastEffect = ContrastTint; break;
            default: _contrastEffect = ContrastNoEffect; break;
        }

        _contrastEffectAmount = options.getOption( section, "ContrastAmount" ).toVariant<double>( disabled ? 0.65 : 0.10 );

        // color effect
        switch( options.getOption( section, "ColorEffect" ).toInt( disabled ? ColorNoEffect: ColorFade ) )
        {
            case 1: _colorEffect = ColorDesaturate; break;
            case 2: _colorEffect = ColorFade; break;
            case 3: _colorEffect = ColorTint; break;
            default: _colorEffect = ColorNoEffect; break;
        }

        _colorEffectAmount = options.getOption( section, "ColorAmount" ).toVariant<double>( disabled ? 0:0.025 );

        // color
        _color = Rgba::fromKdeOption( options.getValue( section, "Color" ) );

        if( !_color.isValid() )
        { _color = disabled ? Rgba( 56.0/255, 56.0/255, 56.0/255 ) : Rgba( 112.0/255, 111.0/255, 110.0/255 ); }

        // enable state
        _enabled = ( options.getOption( section, "Enable" ).toVariant<std::string>("true") == "true" );
    }

    //__________________________________________________________________________________________
    ColorUtils::Rgba ColorUtils::Effect::color( const ColorUtils::Rgba& background ) const
    {

        // do nothing if disabled
        if( !_enabled ) return background;

        // apply intensity effect
        Rgba out( background );
        switch( _intensityEffect )
        {
            case IntensityShade:
            out = shade( out, _intensityEffectAmount );
            break;

            case IntensityDarken:
            out = darken( out, _intensityEffectAmount );
            break;

            case IntensityLighten:
            out = lighten( out, _intensityEffectAmount );
            break;

            default: break;
        }

        // apply color effect
        switch( _colorEffect )
        {
            case ColorDesaturate:
            out = darken( out, 0.0, 1.0 - _colorEffectAmount );
            break;

            case ColorFade:
            out = mix( out, _color, _colorEffectAmount );
            break;

            case ColorTint:
            out = tint( out, _color, _colorEffectAmount );
            break;

            default:
            break;
        }

        return out;

    }

    //__________________________________________________________________________________________
    ColorUtils::Rgba ColorUtils::Effect::color( const ColorUtils::Rgba& foreground, const ColorUtils::Rgba& background ) const
    {

        if( !_enabled ) return foreground;
        Rgba out( foreground );
        switch( _contrastEffect )
        {
            case ContrastFade:
            out = mix( out, background, _contrastEffectAmount );
            break;

            case ContrastTint:
            out = tint( out, background, _contrastEffectAmount );
            break;

            default:
            break;

        }

        return out;

    }

    namespace ColorUtils
    {
        //__________________________________________________________________________________________
        std::ostream& operator << (std::ostream& out, const Effect& effect )
        {
            out << "Color=" << effect._color << std::endl;
            out << "ColorAmount=" << effect._colorEffectAmount << std::endl;
            out << "ColorEffect=" << effect._colorEffect << std::endl;
            out << "ContrastAmount=" << effect._contrastEffectAmount << std::endl;
            out << "ContrastEffect=" << effect._contrastEffect << std::endl;
            out << "IntensityAmount=" << effect._intensityEffectAmount << std::endl;
            out << "IntensityEffect=" << effect._intensityEffect << std::endl;
            return out;
        }
    }

}
