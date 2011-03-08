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

#include "config.h"
#include "oxygenthemingengine.h"
#include "oxygendbus.h"
#include "oxygenstyle.h"

#define OXYGEN_DEBUG 1

namespace Oxygen
{

    //___________________________________________________________________________________________________________
    GtkThemingEngineClass* ThemingEngine::_parentClass = 0L;
    GTypeInfo ThemingEngine::_typeInfo;
    GType ThemingEngine::_type = 0L;


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

        GtkThemingEngineClass* theming_engine_class( GTK_THEMING_ENGINE_CLASS( klass ) );

        _parentClass = static_cast<GtkThemingEngineClass*>( g_type_class_peek_parent( klass ) );

//         theming_engine_class->draw_hline = draw_hline;
//         theming_engine_class->draw_vline = draw_vline;
//         theming_engine_class->draw_shadow = draw_shadow;
//         theming_engine_class->draw_arrow = draw_arrow;
//
//         theming_engine_class->draw_diamond = draw_diamond;
//
//         theming_engine_class->draw_box = draw_box;
//         theming_engine_class->draw_flat_box = draw_flat_box;
//         theming_engine_class->draw_check = draw_check;
//         theming_engine_class->draw_option = draw_option;
//         theming_engine_class->draw_tab = draw_tab;
//         theming_engine_class->draw_shadow_gap = draw_shadow_gap;
//         theming_engine_class->draw_box_gap = draw_box_gap;
//         theming_engine_class->draw_extension = draw_extension;
//         theming_engine_class->draw_focus = draw_focus;
//         theming_engine_class->draw_slider = draw_slider;
//         theming_engine_class->draw_handle = draw_handle;
//         theming_engine_class->draw_resize_grip = draw_resize_grip;
//         theming_engine_class->draw_expander = draw_expander;
//
//         // icon rendering
//         theming_engine_class->render_icon = render_icon;
//
//         // text rendering
//         theming_engine_class->draw_layout = draw_layout;

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
