#ifndef oxygeninnershadowengine_h
#define oxygeninnershadowengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygeninnershadowdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable scrolled windows
    class InnerShadowEngine: public GenericEngine<InnerShadowData>
    {

        public:

        //! constructor
        InnerShadowEngine( Animations* widget ):
            GenericEngine<InnerShadowData>( widget )
            {}

        //! destructor
        virtual ~InnerShadowEngine( void )
        {}

        //! register child
        void registerChild( GtkWidget* widget, GtkWidget* child )
        { data().value( widget ).registerChild( child ); }

    };

}

#endif
