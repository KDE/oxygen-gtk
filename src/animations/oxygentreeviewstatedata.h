#ifndef oxygentreeviewstatedata_h
#define oxygentreeviewstatedata_h
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
#include "../oxygenanimationdata.h"
#include "../oxygengtkcellinfo.h"
#include "../oxygengtkutils.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    //! track arrow state changes events,
    /*! used to deal with spinbox and Notebook arrows */
    class TreeViewStateData
    {

        public:

        //! constructor
        explicit TreeViewStateData( void ):
            _target( 0L ),
            _dirtyRect( Gtk::gdk_rectangle() )
        {}

        //! destructor
        virtual ~TreeViewStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! update state
        bool updateState( const Gtk::CellInfo&, bool );

        //! enable state
        void setEnabled( bool value )
        {
            _current._timeLine.setEnabled( value );
            _previous._timeLine.setEnabled( value );
        }

        //! duration
        void setDuration( int value )
        {
            _current._timeLine.setDuration( value );
            _previous._timeLine.setDuration( value );
        }

        //! true if tab info is animated
        bool isAnimated( const Gtk::CellInfo& info ) const
        {
            if( info == _current._info ) return _current._timeLine.isRunning();
            else if( info == _previous._info ) return _previous._timeLine.isRunning();
            else return false;
        }

        //! opacity
        double opacity( const Gtk::CellInfo& info ) const
        {
            if( info == _current._info ) return _current._timeLine.value();
            else if( info == _previous._info ) return _previous._timeLine.value();
            else return OpacityInvalid;
        }

        protected:

        //! return dirty rect (for update)
        GdkRectangle dirtyRect( void );

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! tab data
        class Data
        {

            public:

            //! constructor
            explicit Data( void )
            {}

            //! timeline
            TimeLine _timeLine;

            //! tab info
            Gtk::CellInfo _info;

        };

        //! target
        GtkWidget* _target;

        //! current tab data
        Data _current;

        //! previous tab data
        Data _previous;

        //! additional dirty rect
        GdkRectangle _dirtyRect;

    };

}

#endif
