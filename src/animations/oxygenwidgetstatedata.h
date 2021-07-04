#ifndef oxygenwidgetstatedata_h
#define oxygenwidgetstatedata_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
