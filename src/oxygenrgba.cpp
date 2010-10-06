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

namespace ColorUtils
{

    //___________________________________________________________
    Rgba Rgba::light( int factor ) const
    {
        if( factor <= 100 ) return *this;
        double h, s, v;
        toHsv( h, s, v );
        v = std::min( 1.0, (v*factor)/100 );
        return Rgba( *this ).fromHsv( h, s, v );
    }

    //___________________________________________________________
    Rgba Rgba::dark( int factor ) const
    {
        if( factor <= 100 ) return *this;
        double h, s, v;
        toHsv( h, s, v );
        v = (v*100.0)/factor;
        return Rgba( *this ).fromHsv( h, s, v );
    }

    //___________________________________________________________
    Rgba Rgba::fromKdeOption( std::string value )
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
    void Rgba::toHsv( double& hue, double& saturation, double& value ) const
    {

        if( !isValid() ) return;

        const double max =  std::max( _red, std::max( _green, _blue ) );
        const double min =  std::min( _red, std::min( _green, _blue ) );
        const double delta = max-min;
        value = max;

        if( delta <= 0 )
        {
            hue = -1;
            saturation = 0;
            return;

        }

        saturation = delta/max;
        if( _red == max ) hue =  (_green - _blue )/delta;
        else if( _green == max ) hue = 2.0 + (_blue-_red)/delta;
        else if( _blue == max ) hue = 4.0 + (_red-_green)/delta;
        else assert( false );

        hue *= 60.0;
        if( hue < 0 ) hue += 360;
        return;

    }

    //___________________________________________________________
    Rgba& Rgba::fromHsv( double hue, double saturation, double value )
    {

        if( hue < 0 )
        {
            setRed( value );
            setGreen( value );
            setBlue( value );
            return *this;
        }

        const double c = value*saturation;
        const double h = hue/60;
        const double x = c*(1 - std::abs((h-2*int(h/2)) - 1 ));
        if( h>=0 && h<1 ) { _red = c; _green = x; _blue = 0; }
        else if( h>=1 && h<2 ) { _red = x; _green = c; _blue = 0; }
        else if( h>=2 && h<3 ) { _red = 0; _green = c; _blue = x; }
        else if( h>=3 && h<4 ) { _red = 0; _green = x; _blue = c; }
        else if( h>=4 && h<5 ) { _red = x; _green = 0; _blue = c; }
        else { _red = c; _green = 0; _blue = x; }

        double m = value - c;
        setRed( _red + m );
        setGreen( _green + m );
        setBlue( _blue + m );

        return *this;
    }

}
