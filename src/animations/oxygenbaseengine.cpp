/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenbaseengine.h"
#include "oxygenanimations.h"

namespace Oxygen
{
    //______________________________________________________
    bool BaseEngine::registerWidget( GtkWidget* widget )
    { return parent().registerWidget( widget ); }

}
