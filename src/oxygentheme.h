#ifndef oxygentheme_h
#define oxygentheme_h

/*
* this file is part of the oxygen gtk engine
* SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
* SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* based on the Null Theme Engine for Gtk+.
* SPDX-FileCopyrightText: 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
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
