#ifndef oxygengeometry_h
#define oxygengeometry_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
