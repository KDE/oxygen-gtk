/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
*
* based on the 0L Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "oxygenrcstyle.h"
#include "oxygenstyle.h"
#include "oxygenstylewrapper.h"

#include <gtk/gtk.h>

//______________________________________________________________________
struct _OxygenRcStyle
{ GtkRcStyle parent; };

//______________________________________________________________________
struct _OxygenRcStyleClass
{ GtkRcStyleClass parent; };

static GtkRcStyleClass *oxygen_rc_style_parent_class = 0L;

//______________________________________________________________________
static GtkStyle* create_style( GtkRcStyle *rc_style )
{ return GTK_STYLE( g_object_new( OXYGEN_TYPE_STYLE, 0L ) ); }

//______________________________________________________________________
static guint parse(
    GtkRcStyle *rc_style,
    GtkSettings *settings,
    GScanner *scanner )
{
    static GQuark scope_id = 0;
    guint old_scope;
    guint token;

    if( !scope_id )
    { scope_id = g_quark_from_string( "oxygen_engine" ); }

    old_scope = g_scanner_set_scope( scanner, scope_id );

    token = g_scanner_peek_next_token( scanner );
    while( token != G_TOKEN_RIGHT_CURLY )
    {
        token = g_scanner_peek_next_token( scanner );
        if( token != G_TOKEN_NONE ) return token;
    }

    g_scanner_get_next_token( scanner );
    g_scanner_set_scope( scanner, old_scope );

    return G_TOKEN_NONE;
}

//______________________________________________________________________
static void merge( GtkRcStyle *dst, GtkRcStyle *src )
{ oxygen_rc_style_parent_class->merge( dst, src ); }

//______________________________________________________________________
extern "C" void oxygen_rc_style_instance_init( OxygenRcStyle *self )
{}

//______________________________________________________________________
extern "C" void oxygen_rc_style_class_init( OxygenRcStyleClass *klass )
{
    GtkRcStyleClass *rc_style_class = GTK_RC_STYLE_CLASS( klass );

    oxygen_rc_style_parent_class = static_cast<GtkRcStyleClass*>(g_type_class_peek_parent( klass ) );

    rc_style_class->create_style = create_style;
    rc_style_class->parse = parse;
    rc_style_class->merge = merge;
}

//______________________________________________________________________
static GType oxygen_rc_style_type = 0;
void oxygen_rc_style_register_type( GTypeModule *module )
{

    if( !oxygen_rc_style_type )
    {
        static const GTypeInfo info =
        {
            sizeof(OxygenRcStyleClass ),
            0L,
            0L,
            (GClassInitFunc) oxygen_rc_style_class_init,
            0L,    /* class_finalize */
            0L,    /* class_data */
            sizeof( OxygenRcStyle ),
            0,    /* n_preallocs */
            (GInstanceInitFunc) oxygen_rc_style_instance_init,
            0L
        };

        oxygen_rc_style_type = g_type_module_register_type( module, GTK_TYPE_RC_STYLE, "OxygenRcStyle", &info, GTypeFlags(0 ) );
    }

}

//______________________________________________________________________
GType oxygen_rc_style_get_type( void )
{ return oxygen_rc_style_type; }
