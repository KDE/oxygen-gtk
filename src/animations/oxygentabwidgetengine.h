#ifndef oxygentabwidgetengine_h
#define oxygentabwidgetengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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

        //!@name modifiers
        //@{

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

        //! toggle dirty state
        void toggleDirty( GtkWidget* widget )
        { data().value( widget ).toggleDirty(); }

        //! true when tabbar is dirty
        void setDirty( GtkWidget* widget, bool value )
        { data().value( widget ).setDirty( value ); }

        //@}

        //!@name accessors
        //@{

        //! returns hovered tab, if any
        int hoveredTab( GtkWidget* widget )
        { return data().value( widget ).hoveredTab(); }

        //! returns true if provided point is in one tab of the widget
        bool isInTab( GtkWidget* widget, int x, int y )
        { return data().value( widget ).isInTab( x, y ); }

        //! true when drag is in progress
        bool dragInProgress( GtkWidget* widget )
        { return data().value( widget ).dragInProgress(); }

        //! true when tabbar is dirty
        bool isDirty( GtkWidget* widget )
        { return data().value( widget ).isDirty(); }

        //@}

    };

}

#endif
