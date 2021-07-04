#ifndef oxygentreeviewengine_h
#define oxygentreeviewengine_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygengenericengine.h"
#include "oxygendatamap.h"
#include "oxygentreeviewdata.h"

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
    class TreeViewEngine: public GenericEngine<TreeViewData>
    {

        public:

        //! constructor
        TreeViewEngine( Animations* );

        //! destructor
        virtual ~TreeViewEngine( void );

        //! register widget
        virtual bool registerWidget( GtkWidget* );

        //! true if widget is hovered
        bool hovered( GtkWidget* widget )
        { return data().value( widget ).hovered(); }

        //! true if treeview data is dirty
        bool isDirty( GtkWidget* widget )
        { return data().value( widget ).isDirty(); }

        //! update hovered cell for given widget
        void updateHoveredCell( GtkWidget* widget )
        { return data().value( widget ).updateHoveredCell(); }

        //! true if given cell is hovered
        bool isCellHovered( GtkWidget* widget, const Gtk::CellInfo& info )
        { return data().value( widget ).isCellHovered( info ); }

        bool isCellHovered( GtkWidget* widget, const Gtk::CellInfo& info, bool fullWidth )
        { return data().value( widget ).isCellHovered( info, fullWidth ); }

        private:

        //! cursor
        bool _cursorLoaded;
        GdkCursor* _cursor;
    };

}

#endif
