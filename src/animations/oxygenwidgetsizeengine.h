#ifndef oxygenwidgetsizeengine_h
#define oxygenwidgetsizeengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

        //! update window size for given widget
        bool updateSize( GtkWidget* widget, int width, int height )
        { return data().value( widget ).updateSize( width, height ); }

    };

}

#endif
