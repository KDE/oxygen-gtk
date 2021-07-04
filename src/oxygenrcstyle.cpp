/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>

    based on the 0L Theme Engine for Gtk+.
    SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygenrcstyle.h"
#include "oxygenstyle.h"
#include "oxygenstylewrapper.h"

#include "config.h"

namespace Oxygen
{

    //______________________________________________________________________
    GType RCStyle::_type = 0L;
    GTypeInfo RCStyle::_typeInfo;
    GtkRcStyleClass* RCStyle::_parentClass = 0L;

    //______________________________________________________________________
    GtkStyle* RCStyle::createStyle( GtkRcStyle* rc_style )
    { return GTK_STYLE( g_object_new( StyleWrapper::type(), 0L ) ); }

    //______________________________________________________________________
    guint RCStyle::parse(
        GtkRcStyle* rc_style,
        GtkSettings* settings,
        GScanner* scanner )
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
    void RCStyle::merge( GtkRcStyle *dst, GtkRcStyle *src )
    {
        if( RCStyle::_parentClass )
        { RCStyle::_parentClass->merge( dst, src ); }
    }

    //______________________________________________________________________
    void RCStyle::classInit( OxygenRcStyleClass *klass )
    {

        GtkRcStyleClass *rcStyleClass( GTK_RC_STYLE_CLASS( klass ) );

        _parentClass = static_cast<GtkRcStyleClass*>(g_type_class_peek_parent( klass ) );

        rcStyleClass->create_style = createStyle;
        rcStyleClass->parse = parse;
        rcStyleClass->merge = merge;
    }

    //______________________________________________________________________
    void RCStyle::registerType( GTypeModule *module )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::RCStyle::registerType" << std::endl;
        #endif

        const GTypeInfo info =
        {
            (guint16)sizeof(OxygenRcStyleClass ),
            (GBaseInitFunc) NULL,
            (GBaseFinalizeFunc) NULL,
            (GClassInitFunc) classInit,
            (GClassFinalizeFunc) NULL,
            NULL,
            (guint16)sizeof( OxygenRcStyle ),
            0,
            (GInstanceInitFunc) NULL,
            NULL
        };

        _typeInfo = info;
        _type = g_type_module_register_type( module, GTK_TYPE_RC_STYLE, "OxygenRcStyle", &_typeInfo, GTypeFlags(0 ) );

    }

    //______________________________________________________________________
    GType RCStyle::type( void )
    { return _type; }

}
