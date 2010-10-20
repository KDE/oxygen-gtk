#ifndef oxygencomboboxentrydata_h
#define oxygencomboboxentrydata_h
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

#include <gtk/gtk.h>

namespace Oxygen
{
    class ComboBoxEntryData
    {

        public:

        //! constructor
        ComboBoxEntryData( void ):
            _hovered( false )
        {}

        //! destructor
        virtual ~ComboBoxEntryData( void )
        {}

        //! setup connections
        /*! does nothing. Only kept here for consistency with other data */
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //!@name modifiers
        //@{

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

        //! true if either button or entry has focus
        bool hasFocus( void ) const
        { return _button._focus || _entry._focus; }

        //!@name  hover
        //@{

        bool hovered( void ) const
        { return _hovered || _button._hovered || _entry._hovered; }

        //@}

        protected:

        //! set hover flag for given widget
        void setHovered( GtkWidget*, bool );

        //! disconnect child
        void unregisterChild( GtkWidget* );

        //!@name callbacks
        //@{

        static void childStyleChangeNotifyEvent( GtkWidget*, GtkStyle*, gpointer );
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );

        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        //@}

        private:

        class Data
        {

            public:

            //! constructor
            Data( void ):
                _widget( 0L ),
                _focus( false ),
                _hovered( false ),
                _enterId(-1),
                _leaveId(-1)
            {}

            //! disconnect
            void disconnect( void );

            GtkWidget* _widget;
            bool _focus;
            bool _hovered;

            //!@name callback ids
            //@{
            int _destroyId;
            int _styleChangeId;
            int _enterId;
            int _leaveId;
            //@}

        };

        //! button data
        Data _button;

        //! entry data
        Data _entry;

        //! hover
        bool _hovered;

        //!@name callback ids
        //@{
        int _enterId;
        int _leaveId;
        //@}

    };

}

#endif
