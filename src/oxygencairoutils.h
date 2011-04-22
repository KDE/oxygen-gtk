#ifndef oxygencairoutils_h
#define oxygencairoutils_h
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

#include "oxygencairopattern.h"
#include "oxygenflags.h"
#include "oxygengeometry.h"
#include "oxygenrgba.h"

#include <cairo.h>
#include <gdk/gdk.h>
#include <vector>

namespace Oxygen
{

    //! draw arc with the parameters similar to those of QPainter::drawArc() (but using diameter instead of width&height). Also, angles are specified in degrees, not in 16ths of degrees
    void cairo_arc_qt( cairo_t*, double, double, double, double, double );

    //! convenience class for cairo pattern color stops
    class ColorStop
    {
        public:

        //! empty constructor
        ColorStop( double x = 0, ColorUtils::Rgba color = ColorUtils::Rgba() ):
            _x( x ),
            _color( color )
        {}

        //! offset
        double _x;

        //! color
        ColorUtils::Rgba _color;

        typedef std::vector<ColorStop> List;

    };

    //!@name color handling
    //@{

    //! add color to pattern
    void cairo_pattern_add_color_stop( cairo_pattern_t*, double, const ColorUtils::Rgba& );

    //! add color to pattern
    inline void cairo_pattern_add_color_stop( cairo_pattern_t* pattern, const ColorStop& colorStop )
    { Oxygen::cairo_pattern_add_color_stop( pattern, colorStop._x, colorStop._color ); }

    //! get color stops
    ColorStop::List cairo_pattern_get_color_stops( cairo_pattern_t* );

    //! set source from pattern
    inline void cairo_set_source( cairo_t* context, const Cairo::Pattern& pattern )
    { ::cairo_set_source( context, (cairo_pattern_t*)( pattern ) ); }

    //! set source from color
    void cairo_set_source( cairo_t*, const ColorUtils::Rgba& );

    //@}

    //!@name patterns
    //@{

    inline cairo_pattern_t* cairo_pattern_create_radial( double x, double y, double r )
    { return ::cairo_pattern_create_radial( x, y, 0, x, y, r ); }

    //@}

    //!@name path
    //@{

    enum Corner
    {
        CornersNone = 0,
        CornersTopLeft = 1<<0,
        CornersTopRight = 1<<1,
        CornersBottomLeft = 1<<2,
        CornersBottomRight = 1<<3,
        CornersTop = CornersTopLeft|CornersTopRight,
        CornersBottom = CornersBottomLeft|CornersBottomRight,
        CornersLeft = CornersTopLeft|CornersBottomLeft,
        CornersRight = CornersTopRight|CornersBottomRight,
        CornersAll = CornersTop|CornersBottom
    };

    OX_DECLARE_FLAGS( Corners, Corner );
    OX_DECLARE_OPERATORS_FOR_FLAGS( Corners );

    //! rounded rectangle
    void cairo_rounded_rectangle( cairo_t*, double x, double y, double width, double height, double radius, Corners corners = CornersAll );

    //! rounded rectangle
    void cairo_rounded_rectangle_negative( cairo_t*, double x, double y, double width, double height, double radius, Corners corners = CornersAll );

    //! negative rectangle
    void cairo_rectangle_negative( cairo_t*, double x, double y, double width, double height );

    //! ellipse
    void cairo_ellipse( cairo_t*, double x, double y, double width, double height );

    //! ellipse
    void cairo_ellipse_negative( cairo_t*, double x, double y, double width, double height );

    //! polygon
    void cairo_polygon( cairo_t*, const Polygon& );

    //@}

    //!@name gdk path
    //@{
    //! rounded rectangle
    inline void gdk_cairo_rounded_rectangle( cairo_t* context, GdkRectangle* rect, double radius, Corners corners = CornersAll )
    { cairo_rounded_rectangle( context, rect->x, rect->y, rect->width, rect->height, radius, corners ); }

    //! rounded rectangle
    inline void gdk_cairo_rounded_rectangle_negative( cairo_t* context, GdkRectangle* rect, double radius, Corners corners = CornersAll )
    { cairo_rounded_rectangle_negative( context, rect->x, rect->y, rect->width, rect->height, radius, corners ); }

    //! ellipse
    inline void gdk_cairo_ellipse( cairo_t* context, GdkRectangle* rect )
    { cairo_ellipse( context, rect->x, rect->y, rect->width, rect->height ); }

    //! ellipse
    inline void gdk_cairo_ellipse_negative( cairo_t* context, GdkRectangle* rect )
    { cairo_ellipse_negative( context, rect->x, rect->y, rect->width, rect->height ); }

    //@}

    //!@name surfaces
    //@{

    //! get width for surface
    int cairo_surface_get_width( cairo_surface_t* );

    //! get height for surface
    int cairo_surface_get_height( cairo_surface_t* );

    //! deep copy
    cairo_surface_t* cairo_surface_copy( cairo_surface_t* );

    //! alpha channel
    void cairo_surface_add_alpha( cairo_surface_t*, double );

    //! saturation
    /*! Inspired from gdk-pixbuf-util.c
    Copyright (C) 1999 The Free Software Foundation
    Authors: Federico Mena-Quintero <federico@gimp.org>
             Cody Russell  <bratsche@gnome.org>
    */
    void cairo_image_surface_saturate( cairo_surface_t*, double );

    //@}

}

#endif
