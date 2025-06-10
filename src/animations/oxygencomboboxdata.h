#ifndef oxygencomboboxdata_h
#define oxygencomboboxdata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
            _target( 0L ),
            _list( 0L )
        {}

        //! destructor
        virtual ~ComboBoxData( void )
        { disconnect( _target ); }

        //! setup connections
        /*! does nothing. Only kept here for consistency with other data */
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //!@name modifiers
        //@{

        //! list
        void setList( GtkWidget* widget )
        { _list = widget; }

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

        //! list
        GtkWidget* list( void ) const
        { return _list; }

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

        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );
        static void childToggledEvent( GtkWidget*, gpointer );
        static void childSizeAllocateEvent( GtkWidget*, GtkAllocation*, gpointer );

        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static void stateChangeEvent( GtkWidget*, GtkStateType, gpointer );
        static void styleSetEvent( GtkWidget*, GtkStyle*, gpointer );

        //@}

        private:

        //! target widget
        GtkWidget* _target;

        //! drop-down list, if set
        GtkWidget* _list;

        //!@name signals
        //@{

        //! state change signal
        Signal _stateChangeId;

        //! style set
        Signal _styleSetId;

        //@}

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

            //!callback id
            Signal _destroyId;

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

        //! cell data
        ChildData _cell;

        //! button data
        ButtonData _button;

    };

}

#endif
