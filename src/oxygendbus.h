#ifndef oxygendbus_h
#define oxygendbus_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* Icon rendering code from Walmis
* <http://gnome-look.org/content/show.php?content=77783&forumpage=3>
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or( at your option ) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library; if not, write to the Free
* Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
* MA 02110-1301, USA.
*/
#include "config.h"

#if HAVE_DBUS_GLIB
#include <gio/gio.h>
#elif HAVE_DBUS
#include <dbus/dbus-glib-lowlevel.h>
#endif

namespace Oxygen
{

    //! pure static class that handles dbus connects
    class DBus
    {

        public:

        //! return singleton
        static DBus& instance( void );

        //! destructor
        virtual ~DBus( void );

        //! connect
        void connect( void );

        //! diconnect
        void disconnect( void );

        protected:

        #if HAVE_DBUS_GLIB

        //! dbus messages filter (callback)
        static void filter( GDBusConnection*, const gchar*, const gchar*, const gchar*, const gchar*, GVariant*, gpointer );

        #elif HAVE_DBUS

        //! dbus messages filter (callback)
        static DBusHandlerResult signalFilter( DBusConnection*, DBusMessage*, gpointer );

        #endif

        // true if connected
        bool isConnected( void ) const
        {
            #if HAVE_DBUS_GLIB
            return _connection;
            #elif HAVE_DBUS
            return _connection;
            #else
            return false;
            #endif
        }

        private:

        //! constructor
        DBus( void );

        #if HAVE_DBUS_GLIB
        GDBusConnection* _connection;
        #elif HAVE_DBUS
        DBusGConnection* _connection;
        #endif

        //! ids
        int _oxygenId;
        int _globalSettingsId;

        //! singleton
        static DBus* _instance;

    };

}

#endif
