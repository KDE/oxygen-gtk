#ifndef oxygengeometry_h
#define oxygengeometry_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

}

#endif
