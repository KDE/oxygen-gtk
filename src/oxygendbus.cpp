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
        if( !_instance ) _instance = new DBus();
        return *_instance;
    }

    //_________________________________________________________
    DBus::DBus( void ):
        #if HAVE_DBUS_GLIB
        _connection( 0L ),
        #elif HAVE_DBUS
        _connection( 0L ),
        #endif
        _oxygenId( 0 ),
        _globalSettingsId( 0 )
    { }

    //_________________________________________________________
    DBus::~DBus( void )
    { _instance = 0L; }

    //_________________________________________________________
    void DBus::connect( void )
    {

        #if HAVE_DBUS_GLIB

        // check connection
        if( isConnected() ) return;

        // get dbus connection
        _connection = g_bus_get_sync(G_BUS_TYPE_SESSION, 0L, 0L );
        if( !_connection ) return;

        // setup subscriptions
        if( _oxygenId <= 0 )
        {
            _oxygenId = g_dbus_connection_signal_subscribe(
                _connection,
                0L, "org.kde.Oxygen.Style", 0L, "/OxygenStyle", 0L,
                G_DBUS_SIGNAL_FLAGS_NONE,
                filter, 0L, 0L );
        }

        if( _globalSettingsId <= 0 )
        {
            _globalSettingsId = g_dbus_connection_signal_subscribe(
                _connection,
                0L, "org.kde.KGlobalSettings", 0L, "/KGlobalSettings", 0L,
                G_DBUS_SIGNAL_FLAGS_NONE,
                filter, 0L, 0L );
        }

        #elif HAVE_DBUS

        // check connection
        if( isConnected() ) return;

        // connect to session bus and check
        _connection = dbus_g_bus_get( DBUS_BUS_SESSION, 0L );
        if( !_connection ) return;

        // install signal filter
        dbus_bus_add_match( dbus_g_connection_get_connection( _connection ), "type='signal',interface='org.kde.Oxygen.Style',path='/OxygenStyle'", 0L );
        dbus_bus_add_match( dbus_g_connection_get_connection( _connection ), "type='signal',interface='org.kde.KGlobalSettings',path='/KGlobalSettings'", 0L );
        dbus_connection_add_filter( dbus_g_connection_get_connection( _connection ), signalFilter, 0L, 0L );

        #endif
    }


    //_________________________________________________________
    void DBus::disconnect( void )
    {
        #if HAVE_DBUS_GLIB

        // check connection
        if( !isConnected() ) return;

        // release subscriptions
        if( _oxygenId > 0 )
        {
          g_dbus_connection_signal_unsubscribe( _connection, _oxygenId );
          _oxygenId = 0;
        }

        if( _globalSettingsId > 0 )
        {
          g_dbus_connection_signal_unsubscribe( _connection, _globalSettingsId );
          _globalSettingsId = 0;
        }

        // unref connection
        g_object_unref( _connection );
        _connection = 0L;

        #elif HAVE_DBUS

        // check connection
        if( !isConnected() ) return;

        dbus_connection_remove_filter( dbus_g_connection_get_connection( _connection ), signalFilter, 0L );
        dbus_g_connection_unref( _connection );
        _connection = 0L;

        #endif
    }

    #if HAVE_DBUS_GLIB

    //_________________________________________________________
    void DBus::filter( GDBusConnection*, const gchar* sender, const gchar* path, const gchar* interface, const gchar* signal, GVariant*, gpointer )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::DBus::signalFilter - received signal"
            << " sender: " << ( sender ? sender:"0x0" )
            << " path: " << (path ? path:"0x0")
            << " interface: " << (interface ? interface:"0x0")
            << " signal: " << (signal ? signal:"0x0")
            << std::endl;
        #endif

        if( !signal ) return;
        const std::string signalString( signal );
        if( signalString ==  "reparseConfiguration" )
        {

            Style::instance().initialize( QtSettings::Oxygen|QtSettings::Forced );

        } else if( signalString == "notifyChange" ) {

            Style::instance().initialize( QtSettings::All|QtSettings::Forced );

        } else return;

        // reset RC styles
        gtk_rc_reset_styles( gtk_settings_get_default() );

    }

    #elif HAVE_DBUS

    //_________________________________________________________
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
