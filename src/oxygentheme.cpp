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

#include "config.h"
#include "oxygenstyle.h"
#include "oxygenthemingengine.h"
#include "oxygenwindecooptions.h"
#include "oxygenwindowshadow.h"

#include <gmodule.h>
#include <gtk/gtk.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>

//_________________________________________________
extern "C" G_MODULE_EXPORT void theme_init( GTypeModule* );
extern "C" G_MODULE_EXPORT void theme_exit( void );

extern "C" G_MODULE_EXPORT GtkThemingEngine* create_engine( void );

extern "C" G_MODULE_EXPORT const gchar* g_module_check_init( GModule* );

// exports for WM theming
extern "C" G_MODULE_EXPORT void drawWindowDecoration(cairo_t*, unsigned long,gint,gint,gint,gint,gchar**,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindecoButton(cairo_t*, unsigned long,unsigned long, unsigned long,gint,gint,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindecoShapeMask(cairo_t*, unsigned long,gint,gint,gint,gint);
extern "C" G_MODULE_EXPORT void drawWindowShadow(cairo_t*, unsigned long options, gint x, gint y, gint w, gint h);
extern "C" G_MODULE_EXPORT gint getWindecoMetric(unsigned long);
extern "C" G_MODULE_EXPORT gint getWindecoButtonSize(unsigned long);
extern "C" G_MODULE_EXPORT unsigned long getWindecoABIVersion(void);

//_________________________________________________
void theme_init( GTypeModule* module )
{

    #if OXYGEN_DEBUG
    std::cerr << "Oxygen::theme_init" << std::endl;
    #endif

    Oxygen::ThemingEngine::registerType( module );

    if( Oxygen::Style::instance().settings().applicationName().isOpenOffice() )
    { Oxygen::Style::instance().animations().setEnabled( false ); }

    Oxygen::Style::instance().settings().initializeHooks();
    Oxygen::Style::instance().windowManager().initializeHooks();
    Oxygen::Style::instance().widgetLookup().initializeHooks();

}

//_________________________________________________
GtkThemingEngine* create_engine( void )
{

    #if OXYGEN_DEBUG
    std::cerr << "Oxygen::create_engine" << std::endl;
    #endif

    return GTK_THEMING_ENGINE( g_object_new( Oxygen::ThemingEngine::type(), 0L) );

}

//_________________________________________________
void theme_exit( void )
{
    #if OXYGEN_DEBUG
    std::cerr << "Oxygen::theme_exit" << std::endl;
    #endif
}

//_________________________________________________
const gchar* g_module_check_init( GModule *module )
{
    return gtk_check_version(
        GTK_MAJOR_VERSION,
        GTK_MINOR_VERSION,
        GTK_MICRO_VERSION - GTK_INTERFACE_AGE );
}

//_________________________________________________
void drawWindowDecoration(cairo_t* context, unsigned long options, gint x,gint y,gint w,gint h, gchar** ws, gint til, gint tir)
{
    Oxygen::Style::instance().drawWindowDecoration( context, (Oxygen::WinDeco::Options) options, x, y, w, h, ws, til, tir);
}

//_________________________________________________
void drawWindecoButton(cairo_t* context, unsigned long buttonType,unsigned long buttonState, unsigned long windowState, gint x,gint y,gint w,gint h)
{
    Oxygen::Style::instance().drawWindecoButton(
        context, (Oxygen::WinDeco::ButtonType)buttonType,
        (Oxygen::WinDeco::ButtonStatus)buttonState, (Oxygen::WinDeco::Options) windowState, x, y, w, h);
}

//_________________________________________________
void drawWindecoShapeMask(cairo_t* context, unsigned long options, gint x,gint y,gint w,gint h)
{
    Oxygen::Style::instance().drawWindecoShapeMask( context, (Oxygen::WinDeco::Options) options, x, y, w, h);
}

//_________________________________________________
void drawWindowShadow(cairo_t* context, unsigned long options, gint x, gint y, gint w, gint h)
{
    Oxygen::Style::instance().drawWindowShadow(context,(Oxygen::WinDeco::Options) options, x, y, w, h);
}

//_________________________________________________
gint getWindecoMetric(unsigned long wm)
{
    return Oxygen::WinDeco::getMetric((Oxygen::WinDeco::Metric)wm);
}

//_________________________________________________
gint getWindecoButtonSize(unsigned long buttonType)
{
    return Oxygen::WinDeco::getButtonSize();
}

//_________________________________________________
unsigned long getWindecoABIVersion(void)
{
    return 0x3;
}

