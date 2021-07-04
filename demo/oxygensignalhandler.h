#ifndef oxygensignalhandler_h
#define oxygensignalhandler_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
