/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenloghandler.h"

#include <string>

namespace Oxygen
{

    //_________________________________________________________
    LogHandler::LogHandler( void )
    {
        _gtkLogId = g_log_set_handler( "Gtk", G_LOG_LEVEL_WARNING, gtkLogHandler, 0L );
        _glibLogId = g_log_set_handler( "GLib-GObject", G_LOG_LEVEL_CRITICAL, glibLogHandler, 0L );
    }

    //_________________________________________________________
    LogHandler::~LogHandler( void )
    {

        if( _gtkLogId > 0 )
        {

            g_log_remove_handler( "Gtk", _gtkLogId );
            g_log_set_handler( "Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, 0L );

        }

        if( _glibLogId > 0 )
        {

            g_log_remove_handler( "GLib-GObject", _glibLogId );
            g_log_set_handler( "GLib-GObject", G_LOG_LEVEL_CRITICAL, g_log_default_handler, 0L );

        }

    }

    //_________________________________________________________
    void LogHandler::gtkLogHandler( const gchar* domain, GLogLevelFlags flags, const gchar* message, gpointer data )
    {

        if( std::string( message ).find( "attempt to underallocate" ) != std::string::npos ) return;
        g_log_default_handler( domain, flags, message, data );

    }

    //_________________________________________________________
    void LogHandler::glibLogHandler( const gchar* domain, GLogLevelFlags flags, const gchar* message, gpointer data )
    {

        if( std::string( message ).find( "g_object_ref" ) != std::string::npos ) return;
        g_log_default_handler( domain, flags, message, data );

    }

}
