// $Id: ObjectCounter.cpp,v 1.9 2011/11/10 21:35:25 hpereira Exp $

/******************************************************************************
*
* Copyright (C) 2002 Hugo PEREIRA <mailto: hugo.pereira@free.fr>
*
* This is free software; you can redistribute it and/or modify it under the
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

#include "oxygenobjectcounter.h"
#include "oxygenobjectcountermap.h"

namespace Oxygen
{
    //____________________________________________________________
    ObjectCounter::ObjectCounter( const std::string& name ):
        count_( 0 )
    {
        count_ = ObjectCounterMap::get().counter( name );
        ObjectCounterMap::get().increment( *count_ );
        return;
    }

    //____________________________________________________________
    ObjectCounter::ObjectCounter( const ObjectCounter& counter ):
        count_( counter.count_ )
    {
        ObjectCounterMap::get().increment( *count_ );
        return;
    }

    //____________________________________________________________
    ObjectCounter::~ObjectCounter( void )
    { if( count_ ) ObjectCounterMap::get().decrement( *count_ ); }

    //____________________________________________________________
    int ObjectCounter::count( void ) const
    { return (count_) ? *count_:0; }

}
