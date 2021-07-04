#ifndef oxygengroupboxlabelengine_h
#define oxygengroupboxlabelengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygengroupboxlabeldata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable scrolled windows
    class GroupBoxLabelEngine: public GenericEngine<GroupBoxLabelData>
    {

        public:

        //! constructor
        GroupBoxLabelEngine( Animations* widget ):
            GenericEngine<GroupBoxLabelData>( widget )
            {}

        //! destructor
        virtual ~GroupBoxLabelEngine( void )
        {}

        // adjust widget size
        void adjustSize( GtkWidget* widget )
        { return data().value( widget ).adjustSize( widget ); }

    };

}

#endif
