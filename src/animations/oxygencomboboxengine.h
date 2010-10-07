#ifndef oxygencomboboxengine_h
#define oxygencomboboxengine_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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


#include "oxygenbaseengine.h"
#include "oxygendatamap.h"
#include "oxygencomboboxdata.h"

#include <gtk/gtkwidget.h>

namespace Oxygen
{
    //! forward declaration
    class Animations;

    //! stores data associated to editable comboboxes
    /*!
    ensures that the text entry and the button of editable comboboxes
    gets hovered and focus flags at the same time
    */
    class ComboBoxEngine: public BaseEngine
    {

        public:

        //! constructor
        ComboBoxEngine( Animations* widget ):
            BaseEngine( widget )
            {}

        //! destructor
        virtual ~ComboBoxEngine( void )
        {}

        //! register widget
        void registerWidget( GtkWidget* widget )
        {
            if( _data.contains( widget ) ) return;
            _data.registerWidget( widget );
            BaseEngine::registerWidget( widget );
        }

        //! unregister widget
        void unregisterWidget( GtkWidget* widget )
        { _data.erase( widget ); }

        //! true if widget is included
        bool contains( GtkWidget* widget )
        { return _data.contains( widget ); }

        //!@name modifiers
        //@{

        //! assign button to data matching widget
        void setButton( GtkWidget* widget, GtkWidget* value )
        { _data.value( widget ).setButton( value ); }

        //! assign entry to data matching widget
        void setEntry( GtkWidget* widget, GtkWidget* value )
        { _data.value( widget ).setEntry( value ); }

        //! button focus
        void setButtonFocus( GtkWidget* widget, bool value )
        { _data.value( widget ).setButtonFocus( value ); }

        //! entry focus
        void setEntryFocus( GtkWidget* widget, bool value )
        { _data.value( widget ).setEntryFocus( value ); }

        //@}

        //! true if either button or entry has focus
        bool hasFocus( GtkWidget* widget )
        { return _data.value( widget ).hasFocus(); }

        //! update mouseoverstate
        void updateMouseOver( GtkWidget* widget )
        { _data.value( widget ).updateMouseOver( widget ); }

        //! true if comboBox is hovered
        bool hovered( GtkWidget* widget )
        { return _data.value( widget ).hovered(); }

        private:

        //! map widgets to ComboBoxData
        DataMap<ComboBoxData> _data;

    };

}

#endif
