#ifndef oxygenlineeditdata_h
#define oxygenlineeditdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger
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

#include <gtk/gtkwidget.h>

namespace Oxygen
{
    class LineEditData
    {

        public:

        //! constructor
        LineEditData( void ):
            _motionId(-1),
            _leaveId(-1),
            _hovered( false )
        {}

        //! destructor
        virtual ~LineEditData( void )
        {}

        //! setup connections
        void connect( GtkWidget* );

        //! disconnect
        void disconnect( GtkWidget* );

        //! true if hovered
        bool hovered( void ) const
        { return _hovered; }

        protected:

        //! true if hovered
        void setHovered( GtkWidget* widget, bool value )
        {
            if( _hovered == value ) return;
            _hovered = value;
            gtk_widget_queue_draw( widget );
        }

        //!@name static callbacks
        //@{
        static gboolean motionNotifyEvent( GtkWidget*, GdkEventMotion*, gpointer);
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        //@}

        private:

        //! motion signal id
        int _motionId;

        //! leave signal id
        int _leaveId;

        //! true if hovered
        bool _hovered;

    };

}

#endif
