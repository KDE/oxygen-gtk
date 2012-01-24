#ifndef Counter_h
#define Counter_h

// $Id: Counter.h,v 1.13 2009/09/09 02:29:34 hpereira Exp $

/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
* terms of the GNU General Public License as published by the Free Software
* Foundation; either version 2 of the License, or (at your option) any later
* version.
*
* This software is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along with
* software; if not, write to the Free Software Foundation, Inc., 59 Temple
* Place, Suite 330, Boston, MA  02111-1307 USA
*
*
*******************************************************************************/

#include <string>
#include <map>


namespace Oxygen
{

    class ObjectCounter
    {

        public:

        //! counter name and counts pair
        typedef std::pair<std::string, int> Pair;

        //! constructor
        ObjectCounter( const std::string& name );

        //! constructor
        ObjectCounter( const ObjectCounter& counter );

        //! destructor
        virtual ~ObjectCounter( void );

        //! retrieves counter count
        int count( void ) const;

        private:

        //! pointer to integer counter
        int* count_;

    };

}

#endif
