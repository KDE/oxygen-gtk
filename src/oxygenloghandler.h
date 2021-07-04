#ifndef oxygenloghandler_h
#define oxygenloghandler_h
/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <gtk/gtk.h>

namespace Oxygen
{
    class LogHandler
    {

        public:

        //! constructor
        LogHandler( void );

        //! destructor
        virtual ~LogHandler( void );

        protected:

        //!@name error handlers
        //@{
        static void gtkLogHandler( const gchar*, GLogLevelFlags, const gchar*, gpointer );
        static void glibLogHandler( const gchar*, GLogLevelFlags, const gchar*, gpointer );
        //@}

        private:

        //! gtk log id
        guint _gtkLogId;


        //! glib log id
        guint _glibLogId;

    };

}

#endif
