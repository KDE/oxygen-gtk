#ifndef oxygentabwidgetengine_h
#define oxygentabwidgetengine_h
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
#include "oxygentabwidgetdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable tabwidgetes
    /*!
    ensures that the text entry and the button of editable tabwidgetes
    gets hovered and focus flags at the same time
    */
    class TabWidgetEngine: public GenericEngine<TabWidgetData>
    {

        public:

        //! constructor
        TabWidgetEngine( Animations* widget ):
            GenericEngine<TabWidgetData>( widget )
            {}

        //! destructor
        virtual ~TabWidgetEngine( void )
        {}

        //! returns hovered tab, if any
        int hoveredTab( GtkWidget* widget )
        { return data().value( widget ).hoveredTab(); }

        //! returns hovered tab, if any
        void updateTabRect( GtkWidget* widget, int index, int x, int y, int w, int h )
        {
            GdkRectangle local = { x, y, w, h };
            return data().value( widget ).updateTabRect( widget, index, local );
        }

        //! returns hovered tab, if any
        void updateTabRect( GtkWidget* widget, int index, const GdkRectangle& r )
        { return data().value( widget ).updateTabRect( widget, index, r ); }

        //! returns hovered tab, if any
        void updateHoveredTab( GtkWidget* widget )
        { return data().value( widget ).updateHoveredTab( widget ); }

        //! true when drag is in progress
        void setDragInProgress( GtkWidget* widget, bool value )
        { data().value( widget ).setDragInProgress( value ); }

        //! true when drag is in progress
        bool dragInProgress( GtkWidget* widget )
        { return data().value( widget ).dragInProgress(); }

    };

}

#endif
