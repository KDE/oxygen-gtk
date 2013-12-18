/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include "oxygenfontinfo.h"

#include <sstream>
#include <vector>

namespace Oxygen
{

    //_____________________________________________
    std::string FontInfo::toString( bool addQuotes ) const
    {
        std::ostringstream out;

        if( addQuotes ) out << "\"";

        out << family() << " " << weightString() << " ";
        if( _italic ) out << italicString() << " ";
        out << size() ;

        if( addQuotes ) out << "\"";

        return out.str();
    }

    //_____________________________________________
    FontInfo FontInfo::fromKdeOption( std::string value )
    {

        FontInfo out;
        if( value.empty() ) return out;

        // split strings using "," as a separator
        size_t position = 0;
        std::vector<std::string> values;
        while( ( position = value.find( ',' ) ) != std::string::npos )
        {
            values.push_back( value.substr( 0, position ) );
            value = value.substr( position+1 );
        }

        if( !value.empty() ) values.push_back( value );
        for( unsigned int index = 0; index < values.size(); index++ )
        {

            if( index == 0 ) {

                out._family = values[index];
                continue;

            }

            std::istringstream in( values[index] );
            if( index == 1 )
            {

                double size(0);
                if( in >> size ) out._size = size;
                continue;

            } else if( index == 4 ) {

                int weight;
                if( in >> weight )
                {
                    if( weight < Normal ) out._weight = Light;
                    else if( weight < DemiBold ) out._weight = Normal;
                    else if( weight < Bold ) out._weight = DemiBold;
                    else if( weight < Black ) out._weight = Bold;
                    else out._weight = Black;

                }

                continue;

            } else if( index == 5 ) {

                bool italic;
                if( in >> italic ) out._italic = italic;
                continue;

            } else if( index == 8 ) {

                bool fixed;
                if( in >> fixed ) out._fixed = fixed;
                continue;

           }

        }

        return out;

    }

    //_____________________________________________
    std::string FontInfo::weightString( void ) const
    {

        switch( _weight )
        {
            case Light: return "light";
            default:
            case Normal: return "";
            case DemiBold: return "demibold";
            case Bold: return "bold";
            case Black: return "black";
        }

    }

    //_____________________________________________
    std::string FontInfo::italicString( void ) const
    { return _italic ? "Italic" : ""; }

}
