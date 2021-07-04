#ifndef oxygenscrollbarstatedata_h
#define oxygenscrollbarstatedata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "../oxygengtkutils.h"
#include "oxygentimeline.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! track scrollbar arrow state changes events
    class ScrollBarStateData
    {

        public:

        //! constructor
        explicit ScrollBarStateData( void ):
            _target( 0L )
        {}

        //! destructor
        virtual ~ScrollBarStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! update state
        bool updateState( GtkArrowType type, bool value )
        { return data( type ).updateState( value ); }

        //! enable state
        void setEnabled( bool value )
        {
            data( GTK_ARROW_UP )._timeLine.setEnabled( value );
            data( GTK_ARROW_DOWN )._timeLine.setEnabled( value );
        }

        //! duration
        void setDuration( int value )
        {
            data( GTK_ARROW_UP )._timeLine.setDuration( value );
            data( GTK_ARROW_DOWN )._timeLine.setDuration( value );
        }

        //! true if timeline for give arrow type is running
        bool isAnimated( GtkArrowType type ) const
        { return data( type )._timeLine.isRunning(); }

        //! opacity
        double opacity( GtkArrowType type ) const
        { return data( type )._timeLine.value(); }

        //! rect
        const GdkRectangle& rect( GtkArrowType type ) const
        { return data( type )._rect; }

        //! rect
        void setRect( GtkArrowType type, const GdkRectangle& rect )
        { data( type )._rect = rect; }

        protected:

        // arrow data
        class Data
        {

            public:

            //! constructor
            explicit Data( void ):
                _rect( Gtk::gdk_rectangle() ),
                _state( false )
            {}

            //! update state
            bool updateState( bool );

            //! timeline
            TimeLine _timeLine;

            //! rectangle
            GdkRectangle _rect;

            //! state
            bool _state;

        };

        //! get data for given arrow type
        const Data& data( GtkArrowType type ) const
        {
            switch( type )
            {

                case GTK_ARROW_UP:
                case GTK_ARROW_LEFT:
                return _upArrowData;

                default:
                case GTK_ARROW_DOWN:
                case GTK_ARROW_RIGHT:
                return _downArrowData;

            }
        }

        //! get data for given arrow type
        Data& data( GtkArrowType type )
        {
            switch( type )
            {

                case GTK_ARROW_UP:
                case GTK_ARROW_LEFT:
                return _upArrowData;

                default:
                case GTK_ARROW_DOWN:
                case GTK_ARROW_RIGHT:
                return _downArrowData;

            }
        }

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! target
        GtkWidget* _target;

        // up/left arrow
        Data _upArrowData;

        // down/right arrow
        Data _downArrowData;

    };

}

#endif
