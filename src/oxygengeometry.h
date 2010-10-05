#ifndef oxygengeometry_h
#define oxygengeometry_h
/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include <vector>

namespace Oxygen
{

    class Point
    {
        public:

        //! constructor
        explicit Point( double x, double y):
            _x( x ),
            _y( y )
        {}


        //! destructor
        virtual ~Point( void )
        {}

        //! accessors
        //@{
        double x( void ) const { return _x; }
        double y( void ) const { return _y; }
        //@}

        //! modifiers
        void setX( double x ) { _x = x; }
        void setY( double y ) { _y = y; }

        private:

        double _x;
        double _y;

    };

    //! polygons
    class Polygon: public std::vector<Point>
    {

        public:

        Polygon& operator << (const Point& point )
        {
            push_back( point );
            return *this;
        }


    };

    //! rectangle
    class Rectangle
    {


        public:

        //! empty constructor
        Rectangle( void ):
            _x(0),
            _y(0),
            _width(-1),
            _height(-1)
        {}

        //! constructor
        Rectangle( double x, double y, double width, double height ):
            _x( x ),
            _y( y ),
            _width( width ),
            _height( height )
        {}

        //! validity
        bool isValid( void )
        { return _width >= 0 && _height >= 0; }

        //! empty
        bool isEmpty( void )
        { return _width == 0 || _height == 0; }

        //!@name accessors
        //@{
        void setX( double value ) { _x = value; }
        void setY( double value ) { _y = value; }
        void setWidth( double value ) { _width = value; }
        void setHeight( double value ) { _height = value; }
        //@}

        //!@name modifiers
        //@{
        double x( void ) const { return _x; }
        double y( void ) const { return _y; }
        double width( void ) const { return _width; }
        double height( void ) const { return _height; }
        //@}

        //!@name corners
        //@{
        Point topLeft( void ) const
        { return Point( _x, _y ); }

        Point bottomLeft( void ) const
        { return Point( _x, _y + _height ); }

        Point topRight( void ) const
        { return Point( _x + _width, _y ); }

        Point bottomRight( void ) const
        { return Point( _x + _width, _y + _height ); }
        //@}

        //! true if rectangle intersects argument
        bool intersects( const Rectangle& other ) const
        {
            return
                ( !( ( _x+_width < other._x ) || (_x > other._x + other._width ) ) ) &&
                ( !( ( _y+_height < other._y ) || (_y > other._y + other._height ) ) );
        }

        //! return intersection between two rectangles
        Rectangle intersected( const Rectangle& other ) const
        {

            double xMin = std::max( _x, other._x );
            double xMax = std::min( _x+_width, other._x + other._width );
            double yMin = std::max( _y, other._y );
            double yMax = std::min( _y+_height, other._y + other._height );
            return Rectangle( xMin, yMin, xMax-xMin, yMax-yMin );
        }

        private:

        double _x;
        double _y;
        double _width;
        double _height;

    };

}

#endif


