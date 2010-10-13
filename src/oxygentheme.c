/*
* this file is part of the oxygen gtk engine
* Copyright( c ) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the Null Theme Engine for Gtk+.
* Copyright( C ) 2008 Robert Staudinger
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

#include <gmodule.h>
#include <gtk/gtk.h>
#include <cstdio>

#include "config.h"

//_________________________________________________
G_MODULE_EXPORT void theme_init( GTypeModule* module )
{
    oxygen_rc_style_register_type( module );
    oxygen_style_register_type( module );

    // make all widgets have ARGB visual by default
    FILE* configFile=fopen(GTK_THEME_DIR "/argb-apps.conf","r");
    if(configFile)
    {
        if(OXYGEN_DEBUG)
            fprintf(stderr,"ARGB config file found\n");
        char* progname = g_get_prgname();
        gboolean rgba=FALSE;
        // FIXME: just to make the code trivial i assume config file never exceeds 4KiB
        // If it does, anything beyond 4K will not be taken into account.
        char configBuf[4096];
        fread(configBuf,sizeof(configBuf[0]),sizeof(configBuf),configFile);

        gboolean allbut = FALSE;
        char ** apps = g_strsplit(configBuf,":",-1);
        if (!g_strcmp0(apps[0],"allbut"))
        {
            allbut = TRUE;
            rgba = TRUE;
        };
        int i;
        for (i = 0; apps[i] != NULL;i++)
        {
            if (!g_strcmp0(apps[i],progname))
            {
                rgba = !allbut;
            };
        };
        g_strfreev(apps);

        if(OXYGEN_DEBUG)
            fprintf(stderr,"Program name is %s; ARGB visual is %sused\n",progname,rgba?"":"not ");

        if(rgba)
        {
            GdkColormap* cmap=gdk_screen_get_rgba_colormap(gdk_screen_get_default());
            gtk_widget_push_colormap(cmap);
            gtk_widget_set_default_colormap(cmap);
        }
    }
    else
        if(OXYGEN_DEBUG)
            fprintf(stderr,"ARGB config file not found!\n");
}

//_________________________________________________
G_MODULE_EXPORT void theme_exit( void )
{}

//_________________________________________________
G_MODULE_EXPORT GtkRcStyle* theme_create_rc_style( void )
{ return GTK_RC_STYLE( g_object_new( OXYGEN_TYPE_RC_STYLE, NULL ) ); }

//_________________________________________________
G_MODULE_EXPORT const gchar* g_module_check_init( GModule *module )
{
    return gtk_check_version(
        GTK_MAJOR_VERSION,
        GTK_MINOR_VERSION,
        GTK_MICRO_VERSION - GTK_INTERFACE_AGE );
}
