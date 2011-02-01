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
#include "oxygenrgba.h"
#include "oxygencairoutils.h"
#include "oxygencolorutils.h"
#include "oxygenqtsettings.h"
#include "oxygenstylehelper.h"
#include <cmath>

namespace Oxygen
{

    //________________________________________________________________________________
    void WinDeco::Button::render( cairo_t* context, gint x, gint y, gint w, gint h ) const
    {
        if( _type==ButtonMenu )
        {
            // menu button isn't drawn - an app icon should be drawn instead
            return;
        }

        cairo_save( context );
        cairo_translate( context, x, y );

        const ColorUtils::Rgba base( settings().palette().color( _state == Disabled ? Palette::Disabled : Palette::Active, Palette::Window ) );
        ColorUtils::Rgba shadow( settings().palette().color( Palette::WindowText ) );
        ColorUtils::Rgba icon(shadow);
        const double scale( (21.0*settings().buttonSize())/22.0 );

        // draw shadow
        if(_state==Hovered)
        {
            shadow=(_type==ButtonClose)?
                ColorUtils::Rgba( settings().palette().color( Palette::NegativeText ) ):
                ColorUtils::Rgba( settings().palette().color( Palette::Hover ) );
            shadow=darkColor(shadow);
            icon=shadow;
        } else {
            shadow=ColorUtils::Rgba(0,0,0);
        }

        const Cairo::Surface& windecoButtonGlow( helper().windecoButtonGlow( shadow, int(scale) ) );
        cairo_set_source_surface( context, windecoButtonGlow, 0, 0 );
        cairo_rectangle( context, 0, 0, w, h );
        cairo_fill( context );

        // draw button slab
        const bool pressed( _state == Pressed || _type==ButtonUnstick || _type==ButtonUndoAbove || _type==ButtonUndoBelow );
        const Cairo::Surface& windecoButton( helper().windecoButton( base, pressed, int(scale) ) );
        cairo_set_source_surface( context, windecoButton, 0, 0 );
        cairo_rectangle( context, 0, 0, w, h );
        cairo_fill( context );

        // draw icon
        cairo_set_line_width( context, 1.2 );
        cairo_set_line_cap( context, CAIRO_LINE_CAP_ROUND );
        cairo_set_line_join( context, CAIRO_LINE_JOIN_ROUND );

        cairo_scale( context, double(w)/22.0, double(h)/22.0 );
        Oxygen::cairo_set_source( context, ColorUtils::lightColor( base ) );
        drawIcon( context, w, h );

        cairo_translate( context, 0, -1.5 );

        if(_state==Disabled)
        {
            icon=ColorUtils::Rgba( settings().palette().color( Palette::Disabled, Palette::WindowText ) );
        }
        cairo_set_source( context, icon );

        drawIcon( context, w, h );
        cairo_restore( context );

    }

    //________________________________________________________________________________
    void WinDeco::Button::drawIcon( cairo_t* context, int w, int h ) const
    {

        switch( _type )
        {
            case ButtonStick:
            case ButtonUnstick:
            cairo_move_to( context, 10.5, 10.5 );
            cairo_close_path( context );
            cairo_stroke( context );
            break;

            case ButtonHelp:
            cairo_save( context );
            cairo_translate( context, 1.5,1.5 );

            cairo_arc_qt( context, 7,5,4, 135.0*M_PI/180, -M_PI );
            cairo_stroke(context);

            cairo_arc_qt( context, 9,8,4, 135*M_PI/180, M_PI/4 );
            cairo_move_to( context, 9,12 );
            cairo_close_path( context );
            cairo_stroke( context );

            cairo_restore( context );
            break;

            case ButtonMin:
            cairo_move_to( context, 7.5, 9.5 );
            cairo_line_to( context, 10.5, 12.5 );
            cairo_line_to( context, 13.5, 9.5 );
            cairo_stroke( context );
            break;

            case ButtonMax:
            cairo_move_to( context, 7.5, 11.5 );
            cairo_line_to( context, 10.5, 8.5 );
            cairo_line_to( context, 13.5, 11.5 );
            cairo_stroke( context );
            break;

            case ButtonUnmax:
            cairo_save( context );
            cairo_translate( context, 1.5,1.5 );
            cairo_move_to( context, 9, 6 );
            cairo_line_to( context, 12, 9 );
            cairo_line_to( context, 9, 12 );
            cairo_line_to( context, 6, 9 );
            cairo_line_to( context, 9, 6 );
            cairo_stroke( context );
            cairo_restore( context );
            break;

            case ButtonClose:
            cairo_move_to( context, 7.5, 7.5 ); cairo_line_to( context, 13.5, 13.5 );
            cairo_move_to( context, 13.5, 7.5 ); cairo_line_to( context, 7.5, 13.5 );
            cairo_stroke( context );
            break;

            case ButtonAbove:
            case ButtonUndoAbove:
            cairo_move_to( context, 7.5, 14 );
            cairo_line_to( context, 10.5, 11 );
            cairo_line_to( context, 13.5, 14 );

            cairo_move_to( context, 7.5, 10 );
            cairo_line_to( context, 10.5, 7 );
            cairo_line_to( context, 13.5, 10 );
            cairo_stroke( context );
            break;

            case ButtonBelow:
            case ButtonUndoBelow:
            cairo_move_to( context, 7.5, 11 );
            cairo_line_to( context, 10.5, 14 );
            cairo_line_to( context, 13.5, 11 );

            cairo_move_to( context, 7.5, 7 );
            cairo_line_to( context, 10.5, 10 );
            cairo_line_to( context, 13.5, 7 );
            cairo_stroke( context );
            break;

            case ButtonShade:
            cairo_move_to( context, 7.5, 7.5 );
            cairo_line_to( context, 10.5, 10.5 );
            cairo_line_to( context, 13.5, 7.5 );

            cairo_move_to( context, 7.5, 13.0 );
            cairo_line_to( context, 13.5, 13.0 );
            cairo_stroke( context );
            break;

            case ButtonUnshade:
            cairo_move_to( context, 7.5, 10.5 );
            cairo_line_to( context, 10.5, 7.5 );
            cairo_line_to( context, 13.5, 10.5 );

            cairo_move_to( context, 7.5, 13 );
            cairo_line_to( context, 13.5, 13 );
            cairo_stroke( context );
            break;

            default:
            break;
        }

    }

}
