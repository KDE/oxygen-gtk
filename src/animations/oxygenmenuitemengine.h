#ifndef oxygenmenuitemengine_h
#define oxygenmenuitemengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenmenuitemdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable menuitemes
    /*!
    ensures that the text entry and the button of editable menuitemes
    gets hovered and focus flags at the same time
    */
    class MenuItemEngine: public GenericEngine<MenuItemData>
    {

        public:

        //! constructor
        MenuItemEngine( Animations* widget ):
            GenericEngine<MenuItemData>( widget )
            {}

        //! destructor
        virtual ~MenuItemEngine( void )
        {}

        //! register all menuItems children of a menu
        virtual bool registerMenu( GtkWidget* );

    };

}

#endif
