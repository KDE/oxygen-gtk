#ifndef oxygentaboptions_h
#define oxygentaboptions_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "oxygenflags.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    //! internal option flags to pass arguments around
    enum TabOption
    {
        FirstTab = 1<<0,
        LastTab = 1<<1,
        CurrentTab = 1<<2,

        /*
        these flags are used to decide how to connect
        tabs to the main frame
        */
        FirstTabAligned = 1<<3,
        LastTabAligned = 1<<4,
        LeftOfSelected = 1<<5,
        RightOfSelected = 1<<6,

        //! true if tab is being dragged
        Dragged = 1<<7,

        //! true if tabs are drawn for Xul application
        Xul = 1<<8

    };

    class TabOptions: public Flags<TabOption>
    {

        public:

        //! constructor
        TabOptions( void )
        {}

        //! constructor
        TabOptions( GtkWidget*, GtkStateType, GtkPositionType, int x, int y, int w, int h );

        //! destructor
        virtual ~TabOptions( void )
        {}

    };

}

OX_DECLARE_OPERATORS_FOR_FLAGS( Oxygen::TabOptions )

#endif
