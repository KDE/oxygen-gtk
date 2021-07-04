/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
