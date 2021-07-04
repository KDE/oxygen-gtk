#ifndef oxygenpanedengine_h
#define oxygenpanedengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygenpaneddata.h"

#include <gtk/gtk.h>
#include <set>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to vertical and horizontal paned
    class PanedEngine: public GenericEngine<PanedData>
    {

        public:

        //! constructor
        PanedEngine( Animations* widget ):
            GenericEngine<PanedData>( widget )
            {}

        //! destructor
        virtual ~PanedEngine( void )
        {}

    };

}

#endif
