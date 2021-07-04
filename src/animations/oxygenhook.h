#ifndef oxygenhook_h
#define oxygenhook_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
