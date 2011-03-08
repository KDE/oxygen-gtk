#ifndef oxygentoolbarstatedata_h
#define oxygentoolbarstatedata_h
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

#include "../oxygenanimationdata.h"
#include "../oxygenanimationmodes.h"
#include "../oxygengtkutils.h"
#include "oxygenfollowmousedata.h"
#include "oxygensignal.h"
#include "oxygentimeline.h"
#include "oxygentimer.h"

#include <map>
#include <gtk/gtk.h>

namespace Oxygen
{
    class ToolBarStateData: public FollowMouseData
    {

        public:

        //! constructor
        ToolBarStateData( void ):
            _target( 0L ),
            _dirtyRect( Gtk::gdk_rectangle() )
        {}

        //! destructor
        virtual ~ToolBarStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! register child
        void registerChild( GtkWidget*, bool = true );

        //!@name modifiers
        //@{

        //! enable state
        void setEnabled( bool value )
        {
            FollowMouseData::setEnabled( value );

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

        //@}

        //!@name accessors
        //@{

        //! true if animated
        bool isAnimated( void ) const
        { return isAnimated( AnimationCurrent ) || isAnimated( AnimationPrevious ); }

        //! true if given animation type is animated
        bool isAnimated( const WidgetType& type ) const
        { return data( type )._timeLine.isRunning(); }

        //! widget matching type
        GtkWidget* widget( const WidgetType& type ) const
        { return data( type )._widget; }

        //! rect for given animation type
        const GdkRectangle& rectangle( const WidgetType& type ) const
        { return data( type )._rect; }

        //! animation data
        AnimationData animationData( const WidgetType& type ) const
        {
            const Data& data( this->data( type ) );
            return data._timeLine.isRunning() ?
                AnimationData( data._timeLine.value(), AnimationHover ):
                AnimationData();
        }

        //! true when fade out animation is locked (delayed)
        bool isLocked( void ) const
        { return _timer.isRunning(); }

        //@}

        protected:

        //! disconnect child
        void unregisterChild( GtkWidget* );

        //! update state for given widget
        bool updateState( GtkWidget*, bool state, bool delayed );

        //! return dirty rect (for update)
        GdkRectangle dirtyRect( void );

        //! animations data
        class Data
        {

            public:

            //! constructor
            explicit Data( void ):
                _widget( 0L ),
                _rect( Gtk::gdk_rectangle() )
            {}

            //! update data
            void copy( const Data& other )
            {
                _widget = other._widget;
                _rect = other._rect;
            }

            //! update data
            void update( GtkWidget* widget, const GdkRectangle& rect )
            {
                _widget = widget;
                _rect = rect;
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

        };

        //! get data for given animation type
        Data& data( const WidgetType& type )
        {
            switch( type )
            {
                default:
                case AnimationCurrent: return _current;
                case AnimationPrevious: return _previous;
            }
        }

        //! get data for given animation type
        const Data& data( const WidgetType& type ) const
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

        //! child is destroyed
        static gboolean childDestroyNotifyEvent( GtkWidget*, gpointer );

        //! child entered
        static gboolean childEnterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);

        //! child left
        static gboolean childLeaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);

        //! mouse leave events
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);

        //! update widget for fade-in/fade-out animation
        static gboolean delayedUpdate( gpointer );

        //! update widget for follow-mouse animation
        static gboolean followMouseUpdate( gpointer );

        //! start delayed fade-out animation
        static gboolean delayedAnimate( gpointer );

        //@}

        private:

        //! target
        GtkWidget* _target;

        //! signals
        Signal _leaveId;

        //!@name animation data
        //@{

        //! additional dirty rect
        GdkRectangle _dirtyRect;

        Data _previous;
        Data _current;

        //@}

        //!@name hover data
        /*! need to keep track of enter/leave events in toolbar items */
        class HoverData
        {

            public:

            //! constructor
            explicit HoverData( void )
            {}

            //! destructor
            virtual ~HoverData( void )
            {}

            //! disconnect
            virtual void disconnect( void )
            {
                _destroyId.disconnect();
                _enterId.disconnect();
                _leaveId.disconnect();
            }

            //!@name callback ids
            //@{
            Signal _destroyId;
            Signal _enterId;
            Signal _leaveId;
            //@}

        };

        typedef std::map<GtkWidget*, HoverData> HoverDataMap;
        HoverDataMap _hoverData;

        //! delayed animation timeOut
        static const int _timeOut;

        //! timer of delayed animation
        Timer _timer;

    };

}

#endif
