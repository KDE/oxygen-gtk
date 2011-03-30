/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenstyleoptions.h"

#include <vector>
#include <string>

namespace Oxygen
{

    //______________________________________________________________
    std::ostream& operator << (std::ostream& out, const StyleOptions& options )
    {
        std::vector<std::string> flags;
        if( options & Blend ) flags.push_back( "Blend" );
        if( options & Sunken ) flags.push_back( "Sunken" );
        if( options & Flat ) flags.push_back( "Flat" );
        if( options & Focus ) flags.push_back( "Focus" );
        if( options & Hover ) flags.push_back( "Hover" );
        if( options & NoFill ) flags.push_back( "NoFill" );
        if( options & Vertical ) flags.push_back( "Vertical" );
        if( options & Alpha ) flags.push_back( "Alpha" );
        if( options & Round ) flags.push_back( "Round" );
        if( options & Contrast ) flags.push_back( "Contrast" );
        if( options & Selected ) flags.push_back( "Selected" );
        if( options & Disabled ) flags.push_back( "Disabled" );
        if( options & Menu ) flags.push_back( "Menu" );

        if( flags.empty() ) out << "None";
        else {

            for( std::vector<std::string>::const_iterator iter = flags.begin(); iter != flags.end(); ++iter )
            {
                if( iter == flags.begin() ) out << *iter;
                else out << "|" << *iter;
            }

        }

        return out;

    }

}
