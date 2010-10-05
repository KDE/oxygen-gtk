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

namespace Oxygen
{
    class ComboBoxData
    {

        public:

        //! constructor
        ComboBoxData( bool buttonFocus = false, bool entryFocus = false ):
            _buttonFocus( buttonFocus ),
            _entryFocus( entryFocus )
        {}

        //!@name modifiers
        //@{
        void setButtonFocus( bool value ) { _buttonFocus = value; }
        void setEntryFocus( bool value ) { _entryFocus = value; }
        //@}

        //! true if either button or entry has focus
        bool hasFocus( void ) const
        { return _buttonFocus || _entryFocus; }

        private:

        bool _buttonFocus;
        bool _entryFocus;


    };

}

#endif
