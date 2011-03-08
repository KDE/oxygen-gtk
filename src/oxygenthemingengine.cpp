/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygenthemingengine.h"

#include "oxygendbus.h"
#include "oxygengtktypenames.h"
#include "oxygengtkutils.h"
#include "oxygenstyle.h"
#include "config.h"

namespace Oxygen
{

    //___________________________________________________________________________________________________________
    GtkThemingEngineClass* ThemingEngine::_parentClass = 0L;
    GTypeInfo ThemingEngine::_typeInfo;
    GType ThemingEngine::_type = 0L;

    //________________________________________________________________________________________________
    void render_line( GtkThemingEngine* engine, cairo_t* context, gdouble x0, gdouble y0, gdouble x1, gdouble y1)
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_line -"
            << " context: " << context
            << " positions: (" << x0 << "," << y0 << ")"
            << " (" << x1 << "," << y1 << ")"
            << std::endl;
        #endif

        // in all other cases, fallback on default rendering, for now
        ThemingEngine::parentClass()->render_line( engine, context, x0, y0, x1, y1 );

    }

    //________________________________________________________________________________________________
    void render_background( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_background -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_background( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_frame( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_frame -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_frame( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_frame_gap(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, gdouble w, gdouble h,
        GtkPositionType gap_side,
        gdouble xy0_gap, gdouble xy1_gap)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_frame -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " side: " << Gtk::TypeNames::position( gap_side )
            << " gap: (" << xy0_gap << "," << xy1_gap << ")"
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_frame_gap( engine, context, x, y, w, h, gap_side, xy0_gap, xy1_gap );

    }

    //________________________________________________________________________________________________
    void render_extension(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, gdouble w, gdouble h,
        GtkPositionType gap_side )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_extension -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " side: " << Gtk::TypeNames::position( gap_side )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_extension( engine, context, x, y, w, h, gap_side );

    }

    //________________________________________________________________________________________________
    void render_check( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_check -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_check( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_option( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_option -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_option( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_arrow(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble angle, gdouble x, gdouble y, gdouble size)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_arrow -"
            << " context: " << context
            << " angle: " << angle
            << " position: (" << x << "," << y << ")"
            << " size: " << size
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_arrow( engine, context, angle, x, y, size );

    }

    //________________________________________________________________________________________________
    void render_expander( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_expander -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_expander( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_focus( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_focus -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_focus( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_layout(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, PangoLayout *layout )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_layout -"
            << " context: " << context
            << " position: (" << x << "," << y << ")"
            << " layout: " << layout
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_layout( engine, context, x, y, layout );

    }

    //________________________________________________________________________________________________
    void render_slider(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, gdouble w, gdouble h,
        GtkOrientation orientation)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_slider -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " orientation: " << Gtk::TypeNames::orientation( orientation )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_slider( engine, context, x, y, w, h, orientation );

    }

    //________________________________________________________________________________________________
    void render_handle( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_handle -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_handle( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    void render_activity(  GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_handle -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << std::endl;
        #endif

        ThemingEngine::parentClass()->render_handle( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    GdkPixbuf* render_icon_pixbuf( GtkThemingEngine *engine, const GtkIconSource *source, GtkIconSize size)
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_handle -"
            << " source: " << source
            << " size: " << size
            << std::endl;
        #endif

        return ThemingEngine::parentClass()->render_icon_pixbuf( engine, source, size );

    }

    //_______________________________________________________________________________________________________________
    void ThemingEngine::instanceInit( OxygenThemingEngine* self )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ThemingEngine::instanceInit" << std::endl;
        #endif

        // hooks
        Style::instance().animations().initializeHooks();
        Style::instance().windowManager().initializeHooks();

        // also initialize dbus
        Oxygen::DBus::instance();

        // initialize argb hooks
        if(
            Style::instance().settings().argbEnabled() &&
            !Style::instance().settings().applicationName().isMozilla() )
        { Style::instance().argbHelper().initializeHooks(); }

    }

    //_______________________________________________________________________________________________________________
    void ThemingEngine::classInit( OxygenThemingEngineClass* klass )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ThemingEngine::classInit" << std::endl;
        #endif

        _parentClass = static_cast<GtkThemingEngineClass*>( g_type_class_peek_parent( klass ) );

        GtkThemingEngineClass* theming_engine_class( GTK_THEMING_ENGINE_CLASS( klass ) );
        theming_engine_class->render_line = render_line;
        theming_engine_class->render_background = render_background;
        theming_engine_class->render_frame = render_frame;
        theming_engine_class->render_frame_gap = render_frame_gap;
        theming_engine_class->render_extension = render_extension;
        theming_engine_class->render_check = render_check;
        theming_engine_class->render_option = render_option;
        theming_engine_class->render_arrow = render_arrow;
        theming_engine_class->render_expander = render_expander;
        theming_engine_class->render_focus = render_focus;
        theming_engine_class->render_layout = render_layout;
        theming_engine_class->render_slider = render_slider;
        theming_engine_class->render_handle = render_handle;
        theming_engine_class->render_activity = render_activity;
        theming_engine_class->render_icon_pixbuf = render_icon_pixbuf;

    }

    //_______________________________________________________________________________________________________________
    void ThemingEngine::registerType( GTypeModule* module )
    {
        if( !_type )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::ThemingEngine::registerType" << std::endl;
            #endif

            const GTypeInfo info =
            {
                (guint16)sizeof( OxygenThemingEngineClass ),
                (GBaseInitFunc) NULL,
                (GBaseFinalizeFunc) NULL,
                (GClassInitFunc) classInit,
                (GClassFinalizeFunc) NULL,
                NULL,
                (guint16)sizeof( OxygenThemingEngine ),
                0,
                (GInstanceInitFunc) instanceInit,
                NULL
            };

            _typeInfo = info;
            _type = g_type_module_register_type( module, GTK_TYPE_THEMING_ENGINE, "OxygenThemingEngine", &_typeInfo, GTypeFlags(0 ) );
        }
    }

    //_______________________________________________________________________________________________________________
    GType ThemingEngine::type( void )
    { return _type; }

}
