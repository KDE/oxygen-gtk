#ifndef oxygencomboboxengine_h
#define oxygencomboboxengine_h
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
#include "oxygencomboboxdata.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboboxes
    /*!
    ensures that the text  and the button of editable comboboxes
    gets hovered and focus flags at the same time
    */
    class ComboBoxEngine: public GenericEngine<ComboBoxData>
    {

        public:

        //! constructor
        ComboBoxEngine( Animations* widget ):
            GenericEngine<ComboBoxData>( widget )
            {}

        //! destructor
        virtual ~ComboBoxEngine( void )
        {}

        //!@name modifiers
        //@{

        //! assign button to data matching widget
        void setButton( GtkWidget* widget, GtkWidget* value )
        { data().value( widget ).setButton( value ); }

        //! button focus
        void setButtonFocus( GtkWidget* widget, bool value )
        { data().value( widget ).setButtonFocus( value ); }

        //! register child
        void registerChild( GtkWidget* widget, GtkWidget* child )
        { data().value( widget ).registerChild( child ); }

        //! initialize cell layout
        void initializeCellLayout( GtkWidget* widget )
        { data().value( widget ).initializeCellLayout(); }

        //@}

        //! true if either button or is pressed
        bool pressed( GtkWidget* widget )
        { return data().value( widget ).pressed(); }

        //! true if either button or entry has focus
        bool hasFocus( GtkWidget* widget )
        { return data().value( widget ).hasFocus(); }

        //! true if comboBox is hovered
        bool hovered( GtkWidget* widget )
        { return data().value( widget ).hovered(); }

        //! return GtkWidget* of currently popped up combobox
        GtkWidget* getPoppedUpWidget()
        { return poppedUpWidget; }

        //! save GtkWidget* of currently popped up combobox
        void setPoppedUpWidget(GtkWidget* widget)
        { poppedUpWidget=widget; }

        private:

        GtkWidget* poppedUpWidget;

    };

}

#endif
