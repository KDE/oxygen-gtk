/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
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

#include "oxygencairoutils.h"
#include "oxygenrgba.h"

#include <cmath>
namespace Oxygen
{

    //__________________________________________________________________
    void cairo_arc_qt(cairo_t* context, double x, double y, double diam, double a, double alen)
    {

        const double xc( x+diam/2 );
        const double yc( y+diam/2 );
        const double radius( diam/2 );

        double angle1( -a );
        double angle2( -(alen+a) );

        if( angle1 > angle2 ) std::swap( angle1, angle2 );
        cairo_arc(context, xc,yc,radius,angle1,angle2);

    }

    //__________________________________________________________________
    void cairo_pattern_add_color_stop( cairo_pattern_t* pattern, double x, const ColorUtils::Rgba& color )
    {
        /*
        for some reason passing 1.0 for the upperbound on some gradients
        breaks the gradient definition internally with some compilers
        this is fixed by passing a value close to (but smaller than) unity.
        */
        cairo_pattern_add_color_stop_rgba( pattern, x, color.red(), color.green(), color.blue(), color.alpha() );
    }

    //__________________________________________________________________
    void cairo_set_source( cairo_t* context, const ColorUtils::Rgba& color )
    { cairo_set_source_rgba( context, color.red(), color.green(), color.blue(), color.alpha() ); }

    //__________________________________________________________________
    void cairo_rounded_rectangle( cairo_t* context, double x, double y, double w, double h, double r, Corners corners )
    {

        if( corners == CornersNone )
        {
            cairo_rectangle( context, x, y, w, h );
            return;
        }

        if( corners & CornersTopLeft )
        {
            cairo_move_to( context, x, y+r );
            cairo_arc( context, x+r, y+r, r, M_PI, 3.0*M_PI/2 );
        } else cairo_move_to( context, x, y );

        if( corners & CornersTopRight )
        {
            cairo_line_to( context, x+w-r, y );
            cairo_arc( context, x+w-r, y+r, r, -M_PI/2, 0 );
        } else cairo_line_to( context, x+w, y );

        if( corners & CornersBottomRight )
        {
            cairo_line_to( context, x+w, y+h-r );
            cairo_arc( context, x+w-r, y+h-r, r, 0, M_PI/2 );
        } else cairo_line_to( context, x+w, y+h );

        if( corners & CornersBottomLeft )
        {
            cairo_line_to( context, x+r, y+h );
            cairo_arc( context, x+r, y+h-r, r, M_PI/2, M_PI );
        } else cairo_line_to( context, x, y+h );

        cairo_close_path( context );

    }

    //__________________________________________________________________
    void cairo_rounded_rectangle_negative( cairo_t* context, double x, double y, double w, double h, double r, Corners corners )
    {

        if( corners & CornersTopRight )
        {
            cairo_move_to( context, x+w, y+r );
            cairo_arc_negative( context, x+w-r, y+r, r, 0, -M_PI/2 );
        } else cairo_move_to( context, x+w, y );

        if( corners & CornersTopLeft )
        {
            cairo_line_to( context, x+r, y );
            cairo_arc_negative( context, x+r, y+r, r, -M_PI/2, -M_PI );
        } else cairo_line_to( context, x, y );

        if( corners & CornersBottomLeft )
        {
            cairo_line_to( context, x, y+h-r );
            cairo_arc_negative( context, x+r, y+h-r, r, -M_PI, -3.0*M_PI/2 );
        } else cairo_line_to( context, x, y+h );

        if( corners & CornersBottomRight )
        {
            cairo_line_to( context, x+w-r, y+h );
            cairo_arc_negative( context, x+w-r, y+h-r, r, M_PI/2, 0 );
        } else cairo_line_to( context, x+w, y+h );

        cairo_close_path( context );

    }

    //__________________________________________________________________
    void cairo_ellipse( cairo_t* context, double x, double y, double w, double h )
    {
        cairo_save( context );
        cairo_translate( context, x+w/2, y+h/2 );
        cairo_scale( context, w/2, h/2 );
        cairo_arc( context, 0, 0, 1, 0, 2.0*M_PI);
        cairo_restore( context );
    }

    //___________________________________________________________
    void cairo_ellipse_negative( cairo_t* context, double x, double y, double w, double h )
    {
        cairo_save( context );
        cairo_translate( context, x+w/2, y+h/2 );
        cairo_scale( context, w/2, h/2 );
        cairo_arc_negative( context, 0, 0, 1, 2.0*M_PI, 0);
        cairo_restore( context );
    }

    //__________________________________________________________________
    void cairo_polygon( cairo_t* context, const Polygon& polygon )
    {
        for( Polygon::const_iterator iter = polygon.begin(); iter != polygon.end(); ++iter )
        {
            if( iter == polygon.begin() ) cairo_move_to( context, iter->x(), iter->y() );
            else cairo_line_to( context, iter->x(), iter->y() );
        }
    }

}
