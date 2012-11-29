#ifndef oxygenhoverengine_h
#define oxygenhoverengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
*
* This library is distributed  in the hope that it will be useful,
* but  WITHOUT ANY WARRANTY; without even  the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License  along  with  this library;  if not,  write to  the Free
* Software Foundation, Inc., 51  Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
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
