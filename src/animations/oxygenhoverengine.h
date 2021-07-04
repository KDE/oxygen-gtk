#ifndef oxygenhoverengine_h
#define oxygenhoverengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygenhoverdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboboxes
    /*!
    ensures that the text entry and the button of editable comboboxes
    gets hovered and focus flags at the same time
    */
    class HoverEngine: public GenericEngine<HoverData>
    {

        public:

        //! constructor
        HoverEngine( Animations* parent ):
            GenericEngine<HoverData>( parent )
            {}

        //! destructor
        virtual ~HoverEngine( void )
        {}

        //! register widget
        virtual bool registerWidget( GtkWidget* widget )
        { return registerWidget( widget, false ); }

        //! register widget
        virtual bool registerWidget( GtkWidget* widget, bool updateOnHover )
        {
            if( GenericEngine<HoverData>::registerWidget( widget ) )
            {
                data().value(widget).setUpdateOnHover( updateOnHover );
                return true;
            } else return false;

        }

        //! true if widget is hovered
        bool hovered( GtkWidget* widget )
        { return data().value( widget ).hovered(); }

        //! set hover state
        bool setHovered( GtkWidget* widget, bool value )
        { return data().value( widget ).setHovered( widget, value ); }

    };

}

#endif
