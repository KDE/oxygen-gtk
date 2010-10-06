#ifndef oxygenmenushelldata_h
#define oxygenmenushelldata_h
/*
* this file is part of the oxygen gtk engine
* Copyright(c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright(C ) 2008 Robert Staudinger
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
    class MenuShellData
    {

        public:

        //! constructor
        MenuShellData():
            _motionId(-1),
            _leaveId(-1)
        {}

        //! constructor
        MenuShellData( GtkWidget* );

        //!@name accessors
        //@{

        int motionId( void ) const { return _motionId; }
        int leaveId( void ) const { return _leaveId; }

        //@}

        private:

        //! motion signal id
        int _motion_id;

        //! leave signal id
        int _leave_id;

    };

}

#endif
