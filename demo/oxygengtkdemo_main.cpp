/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#include "oxygenversion.h"
#include "oxygendemodialog.h"

//___________________________________________________________________
int main( int argc, char** argv )
{

    // initialize gtk
    gtk_init(&argc, &argv);

    // command line arguments
    gboolean version( FALSE );
    GOptionEntry entries[] =
    {
        { "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Show the application's version", 0L },
        { 0L }
    };

    GError *error = 0L;
    GOptionContext* context( g_option_context_new( "- Gtk+ widgets preview for oxygen" ) );
    g_option_context_add_main_entries(context, entries, 0L );
    g_option_context_add_group (context, gtk_get_option_group( TRUE ) );
    g_option_context_parse( context, &argc, &argv, &error );
    g_option_context_free( context );

    if( version )
    {
        /*
        HACK: need to create (and destroy immediatly) a dummy window
        in order to make sure that the widget style is initialized properly
        */
        GtkWidget* window = gtk_window_new( GTK_WINDOW_TOPLEVEL );
        gtk_widget_destroy( window );

        Oxygen::Version::print();
        return 0;
    }

    // dialog
    Oxygen::DemoDialog demoDialog;

    // connect demo dialog destruction to quit
    g_signal_connect( G_OBJECT( demoDialog.mainWidget() ), "destroy", G_CALLBACK(gtk_main_quit), 0L);
    gtk_widget_show( demoDialog.mainWidget() );

    // run main loop
    gtk_main();

    return 0;

}
