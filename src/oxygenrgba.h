#ifndef oxygenrgba_h
#define oxygenrgba_h
/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <gdk/gdktypes.h>

namespace ColorUtils
{

    //! stores rgba representation of a color
    /*! all values must be between 0 and 1 */
    class Rgba
    {

        public:

        typedef std::vector<Rgba> List;

        //! constructor
        Rgba( void ):
            _red(0),
            _green(0),
            _blue(0),
            _alpha(1),
            _mask(0)
        {}

        //! constructor
        Rgba( double r, double g, double b, double a = 1 ):
            _red( r ),
            _green( g ),
            _blue( b ),
            _alpha( a ),
            _mask( RGBA )
        {}

        // convert to integer
        guint32 toInt( void ) const
        {
            return
                (guint32( _red*255 ) << 24) |
                (guint32( _green*255 ) << 16) |
                (guint32( _green*255 ) << 8) |
                guint32( _alpha*255 );
        }

        // convert to string
        operator std::string ( void ) const
        {
            std::ostringstream out;
            out
                << "\"#"
                << std::hex
                << std::setw( 2 ) << std::setfill( '0' ) << guint32( _red*255 )
                << std::setw( 2 ) << std::setfill( '0' ) << guint32( _green*255 )
                << std::setw( 2 ) << std::setfill( '0' ) << guint32( _blue*255 )
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
        { return _red; }

        double green( void ) const
        { return _green; }

        double blue( void ) const
        { return _blue; }

        double alpha( void ) const
        { return _alpha; }

        //! value (in HSV colorspace)
        double value( void ) const
        { return std::max( _red, std::max( _green, _blue ) ); }

        //@}

        //!@name set colors
        //@{

        Rgba& setRed( double value )
        {
            _red = value;
            _mask |= R;
            return *this;
        }

        Rgba& setGreen( double value )
        {
            _green = value;
            _mask |= G;
            return *this;
        }

        Rgba& setBlue( double value )
        {
            _blue = value;
            _mask |= B;
            return *this;
        }

        Rgba& setAlpha( double value )
        {
            _alpha = value;
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
        { return Rgba( base._red, base._green, base._blue, 0 ); }

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

        double _red;
        double _green;
        double _blue;
        double _alpha;

        unsigned int _mask;

        friend std::ostream& operator << ( std::ostream& out, const Rgba& rgba )
        { return out << int(rgba.red()*255) << "," << int(rgba.green()*255) << "," << int(rgba.blue()*255) << "," << int(rgba.alpha()*255); }
    };

}

#endif
