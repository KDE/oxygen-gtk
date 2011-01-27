#ifndef oxygencomboboxdata_h
#define oxygencomboboxdata_h
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

#include "oxygensignal.h"

#include <gtk/gtk.h>
#include <algorithm>
#include <map>
#include <iostream>

namespace Oxygen
{

    // handles focus and hover and pressed down state for comboboxes
    class ComboBoxData
    {

        public:

        //! constructor
        ComboBoxData( void ):
            _target( 0L )
        {}

        //! destructor
        virtual ~ComboBoxData( void )
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

        //! button focus
        void setButtonFocus( bool value )
        {
            if( _button._focus == value ) return;
            _button._focus = value;

            // trigger update
            if( _target ) gtk_widget_queue_draw( _target );

            return;
        }

        //! register child
        void registerChild( GtkWidget*, bool recursive = true );

        //@}

        //!@name accessors
        //@{

        //! pressed
        bool pressed( void ) const
        { return _button._pressed; }

        //! true if either button or entry has focus
        bool hasFocus( void ) const
        { return _button._focus; }

        //! true if either button or entry has hover
        bool hovered( void ) const
        { return std::find_if( _hoverData.begin(), _hoverData.end(), HoveredFTor() ) != _hoverData.end(); }

        //@}

        protected:

        //! initialize cell view
        void initializeCellView( GtkWidget* value );

        //! update cell view color
        void updateCellViewColor( void ) const;

        //! update button event window
        /*!
        the trick is to extend the button event window by 6 pixels to the left,
        in order to fill a dead area created by the Combobox's GtkFrame's margin.
        */
        void updateButtonEventWindow( void ) const;

        //! set hover flag for given widget
        void setPressed( GtkWidget*, bool );

        //! set hover flag for given widget
        void setHovered( GtkWidget*, bool );

        //! disconnect child
        void unregisterChild( GtkWidget* );

        //!@name callbacks
        //@{

        static void childStyleChangeNotifyEvent( GtkWidget*, GtkStyle*, gpointer );
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );
        static void childToggledEvent( GtkWidget*, gpointer );
        static void childSizeAllocateEvent( GtkWidget*, GtkAllocation*, gpointer );

        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static void stateChangeEvent( GtkWidget*, GtkStateType, gpointer );


        //@}

        private:

        //! target widget
        GtkWidget* _target;

        Signal _stateChangeId;

        // handle child registration
        class ChildData
        {

            public:

            //! constructor
            explicit ChildData( void ):
                _widget(0L)
            {}

            //! destructor
            virtual ~ChildData( void )
            {}

            //! disconnect
            virtual void disconnect( void );

            //! widget
            GtkWidget* _widget;

            //!@name callback ids
            //@{
            Signal _destroyId;
            Signal _styleChangeId;
            //@}

        };

        // handle focus and toggle state
        class ButtonData: public ChildData
        {

            public:

            //! constructor
            explicit ButtonData( void ):
                _pressed( false ),
                _focus( false )
            {}

            //! destructor
            virtual ~ButtonData( void )
            {}

            //! disconnect
            virtual void disconnect( void );

            //! true if widget is down
            bool _pressed;

            //! true if widget has focus
            bool _focus;

            //! toggled callback Id
            Signal _toggledId;
            Signal _sizeAllocateId;

        };

        class HoverData: public ChildData
        {
            public:

            //! constructor
            explicit HoverData( void ):
                _hovered( false )
            {}

            //! destructor
            virtual ~HoverData( void )
            {}

            //! disconnect
            virtual void disconnect( void );

            //! true if widget is hovered
            bool _hovered;

            //!@name callback ids
            //@{
            Signal _enterId;
            Signal _leaveId;
            //@}

        };

        //! need to detect hovered child
        class HoveredFTor
        {
            public:

            bool operator () ( const std::pair<GtkWidget*, HoverData>& dataPair )
            { return dataPair.second._hovered; }

        };

        typedef std::map<GtkWidget*, HoverData> HoverDataMap;
        HoverDataMap _hoverData;

        //! true if cell layout has been initialized
        bool _cellLayoutInitialized;

        //! cell data
        ChildData _cell;

        //! button data
        ButtonData _button;

    };

}

#endif
