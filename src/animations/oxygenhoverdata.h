#ifndef oxygenhoverdata_h
#define oxygenhoverdata_h
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

#include <gtk/gtk.h>

namespace Oxygen
{
    class HoverData
    {

        public:

        //! constructor
        HoverData( void ):
            _enterId(-1),
            _leaveId(-1),
            _hovered( false )
        {}

        //! destructor
        virtual ~HoverData( void )
        {}

        //! setup connections
        virtual void connect( GtkWidget* );

        //! disconnect
        virtual void disconnect( GtkWidget* );

        //! true if hovered
        virtual bool hovered( void ) const
        { return _hovered; }

        protected:

        //! set mouse over state
        virtual bool setHovered( GtkWidget*, bool value )
        {
            if( _hovered == value ) return false;
            _hovered = value;
            return true;
        }

        //!@name static callbacks
        //@{
        static gboolean enterNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        static gboolean leaveNotifyEvent( GtkWidget*, GdkEventCrossing*, gpointer);
        //@}

        private:

        //! enter signal id
        int _enterId;

        //! leave signal id
        int _leaveId;

        //! true if hovered
        bool _hovered;

    };

}

#endif
