#ifndef oxygenwidgetstatedata_h
#define oxygenwidgetstatedata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include <gtk/gtk.h>

namespace Oxygen
{
    // track widget state changes events
    class WidgetStateData
    {

        public:

        //! constructor
        WidgetStateData( void );

        //! destructor
        virtual ~WidgetStateData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! update state
        bool updateState( bool );

        //! return timeline
        const TimeLine& timeLine( void ) const
        { return _timeLine; }

        //! return timeline
        TimeLine& timeLine( void )
        { return _timeLine; }

        //! opacity
        double opacity( void ) const
        { return timeLine().value(); }

        protected:

        //! delayed update
        static gboolean delayedUpdate( gpointer );

        private:

        //! target
        GtkWidget* _target;

        //! timeline
        TimeLine _timeLine;

        //! state
        bool _state;

    };

}

#endif
