#ifndef oxygenscrolledwindowengine_h
#define oxygenscrolledwindowengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenscrolledwindowdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable scrolled windows
    class ScrolledWindowEngine: public GenericEngine<ScrolledWindowData>
    {

        public:

        //! constructor
        ScrolledWindowEngine( Animations* widget ):
            GenericEngine<ScrolledWindowData>( widget )
            {}

        //! destructor
        virtual ~ScrolledWindowEngine( void )
        {}

        //! true if scrolled window is hovered
        bool hovered( GtkWidget* widget )
        { return data().value( widget ).hovered(); }

        //! true if scrolled window is focused
        bool focused( GtkWidget* widget )
        { return data().value( widget ).focused(); }

        //! register child
        void registerChild( GtkWidget* widget, GtkWidget* child )
        { data().value( widget ).registerChild( child ); }


    };

}

#endif
