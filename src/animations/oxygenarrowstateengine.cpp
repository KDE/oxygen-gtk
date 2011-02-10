/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenarrowstateengine.h"

namespace Oxygen
{

    //________________________________________________________________________
    void ArrowStateEngine::setEnabled( bool value )
    {

        if( enabled() == value ) return;
        BaseEngine::setEnabled( value );

        // focus data map
        for( DataMap<ArrowStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
        {
            iter->second.timeLine( GTK_ARROW_UP ).setEnabled( value );
            iter->second.timeLine( GTK_ARROW_DOWN ).setEnabled( value );
            if( enabled() && !widgetIsBlackListed( iter->first ) ) iter->second.connect( iter->first );
            else iter->second.disconnect( iter->first );
        }

    }

    //________________________________________________________________________
    void ArrowStateEngine::setDuration( int value )
    {

        if( _duration == value ) return;
        _duration = value;

        // focus data map
        for( DataMap<ArrowStateData>::Map::iterator iter = data().map().begin(); iter != data().map().end(); iter++ )
        {
            iter->second.timeLine( GTK_ARROW_UP ).setDuration( value );
            iter->second.timeLine( GTK_ARROW_DOWN ).setDuration( value );
        }

    }

}


