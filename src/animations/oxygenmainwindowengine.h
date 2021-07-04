#ifndef oxygenmainwindowengine_h
#define oxygenmainwindowengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenmainwindowdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable mainwindowes
    /*!
    ensures that the text entry and the button of editable mainwindowes
    gets hovered and focus flags at the same time
    */
    class MainWindowEngine: public GenericEngine<MainWindowData>
    {

        public:

        //! constructor
        MainWindowEngine( Animations* widget ):
            GenericEngine<MainWindowData>( widget )
            {}

        //! destructor
        virtual ~MainWindowEngine( void )
        {}

    };

}

#endif
