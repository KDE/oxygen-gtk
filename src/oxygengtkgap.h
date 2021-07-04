#ifndef oxygengtkgap_h
#define oxygengtkgap_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
