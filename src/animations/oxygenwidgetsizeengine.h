#ifndef oxygenwidgetsizeengine_h
#define oxygenwidgetsizeengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/


#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenwidgetsizedata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable widgetsizees
    /*!
    ensures that the text entry and the button of editable widgetsizees
    gets hovered and focus flags at the same time
    */
    class WidgetSizeEngine: public GenericEngine<WidgetSizeData>
    {

        public:

        //! constructor
        WidgetSizeEngine( Animations* widget ):
            GenericEngine<WidgetSizeData>( widget )
            {}

        //! destructor
        virtual ~WidgetSizeEngine( void )
        {}

        //! update window XShape for given widget
        WidgetSizeData::ChangedFlags update( GtkWidget* widget )
        { return data().value( widget ).update(); }

        //! width
        int width( GtkWidget* widget )
        { return  data().value( widget ).width(); }

        //! height
        int height( GtkWidget* widget )
        { return data().value( widget ).height(); }

        //! alpha
        bool alpha( GtkWidget* widget )
        { return data().value( widget ).alpha(); }

    };

}

#endif
