/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2012 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenobjectcountermap.h"

namespace Oxygen
{

    //___________________________________________________
    ObjectCounterMap& ObjectCounterMap::get( void )
    {
        static ObjectCounterMap singleton;
        return singleton;
    }

}
