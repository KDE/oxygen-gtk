#ifndef oxygenmenustatedata_h
#define oxygenmenustatedata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* MenuState prelight effect is based on
* Redmond95 - a cairo based GTK+ engine
* Copyright (C) 2001 Red Hat, Inc. <@redhat.com>
* Copyright (C) 2006 Andrew Johnson <acjgenius@earthlink.net>
* Copyright (C) 2006-2007 Benjamin Berg <benjamin@sipsolutions.net>
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

#include "../oxygenanimationdata.h"
#include "../oxygenanimationmodes.h"
#include "../oxygengtkutils.h"
#include "oxygensignal.h"
#include "oxygentimeline.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    class MenuStateData
    {

        public:

        //! constructor
        MenuStateData( void ):
            _target( 0L ),
            _animationsEnabled( true ),
            _dirtyRect( Gtk::gdk_rectangle() )
            {}

        //! destructor
        virtual ~MenuStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! enable state
        void setAnimationsEnabled( bool value )
        {
            _animationsEnabled = value;
            _current._timeLine.setEnabled( value );
            _previous._timeLine.setEnabled( value );

            if( !value )
            {
                _current.clear();
                _previous.clear();
            }

        }

        //! duration
        void setDuration( int value )
        {
            _current._timeLine.setDuration( value );
            _previous._timeLine.setDuration( value );
        }

        //!@name accessors
        //@{

        //! true if animated
        bool isAnimated( void ) const
        { return isAnimated( AnimationCurrent ) || isAnimated( AnimationPrevious ); }

        //! true if given animation type is animated
        bool isAnimated( const AnimationType& type ) const
        { return data( type )._timeLine.isRunning(); }

        //! widget for current animation type
        GtkWidget* widget( const AnimationType& type ) const
        { return data( type )._widget; }

        //! rect for given animation type
        const GdkRectangle& rectangle( const AnimationType& type ) const
        { return data( type )._rect; }

        //! animation data
        AnimationData animationData( const AnimationType& type ) const
        {
            const Data& data( this->data( type ) );
            return data._timeLine.isRunning() ?
                AnimationData( data._timeLine.value(), AnimationHover ):
                AnimationData();
        }

        //@}

        protected:

        //! update items
        void updateItems( void );

        //! update state for given widget
        bool updateState( GtkWidget*, const GdkRectangle&, int, int, bool );

        //! true if menu item is active (pressed down)
        bool menuItemIsActive( GtkWidget* ) const;

        //! return dirty rect (for update)
        GdkRectangle dirtyRect( void );

        //! animations data
        class Data
        {

            public:

            //! constructor
            explicit Data( void ):
                _widget( 0L ),
                _rect( Gtk::gdk_rectangle() ),
                _xOffset( 0 ),
                _yOffset( 0 )
            {}

            //! update data
            void copy( const Data& other )
            {
                _widget = other._widget;
                _rect = other._rect;
                _xOffset = other._xOffset;
                _yOffset = other._yOffset;
            }

            //! update data
            void update( GtkWidget* widget, const GdkRectangle& rect, int xOffset, int yOffset )
            {
                _widget = widget;
                _rect = rect;
                _xOffset = xOffset;
                _yOffset = yOffset;
            }

            //! true if valid
            bool isValid( void ) const
            { return _widget && Gtk::gdk_rectangle_is_valid( &_rect ); }

            //! clear
            void clear( void )
            {
                if( _timeLine.isRunning() ) _timeLine.stop();
                _widget = 0L;
                _rect = Gtk::gdk_rectangle();
            }

            //! timeline
            TimeLine _timeLine;

            //! widget
            GtkWidget* _widget;

            //! rectangle
            GdkRectangle _rect;

            //! offset between paint window and menu window
            int _xOffset;
            int _yOffset;

        };

        //! get data for given animation type
        Data& data( const AnimationType& type )
        {
            switch( type )
            {
                default:
                case AnimationCurrent: return _current;
                case AnimationPrevious: return _previous;
            }
        }

        //! get data for given animation type
        const Data& data( const AnimationType& type ) const
        {
            switch( type )
            {
                default:
                case AnimationCurrent: return _current;
                case AnimationPrevious: return _previous;
            }
        }

        //!@name callbacks
        //@{
        static gboolean motionNotifyEvent( GtkWidget*, GdkEventMotion*, gpointer);
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        static gboolean delayedUpdate( gpointer );
        //@}

        private:

        //! target
        GtkWidget* _target;

        //!@name signals
        //@{
        Signal _motionId;
        Signal _leaveId;
        //@}

        //!@name animation data
        //@{

        //! true if menu item mouse-over is animated
        bool _animationsEnabled;

        //! additional dirty rect
        GdkRectangle _dirtyRect;

        Data _previous;
        Data _current;

        //@}

    };

}

#endif
