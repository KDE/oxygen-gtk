#ifndef oxygenrgba_h
#define oxygenrgba_h
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

#include <climits>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <gdk/gdk.h>

namespace Oxygen
{

    namespace ColorUtils
    {

        //! stores rgba representation of a color
        /*! all values must be between 0 and 1 */
        class Rgba
        {

            private:
            typedef unsigned short color_t;

            public:

            //! constructor
            Rgba( void ):
                _red(0),
                _green(0),
                _blue(0),
                _alpha(USHRT_MAX),
                _mask(0)
            {}

            //! constructor
            Rgba( double r, double g, double b, double a = 1 ):
                _red( (color_t) (r*USHRT_MAX) ),
                _green( (color_t) (g*USHRT_MAX) ),
                _blue( (color_t) (b*USHRT_MAX) ),
                _alpha( (color_t) (a*USHRT_MAX) ),
                _mask( RGBA )
            {}

            //! equal to operator
            /*! TODO: check why alpha channel is not used in the comparison */
            bool operator == (const Rgba& other ) const
            {
                return
                    _mask == other._mask &&
                    _red == other._red &&
                    _green == other._green &&
                    _blue == other._blue;
            }

            // convert to integer
            guint32 toInt( void ) const
            {
                return
                    (guint32( _red >> 8 ) << 24) |
                    (guint32( _green >> 8 ) << 16) |
                    (guint32( _green >> 8 ) << 8) |
                    guint32( _alpha >> 8 );
            }

            // convert to string
            operator std::string ( void ) const
            {
                std::ostringstream out;
                out
                    << "\"#"
                    << std::hex
                    << std::setw( 2 ) << std::setfill( '0' ) << guint32( _red >> 8 )
                    << std::setw( 2 ) << std::setfill( '0' ) << guint32( _green >> 8 )
                    << std::setw( 2 ) << std::setfill( '0' ) << guint32( _blue >> 8 )
                    << "\"";
                return out.str();
            }

            //! make color lighter
            /*! Copied from QColor. Amount must be > 100. */
            Rgba light( int amount ) const;

            //! make color darker
            /*! Copied from QColor. Amount must be > 100. */
            Rgba dark( int amount ) const;

            //!@name access colors
            //@{

            double red( void ) const
            { return double(_red)/USHRT_MAX; }

            double green( void ) const
            { return double(_green)/USHRT_MAX; }

            double blue( void ) const
            { return double(_blue)/USHRT_MAX; }

            double alpha( void ) const
            { return double(_alpha)/USHRT_MAX; }

            //! value (in HSV colorspace)
            double value( void ) const
            { return std::max( red(), std::max( green(), blue() ) ); }

            //@}

            //!@name set colors
            //@{

            Rgba& setRed( double value )
            {
                _red = (color_t)(value*USHRT_MAX);
                _mask |= R;
                return *this;
            }

            Rgba& setGreen( double value )
            {
                _green = (color_t)(value*USHRT_MAX);
                _mask |= G;
                return *this;
            }

            Rgba& setBlue( double value )
            {
                _blue = (color_t)(value*USHRT_MAX);
                _mask |= B;
                return *this;
            }

            Rgba& setAlpha( double value )
            {
                _alpha = (color_t)(value*USHRT_MAX);
                _mask |= A;
                return *this;
            }

            //@}

            //! convert to hsv
            void toHsv( double&, double&, double& ) const;

            //! convert from hsv
            Rgba& fromHsv( double, double, double );

            //! validity
            bool isValid( void ) const
            { return (_mask & RGB ) == RGB; }

            //! utilities
            static Rgba fromKdeOption( std::string );

            //!@name predefined colors
            //@{
            static Rgba black( void ) { return Rgba( 0, 0, 0, 1 ); }
            static Rgba white( void ) { return Rgba( 1, 1, 1, 1 ); }
            static Rgba transparent( const ColorUtils::Rgba& base = black()  )
            { return Rgba( base ).setAlpha(0); }

            //@}

            private:

            enum ColorBit
            {
                R = 1<<0,
                G = 1<<1,
                B = 1<<2,
                A = 1<<3,
                RGB = R|G|B,
                RGBA = RGB|A
            };

            color_t _red;
            color_t _green;
            color_t _blue;
            color_t _alpha;

            unsigned int _mask;

            friend std::ostream& operator << ( std::ostream& out, const Rgba& rgba )
            { return out << ( rgba._red >> 8 ) << "," << ( rgba._green >> 8 ) << "," << ( rgba._blue >> 8 ) << "," << (rgba._alpha >> 8); }

        };

    }
}

#endif
