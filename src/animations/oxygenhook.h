#ifndef oxygenhook_h
#define oxygenhook_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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
#include <string>

namespace Oxygen
{
    //! handles gtk signal hooks
    class Hook
    {
        public:

        //! constructor
        Hook( void ):
            _signalId(0),
            _hookId(0)
        {}

        //! destructor
        virtual ~Hook( void )
        {}



        //! connect
        bool connect( const std::string&, GType, GSignalEmissionHook, gpointer );

        bool connect( const std::string& signal, GSignalEmissionHook hook, gpointer data )
        { return connect( signal, GTK_TYPE_WIDGET, hook, data ); }

        //! disconnect
        void disconnect( void );

        private:

        //! signal id
        guint _signalId;
        gulong _hookId;

    };

}

#endif
