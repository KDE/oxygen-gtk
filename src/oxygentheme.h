#ifndef oxygentheme_h
#define oxygentheme_h

/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include <gtk/gtk.h>

//!@name external 'c' methods, expected by gtk
//@{
extern "C" G_MODULE_EXPORT void theme_init( GTypeModule* );
extern "C" G_MODULE_EXPORT void theme_exit( void );
extern "C" G_MODULE_EXPORT GtkRcStyle* theme_create_rc_style( void );
extern "C" G_MODULE_EXPORT const gchar* g_module_check_init( GModule* );

// exports for WM theming
extern "C" G_MODULE_EXPORT void drawWindowDecoration(cairo_t*, unsigned long,gint,gint,gint,gint,const gchar**,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindecoButton(cairo_t*, unsigned long,unsigned long, unsigned long,gint,gint,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindecoShapeMask(cairo_t*, unsigned long,gint,gint,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindowShadow(cairo_t*, unsigned long options, gint x, gint y, gint w, gint h);
extern "C" G_MODULE_EXPORT gint getWindecoMetric(unsigned long);
extern "C" G_MODULE_EXPORT gint getWindecoButtonSize(unsigned long);
extern "C" G_MODULE_EXPORT unsigned long getWindecoABIVersion(void);
//@}

#endif
