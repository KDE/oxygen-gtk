#ifndef oxygencomboboxdata_h
#define oxygencomboboxdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright(c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright(C ) 2008 Robert Staudinger
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

#include <gtk/gtkwidget.h>

namespace Oxygen
{
    class ComboBoxData
    {

        public:

        //! constructor
        ComboBoxData( GtkWidget* button = 0L, GtkWidget* entry = 0L ):
            _button( button ),
            _entry( entry ),
            _buttonFocus( false ),
            _entryFocus( false )
        {}

        //!@name accessors
        //@{

        GtkWidget* button( void ) const { return _button; }
        GtkWidget* entry( void ) const { return _entry; }
        bool buttonFocus( void ) const { return _buttonFocus; }
        bool entryFocus( void ) const { return _entryFocus; }

        //@}

        //!@name modifiers
        //@{
        void setButton( GtkWidget* value ) { _button = value; }
        void setEntry( GtkWidget* value ) { _entry = value; }

        bool setButtonFocus( bool value )
        {
            if( _buttonFocus == value ) return false;
            _buttonFocus = value;
            return true;
        }

        bool setEntryFocus( bool value )
        {
            if( _entryFocus == value ) return false;
            _entryFocus = value;
            return true;
        }

        //@}

        //! true if either button or entry has focus
        bool hasFocus( void ) const
        { return _buttonFocus || _entryFocus; }

        private:

        GtkWidget* _button;
        GtkWidget* _entry;
        bool _buttonFocus;
        bool _entryFocus;


    };

}

#endif
