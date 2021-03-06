#ifndef oxygensignal_h
#define oxygensignal_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <gtk/gtk.h>
#include <cassert>
#include <string>

namespace Oxygen
{
    //! handles gtk signal connections
    class Signal
    {
        public:

        //! constructor
        Signal( void ):
            _id(0),
            _object(0L)
        {}

        //! destructor
        virtual ~Signal( void )
        {}

        //! connect
        void connect( GObject*, const std::string&, GCallback, gpointer );

        //! disconnect
        void disconnect( void );

        //! check if signal belongs to given object
        bool belongsTo( GObject* object ) const
        { return object == _object; }

        private:

        //! signal id
        guint _id;

        //! connected object
        GObject* _object;

    };

}
#endif
