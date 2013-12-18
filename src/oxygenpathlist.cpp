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

#include "oxygenpathlist.h"

#include <sstream>

namespace Oxygen
{

    //_________________________________________________________
    void PathList::split( const std::string& path, const std::string& separator )
    {

        clear();
        std::string local( path );
        if( local.empty() ) return;
        if( local[local.size()-1] == '\n' ) local = local.substr( 0, local.size()-1 );

        size_t position( std::string::npos );
        while( ( position = local.find( separator ) ) != std::string::npos )
        {
            push_back( local.substr(0, position ) );
            local = local.substr( position + separator.length() );
        }

        if( !local.empty() ) push_back( local );
        return;

    }

    //_________________________________________________________
    std::string PathList::join( const std::string& separator ) const
    {
        std::ostringstream out;
        for( const_iterator iter = begin(); iter != end(); ++iter )
        {
            if( iter != begin() ) out << separator;
            out << *iter;
        }

        return out.str();
    }

}
