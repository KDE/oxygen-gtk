#ifndef oxygenstylewrapper_h
#define oxygenstylewrapper_h
/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define OXYGEN_TYPE_STYLE ( oxygen_style_get_type() )
#define OXYGEN_STYLE(object) ( G_TYPE_CHECK_INSTANCE_CAST(( object ), OXYGEN_TYPE_STYLE, OxygenStyle ) )
#define OXYGEN_STYLE_CLASS(klass) ( G_TYPE_CHECK_CLASS_CAST(( klass ), OXYGEN_TYPE_STYLE, OxygenStyleClass ) )
#define OXYGEN_IS_STYLE(object) ( G_TYPE_CHECK_INSTANCE_TYPE(( object ), OXYGEN_TYPE_STYLE ) )
#define OXYGEN_IS_STYLE_CLASS(klass) ( G_TYPE_CHECK_CLASS_TYPE(( klass ), OXYGEN_TYPE_STYLE ) )
#define OXYGEN_STYLE_GET_CLASS(obj) ( G_TYPE_INSTANCE_GET_CLASS(( obj ), OXYGEN_TYPE_STYLE, OxygenStyleClass ) )

typedef struct _OxygenStyle OxygenStyle;
typedef struct _OxygenStyleClass OxygenStyleClass;

void  oxygen_style_register_type( GTypeModule *module );
GType oxygen_style_get_type( void ) G_GNUC_CONST;

G_END_DECLS

#endif
