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

#include "oxygendbus.h"
#include "oxygenstyle.h"
#include "config.h"

#include <gtk/gtk.h>
#include <iostream>

namespace Oxygen
{
    //__________________________________________________________________
    DBus* DBus::_instance = 0;
    DBus& DBus::instance( void )
    {
        if( !_instance )
        {
            _instance = new DBus();
            _instance->setupConnection();
        }

        return *_instance;
    }

    //_________________________________________________________
    void DBus::setupConnection( void )
    {

        // make sure the method is called only once
        static bool first( true );
        if( !first ) return;
        first = false;

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::DBus::setupConnection" << std::endl;
        #endif

        #if HAVE_DBUS
        // dbus error
        DBusError error;
        dbus_error_init( &error );

        // connect to session bus and check
        DBusConnection *bus( dbus_bus_get( DBUS_BUS_SESSION, &error ) );
        if( !bus )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::DBus::setupConnection -"
                << " connection failed."
                << " Error: " << error.message
                << std::endl;
            #endif

            dbus_error_free( &error );
            return;
        }
        dbus_connection_setup_with_g_main( bus, 0L );

        // install signal filter
        dbus_bus_add_match( bus, "type='signal',interface='org.kde.Oxygen.Style',path='/OxygenStyle'", &error );
        dbus_bus_add_match( bus, "type='signal',interface='org.kde.KGlobalSettings',path='/KGlobalSettings'", &error );
        dbus_connection_add_filter( bus, signalFilter, 0L, 0L );
        #endif
    }

    //_________________________________________________________
    #if HAVE_DBUS
    DBusHandlerResult DBus::signalFilter( DBusConnection*, DBusMessage* message, gpointer data )
    {

        #if OXYGEN_DEBUG
        const char* path( dbus_message_get_path( message ) );
        const char* interface( dbus_message_get_interface( message ) );
        std::cerr
            << "Oxygen::DBus::signalFilter - received signal"
            << " type: " << dbus_message_get_type( message )
            << " path: " << (path ? path:"0x0")
            << " interface: " << (interface ? interface:"0x0")
            << std::endl;
        #endif

        if( dbus_message_is_signal( message, "org.kde.Oxygen.Style", "reparseConfiguration" ) )
        {

            Style::instance().initialize( QtSettings::Oxygen|QtSettings::Forced );
            gtk_rc_reset_styles( gtk_settings_get_default() );
            return DBUS_HANDLER_RESULT_HANDLED;

        } else if( dbus_message_is_signal( message, "org.kde.KGlobalSettings", "notifyChange" ) ) {

            Style::instance().initialize( QtSettings::All|QtSettings::Forced );
            gtk_rc_reset_styles( gtk_settings_get_default() );
            return DBUS_HANDLER_RESULT_HANDLED;

        } else return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;

    }
    #endif

}
