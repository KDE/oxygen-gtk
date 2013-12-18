#ifndef oxygencomboboxentrydata_h
#define oxygencomboboxentrydata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygenhoverdata.h"
#include "oxygensignal.h"

#include <gtk/gtk.h>

namespace Oxygen
{

    // handles focus and hover for editable comboboxes
    class ComboBoxEntryData: public HoverData
    {

        public:

        //! constructor
        ComboBoxEntryData( void ):
            _list( 0L )
        {}

        //! destructor
        virtual ~ComboBoxEntryData( void )
        { disconnect( _list ); }

        //! disconnect
        virtual void disconnect( GtkWidget* );

        //!@name modifiers
        //@{

        //! list
        void setList( GtkWidget* widget )
        { _list = widget; }

        //! assign button
        void setButton( GtkWidget* value );

        //! assign entry
        void setEntry( GtkWidget* value );

        //! button focus
        void setButtonFocus( bool value )
        {
            if( _button._focus == value ) return;
            _button._focus = value;

            // trigger entry update
            if( _entry._widget )
            { gtk_widget_queue_draw( _entry._widget ); }

            return;
        }

        //! button focus
        void setButtonHovered( bool value )
        {
            if( _button._hovered == value ) return;
            if( _button._widget )
            { setHovered( _button._widget, value ); }
            return;
        }

        //! entry focus
        void setEntryFocus( bool value )
        {

            if( _entry._focus == value ) return;
            _entry._focus = value;

            // trigger button update
            if( _button._widget )
            { gtk_widget_queue_draw( _button._widget ); }

        }

        //@}

        //!@name accessors
        //@{

        //! list
        GtkWidget* list( void ) const
        { return _list; }

        //! true if either button or entry has focus
        bool hasFocus( void ) const
        { return _button._focus || _entry._focus; }

        //! true if either button or entry has hover
        bool hovered( void ) const
        { return HoverData::hovered() || _button._hovered || _entry._hovered; }

        //! pressed
        bool pressed( void ) const
        { return _button._pressed; }

        //@}

        protected:

        //! set hover flag for given widget
        void setPressed( GtkWidget*, bool );

        //! set hover flag for given widget
        virtual bool setHovered( GtkWidget*, bool );

        //! disconnect child
        void unregisterChild( GtkWidget* );

        //!@name callbacks
        //@{

        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );
        static void childToggledEvent( GtkWidget*, gpointer );

        //@}

        private:

        //! drop-down list, if set
        GtkWidget* _list;

        class Data
        {

            public:

            //! constructor
            Data( void ):
                _widget( 0L ),
                _focus( false ),
                _hovered( false ),
                _pressed( false )
            {}

            //! disconnect
            void disconnect( void );

            GtkWidget* _widget;
            bool _focus;
            bool _hovered;
            bool _pressed;

            //!@name callback ids
            //@{
            Signal _destroyId;
            Signal _enterId;
            Signal _leaveId;
            Signal _toggledId;
            //@}

        };

        //! button data
        Data _button;

        //! entry data
        Data _entry;

    };

}

#endif
