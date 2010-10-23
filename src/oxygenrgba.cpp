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

#include "oxygenrgba.h"

#include <cassert>
#include <cmath>

namespace Oxygen
{

    //___________________________________________________________
    ColorUtils::Rgba ColorUtils::Rgba::light( int factor ) const
    {

        if( factor <= 0 ) return *this;
        else if( factor < 100 ) return dark( 10000 / factor );

        double h, s, v;
        toHsv( h, s, v );
        v = (v*factor)/100;
        if( v > 1 )
        {
            // overflow. Adjust saturation
            s -= v - 1;
            if( s < 0 ) s = 0;
            v = 1.0;
        }

        return Rgba( *this ).fromHsv( h, s, v );
    }

    //___________________________________________________________
    ColorUtils::Rgba ColorUtils::Rgba::dark( int factor ) const
    {
        if( factor <= 0 ) return *this;
        else if( factor < 100 ) return light( 10000/ factor );

        double h, s, v;
        toHsv( h, s, v );
        v = (v*100.0)/factor;
        return Rgba( *this ).fromHsv( h, s, v );
    }

    //___________________________________________________________
    ColorUtils::Rgba ColorUtils::Rgba::fromKdeOption( std::string value )
    {

        Rgba out;

        // split strings using "," as a separator
        size_t position = 0;
        std::vector<std::string> values;
        while( ( position = value.find( ',' ) ) != std::string::npos )
        {
            values.push_back( value.substr( 0, position ) );
            value = value.substr( position+1 );
        }

        if( !value.empty() ) values.push_back( value );
        for( unsigned int index = 0; index < 4 && index < values.size(); index++ )
        {

            int colorIndex;
            std::istringstream in( values[index] );
            if( !(in >> colorIndex) ) break;

            if( index == 0 ) out.setRed( double(colorIndex)/255 );
            else if( index == 1 ) out.setGreen( double(colorIndex)/255 );
            else if( index == 2 ) out.setBlue( double(colorIndex)/255 );
            else if( index == 3 ) out.setAlpha( double(colorIndex)/255 );
        }

        return out;

    }

    //___________________________________________________________
    void ColorUtils::Rgba::toHsv( double& hue, double& saturation, double& value ) const
    {

        if( !isValid() ) return;

        const color_t max =  std::max( _red, std::max( _green, _blue ) );
        const color_t min =  std::min( _red, std::min( _green, _blue ) );
        const color_t delta = max-min;
        value = double(max)/USHRT_MAX;

        if( delta <= 0 )
        {
            hue = -1;
            saturation = 0;
            return;

        }

        saturation = double(delta)/max;
        if( _red == max ) hue =  double(_green - _blue )/delta;
        else if( _green == max ) hue = 2.0 + double(_blue-_red)/delta;
        else if( _blue == max ) hue = 4.0 + double(_red-_green)/delta;
        else assert( false );

        hue *= 60.0;
        if( hue < 0 ) hue += 360;
        return;

    }

    //___________________________________________________________
    ColorUtils::Rgba& ColorUtils::Rgba::fromHsv( double hue, double saturation, double value )
    {

        if( hue < 0 )
        {
            setRed( value );
            setGreen( value );
            setBlue( value );
            return *this;
        }

        const double h = hue/60;
        const double c = value*saturation*USHRT_MAX;
        const double x = c*(1 - std::abs((h-2*int(h/2)) - 1 ));

        if( h>=0 && h<1 ) { _red = (color_t) c; _green = (color_t) x; _blue = 0; }
        else if( h>=1 && h<2 ) { _red = (color_t) x; _green = (color_t) c; _blue = 0; }
        else if( h>=2 && h<3 ) { _red = 0; _green = (color_t) c; _blue = (color_t) x; }
        else if( h>=3 && h<4 ) { _red = 0; _green = (color_t) x; _blue = (color_t) c; }
        else if( h>=4 && h<5 ) { _red = (color_t) x; _green = 0; _blue = (color_t) c; }
        else { _red = (color_t) c; _green = 0; _blue = (color_t) x; }

        double m = value*USHRT_MAX - c;
        _red += (color_t) m;
        _green += (color_t) m;
        _blue += (color_t) m;

        _mask |= RGB;

        return *this;
    }

}
