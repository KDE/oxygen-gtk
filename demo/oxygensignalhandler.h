#ifndef oxygensignalhandler_h
#define oxygensignalhandler_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include <set>
#include <vector>

namespace Oxygen
{

    class SignalHandler
    {

        public:

        //! constructor
        SignalHandler( void )
        {}

        //! destructor
        virtual ~SignalHandler( void );

        //! connect
        void connect( GObject*, const std::string&, GCallback, gpointer );

        protected:

        //! destruction callback
        static gboolean destroyNotifyEvent( GtkWidget*, gpointer );

        private:

        //! registered widgets
        std::set<GObject*> _objects;

        //! registered signals
        std::vector<Signal> _signals;

    };

}

#endif
