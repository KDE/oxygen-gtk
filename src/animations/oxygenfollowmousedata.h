#ifndef oxygenfollowmousedata_h
#define oxygenfollowmousedata_h
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

#include "../oxygengtkutils.h"
#include "oxygentimeline.h"

#include <gdk/gdk.h>

namespace Oxygen
{
    //! stores data needed for follow-mouse animations
    class FollowMouseData
    {
        public:

        //! constructor
        FollowMouseData( void ):
            _followMouse( false ),
            _startRect( Gtk::gdk_rectangle() ),
            _endRect( Gtk::gdk_rectangle() ),
            _animatedRect( Gtk::gdk_rectangle() ),
            _dirtyRect( Gtk::gdk_rectangle() )
        {}

        //! destructor
        virtual ~FollowMouseData( void )
        {}

        //!@name modifiers
        //@{

        //! enable state
        virtual void setEnabled( bool value )
        { _timeLine.setEnabled( value ); }

        //! follow-mouse animation
        virtual void setFollowMouse( bool value )
        { _followMouse = value; }

        //! follow-mouse animation duration
        virtual void setFollowMouseAnimationsDuration( int value )
        { _timeLine.setDuration( value ); }

        //@}

        //!@name accessors
        //@{

        //! returns true if animated rectangle is valid
        virtual bool animatedRectangleIsValid( void ) const
        { return _timeLine.isRunning() && Gtk::gdk_rectangle_is_valid( &_animatedRect ); }

        //! animated rectangle
        virtual const GdkRectangle& animatedRectangle( void ) const
        { return _animatedRect; }

        //@}

        protected:

        //! connect
        virtual void connect( GSourceFunc function, gpointer data )
        {
            _timeLine.connect( function, data );
            _timeLine.setDirection( TimeLine::Forward );
        }

        //! disconnect
        virtual void disconnect( void )
        { _timeLine.disconnect(); }

        //! true if enabled
        virtual bool followMouse( void ) const
        { return _followMouse; }

        //! follow-mouse dirty rect
        virtual GdkRectangle dirtyRect( void );

        //! start follow-mouse animation
        virtual void startAnimation( const GdkRectangle&, const GdkRectangle& );

        //! update animated rect
        virtual void updateAnimatedRect( void );

        private:

        //! true if enabled
        bool _followMouse;

        //! timeline
        TimeLine _timeLine;

        //! start rectangle
        GdkRectangle _startRect;

        //! end rectangle
        GdkRectangle _endRect;

        //! animated rectangle
        GdkRectangle _animatedRect;

        //! dirty rect
        GdkRectangle _dirtyRect;

    };


}

#endif
