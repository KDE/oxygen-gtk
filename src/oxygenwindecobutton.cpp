/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygenwindecobutton.h"
#include "oxygenqtsettings.h"
#include "oxygenstylehelper.h"

namespace Oxygen
{

    //________________________________________________________________________________
    void WinDeco::Button::render( Cairo::Context& context, gint x, gint y, gint w, gint h ) const
    {

        cairo_save( context );
        cairo_translate( context, x, y );

        const ColorUtils::Rgba base( settings().palette().color( _state == Disabled ? Palette::Disabled : Palette::Active, Palette::Window ) );
        const ColorUtils::Rgba shadow( ColorUtils::Rgba::black() );
        const double scale( (21.0*settings().buttonSize())/22.0 );

        // draw shadow
        GdkPixbuf *windecoButtonGlow( helper().windecoButtonGlow( shadow, int(scale) ) );
        gdk_cairo_set_source_pixbuf( context, windecoButtonGlow, 0, 0 );
        cairo_rectangle( context, 0, 0, w, h );
        cairo_fill( context );

        const bool pressed( _state == Pressed );
        GdkPixbuf* windecoButton( helper().windecoButton( base, pressed, int(scale) ) );
        gdk_cairo_set_source_pixbuf( context, windecoButton, 0, 0 );
        cairo_rectangle( context, 0, 0, w, h );
        cairo_fill( context );

        cairo_restore( context );

    }

}
