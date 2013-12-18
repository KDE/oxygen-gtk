#ifndef oxygenhoverdata_h
#define oxygenhoverdata_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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

#include "oxygensignal.h"

#include <gtk/gtk.h>

namespace Oxygen
{
    class HoverData
    {

        public:

        //! constructor
        HoverData( void ):
            _hovered( false ),
            _updateOnHover( false )
        {}

        //! destructor
        virtual ~HoverData( void )
        { disconnect( 0L ); }

        //! setup connections
        virtual void connect( GtkWidget* );

        //! disconnect
        virtual void disconnect( GtkWidget* );

        //! true if hovered
        virtual bool hovered( void ) const
        { return _hovered; }

        //! true if hover state change should trigger widget repaint
        void setUpdateOnHover( bool value )
        { _updateOnHover = value; }

        //! set mouse over state
        virtual bool setHovered( GtkWidget* widget, bool value )
        {
            if( _hovered == value ) return false;
            _hovered = value;

            // schedule repaint
            if( _updateOnHover )
            { gtk_widget_queue_draw( widget ); }

            return true;
        }

        protected:

        //!@name static callbacks
        //@{
        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer );
        //@}

        private:

        //! enter signal id
        Signal _enterId;

        //! leave signal id
        Signal _leaveId;

        //! true if hovered
        bool _hovered;

        //! also update
        bool _updateOnHover;

    };

}

#endif
