#ifndef oxygengtkgap_h
#define oxygengtkgap_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or( at your option ) any later version.
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
    namespace Gtk
    {

        //! stores 'gap' parameters, for convenience
        class Gap
        {
            public:

            //! constructor
            explicit Gap( void ):
                _x(0),
                _w(0),
                _h( DefaultGapHeight ),
                _position( GTK_POS_TOP )
            {}

            //! constructor
            Gap( gint x, gint w, GtkPositionType position ):
                _x( x ),
                _w( w ),
                _h( DefaultGapHeight ),
                _position( position )
            {}

            //!@name accessors
            //@{

            const gint& x( void ) const { return _x; }
            const gint& width( void ) const { return _w; }
            const gint& height( void ) const { return _h; }
            const GtkPositionType& position( void ) const { return _position; }

            //@}

            //!@name modifiers
            //@{

            void setX( gint value ) { _x = value; }
            void setWidth( gint value) { _w = value; }
            void setHeight( gint value ) { _h = value; }

            //@}

            private:

            enum { DefaultGapHeight = 4 };

            gint _x;
            gint _w;
            gint _h;
            GtkPositionType _position;



        };

    }

}
#endif
