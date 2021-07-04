#ifndef oxygenpathlist_h
#define oxygenpathlist_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* inspired notably from kdelibs/kdeui/color/kcolorutils.h
* SPDX-FileCopyrightText: 2007 Matthew Woehlke <mw_triad@users.sourceforge.net>
* SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
* SPDX-FileCopyrightText: 2007 Zack Rusin <zack@kde.org>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace Oxygen
{

    typedef std::set<std::string> PathSet;

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
