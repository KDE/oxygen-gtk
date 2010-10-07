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
        ComboBoxEngine( Animations* parent ):
            BaseEngine( parent )
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

        //!@name modifiers
        //@{

        //! assign button to data matching parent
        void setButton( GtkWidget* parent, GtkWidget* value )
        { _data.value( parent ).setButton( value ); }

        //! assign entry to data matching parent
        void setEntry( GtkWidget* parent, GtkWidget* value )
        { _data.value( parent ).setEntry( value ); }

        //! button focus
        void setButtonFocus( GtkWidget* parent, bool value )
        { _data.value( parent ).setButtonFocus( value ); }

        //! entry focus
        void setEntryFocus( GtkWidget* parent, bool value )
        { _data.value( parent ).setEntryFocus( value ); }

        //@}

        //! true if either button or entry has focus
        bool hasFocus( GtkWidget* parent ) const
        { return _data.value( parent ).hasFocus(); }


        private:

        //! map widgets to ComboBoxData
        DataMap<ComboBoxData> _data;

    };

}

#endif
