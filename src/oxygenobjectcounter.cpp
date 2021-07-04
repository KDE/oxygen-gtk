/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

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
