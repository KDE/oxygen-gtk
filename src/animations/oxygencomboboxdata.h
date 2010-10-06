#ifndef oxygencomboboxdata_h
#define oxygencomboboxdata_h
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

#include <gtk/gtkwidget.h>
#include <gtk/gtk.h>

namespace Oxygen
{
    class ComboBoxData
    {

        public:

        //! constructor
        ComboBoxData( void )
        {}

        //! destructor
        virtual ~ComboBoxData( void )
        {}

        //!@name modifiers
        //@{
        void setButton( GtkWidget* value ) { _button._widget = value; }
        void setEntry( GtkWidget* value ) { _entry._widget = value; }

        //! button focus
        void setButtonFocus( bool value )
        {
            if( _button._focus == value ) return;
            _button._focus = value;

            // trigger entry update
            if( _entry._widget ) gtk_widget_queue_draw( _entry._widget );

            return;
        }

        //! entry focus
        void setEntryFocus( bool value )
        {
            if( _entry._focus == value ) return;
            _entry._focus = value;

            // trigger button update
            if( _button._widget ) gtk_widget_queue_draw( _button._widget );

        }

        //@}

        //! true if either button or entry has focus
        bool hasFocus( void ) const
        { return _button._focus || _entry._focus; }

        private:

        class Data
        {

            public:

            //! constructor
            Data( void ):
                _widget( 0L ),
                _focus( false )
            {}


            GtkWidget* _widget;
            bool _focus;

        };

        Data _button;
        Data _entry;


    };

}

#endif
