#ifndef oxygenpathlist_h
#define oxygenpathlist_h
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

#include <iostream>
#include <string>
#include <vector>

namespace Oxygen
{

    //! utility class to handle path list
    //! path list
    class PathList: public std::vector<std::string>
    {

        public:

        //! empty constructor
        explicit PathList( void )
        {}

        //! splitting constructor
        explicit PathList( const std::string& path, const std::string& separator = ":" )
        { split( path, separator ); }

        //! split string using provided separator and store
        void split( const std::string&, const std::string& = ":" );

        //! concatenate using provided separator
        std::string join( const std::string& = ":" ) const;

        friend std::ostream& operator << ( std::ostream& out, const PathList& pathList )
        {
            for( PathList::const_iterator iter = pathList.begin(); iter != pathList.end(); iter++ )
            { out << "    " << *iter << std::endl; }

            return out;
        }

    };
}

#endif
