#ifndef oxygenwidgetstatedata_h
#define oxygenwidgetstatedata_h
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

#include "oxygentimeline.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! track widget state changes events
    class WidgetStateData
    {

        public:

        //! constructor
        explicit WidgetStateData( void ):
            _target( 0L ),
            _dirtyRect( Gtk::gdk_rectangle() ),
            _state( false )
        {}

        //! destructor
        virtual ~WidgetStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! update state
        bool updateState( bool value, const GdkRectangle& );

        //! update state
        bool updateState( bool value )
        { return updateState( value, Gtk::gdk_rectangle() ); }

        //! true if animated
        bool isAnimated( void ) const
        { return _timeLine.isRunning(); }

        //! opacity
        double opacity( void ) const
        { return _timeLine.value(); }

        //! enable state
        void setEnabled( bool value )
        { _timeLine.setEnabled( value ); }

        //! duration
        void setDuration( int value )
        { _timeLine.setDuration( value ); }

        protected:

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! target
        GtkWidget* _target;

        //! timeline
        TimeLine _timeLine;

        //! dirty rect
        GdkRectangle _dirtyRect;

        //! state
        bool _state;

    };

}

#endif
