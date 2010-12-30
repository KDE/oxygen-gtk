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

#include "oxygenstyle.h"
#include "oxygencairocontext.h"
#include "oxygencairoutils.h"
#include "oxygencolorutils.h"
#include "oxygengtkutils.h"
#include "oxygenwindecobutton.h"

#include <algorithm>
#include <cmath>

namespace Oxygen
{

    //__________________________________________________________________
    Style* Style::_instance = 0;
    Style& Style::instance( void )
    {
        if( !_instance )
        {
            _instance = new Style();
            _instance->initialize();
        }

        return *_instance;
    }

    //__________________________________________________________________
    Style::Style( void )
    {}

    //__________________________________________________________________
    void Style::initialize( void )
    {
        _settings.initialize();

        // pass window drag mode to window manager
        if( !settings().windowDragEnabled() ) windowManager().setMode( WindowManager::Disabled );
        else if( settings().windowDragMode() == QtSettings::WD_MINIMAL ) windowManager().setMode( WindowManager::Minimal );
        else windowManager().setMode( WindowManager::Full );

        // pass drag distance and delay to window manager
        windowManager().setDragDistance( settings().startDragDist() );
        windowManager().setDragDelay( settings().startDragTime() );

    }

    //__________________________________________________________________
    void Style::fill( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& color ) const
    {

        // define colors
        Cairo::Context context( window, clipRect );
        cairo_rectangle( context, x, y, w, h );
        cairo_set_source( context, color );
        cairo_fill( context );

    }

    //__________________________________________________________________
    void Style::drawSeparator( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, StyleOptions options )
    {

        // define colors
        ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        if( options & Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            if( wh > 0 )
            {
                if( options & Menu ) base = ColorUtils::menuBackgroundColor( settings().palette().color( Palette::Window ), wh, y+wy+h/2 );
                else base = ColorUtils::backgroundColor( settings().palette().color( Palette::Window ), wh, y+wy+h/2 );
            }

        }

        Cairo::Context context( window, clipRect );
        helper().drawSeparator( context, base, x, y, w, h, (options&Vertical) );

    }

    //__________________________________________________________________
    bool Style::renderWindowBackground( cairo_t* context, GdkWindow* window, GtkWidget* widget, GdkRectangle* clipRect, gint x, gint y, gint w, gint h ) const
    {

        // TODO: render custom color for widgets with modify_bg set (like white viewport bin in ccsm)

        bool needToDestroyContext;

        // no background for gnome applets
        if( widget )
        {
            // external applets
            if( Gtk::gtk_widget_is_panel_applet( widget ) )
            { return false; }

            // internal applets
            GtkWidget *parent = gtk_widget_get_parent( widget );
            if( parent && Gtk::gtk_widget_is_panel_applet( parent ) )
            { return false; }
        }

        // define colors
        ColorUtils::Rgba base(settings().palette().color( Palette::Window ) );

        // the hard-coded metrics are copied for
        // kdebase/workspace/libs/oxygen/oxygenhelper.cpp
        // vertical shift to account for window decoration
        const int yShift = 23;

        gint ww, wh;
        gint wx, wy;

        // if we aren't going to draw window decorations...
        if(!context)
        {
            // draw flat background for OpenOffice
            if( Style::instance().settings().applicationName().isOpenOffice() )
            {
                Cairo::Context context(window,clipRect);
                cairo_set_source(context,base);
                cairo_rectangle(context,x,y,w,h);
                cairo_fill(context);
                return true;
            }


            // get window dimension and position
            if( !Gtk::gdk_map_to_toplevel( window, widget, &wx, &wy, &ww, &wh, true ) )
            { return false; }

            wy += yShift;

            // translate to toplevel coordinates
            x+=wx;
            y+=wy;

            // create context and translate to toplevel coordinates
            // make it the old good way since context is cairo_t* instead Cairo::Context
            context=gdk_cairo_create(window);
            needToDestroyContext=true;
            if(clipRect)
            {
                cairo_rectangle(context,clipRect->x,clipRect->y,clipRect->width,clipRect->height);
                cairo_clip(context);
            }
            // no sense in context saving since it will be destroyed
            cairo_translate( context, -wx, -wy );
        }
        else
        {
            // drawing window decorations, so logic is simplified
            ww=w;
            wh=h;
            needToDestroyContext=false;
            cairo_save(context);
            cairo_translate(context,x,y);
            x=0;
            y=0;
            // wx and wy aren't set since clipRect will be 0L if we are here, and wx and wy are only used with clipRect
        }

        // split
        const int splitY( std::min(300, 3*wh/4 ) );

        // store rectangle
        GdkRectangle rect = { x, y, w, h };

        /*
        if there is a valid clipRect,
        intersects it with painting Rect, for performances
        */
        if( clipRect )
        {

            GdkRectangle localClip( *clipRect );
            localClip.x += wx;
            localClip.y += wy;
            gdk_rectangle_intersect( &rect, &localClip, &rect );

        }

        // upper rect
        GdkRectangle upperRect = { 0, 0, ww, splitY };
        if( gdk_rectangle_intersect( &rect, &upperRect, &upperRect ) )
        {

            Cairo::Pattern pattern( verticalGradient( base, 0, splitY ) );

            gdk_cairo_rectangle( context, &upperRect );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        // fill lower rect
        GdkRectangle lowerRect = { 0, splitY, ww, wh - splitY + yShift };
        if( gdk_rectangle_intersect( &rect, &lowerRect, &lowerRect ) )
        {

            ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );
            gdk_cairo_rectangle( context, &lowerRect );
            cairo_set_source( context, bottom );
            cairo_fill( context );

        }

        // radial pattern
        const int patternHeight = 64;
        const int radialW( std::min(600, ww ) );

        GdkRectangle radialRect = {  (ww - radialW)/2, 0, radialW, patternHeight };
        if( gdk_rectangle_intersect( &rect, &radialRect, &radialRect ) )
        {

            // get pattern
            Cairo::Pattern pattern( radialGradient( base, 64, patternHeight - 64, 64 ) );

            // add matrix transformation
            cairo_matrix_t transformation;
            cairo_matrix_init_identity( &transformation );
            cairo_matrix_scale( &transformation, 128.0/radialW, 1.0 );
            cairo_matrix_translate( &transformation, -(ww - radialW)/2, 0 );
            cairo_pattern_set_matrix( pattern, &transformation );

            // fill
            gdk_cairo_rectangle( context, &radialRect );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        if(needToDestroyContext)
            cairo_destroy(context);
        else
            cairo_restore(context);
        return true;

    }

    //__________________________________________________________________
    void Style::renderMenuBackground( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, StyleOptions options ) const
    {
        // define colors
        ColorUtils::Rgba base(settings().palette().color( Palette::Window ) );
        ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
        ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );

        // get window dimension and position
        gint ww, wh;
        gint wx, wy;
        //if( !Gtk::gdk_map_to_toplevel( window, widget, &wx, &wy, &ww, &wh, true ) )
        if( !Gtk::gdk_map_to_toplevel( window, 0L, &wx, &wy, &ww, &wh, true ) )
        { return; }

        // translate to toplevel coordinates
        x+=wx;
        y+=wy;

        // create context and translate
        Cairo::Context context( window, clipRect );
        cairo_translate( context, -wx, -wy );
        const bool hasAlpha( options&Alpha );
        const bool isMenu( options&Menu );
        const bool isMozilla( settings().applicationName().isMozilla() );
        const bool isOpenOffice( settings().applicationName().isOpenOffice() );

        GdkRectangle rect = { x, y, w, h };

        // paint translucent first
        if( hasAlpha && !isOpenOffice )
        {
            cairo_rectangle( context, 0, 0, ww, wh );
            cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
            cairo_set_source( context, ColorUtils::alphaColor( base, 0 ) );
            cairo_fill( context );
        }

        const int splitY( std::min(200, 3*wh/4 ) );
        const int verticalOffset( (isMozilla||isOpenOffice||!isMenu) ? 0:Menu_VerticalOffset );

        GdkRectangle upperRect = { 0, verticalOffset, ww, splitY - verticalOffset };
        if( gdk_rectangle_intersect( &rect, &upperRect, &upperRect ) )
        {
            // upper rect
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, splitY ) );
            cairo_pattern_add_color_stop( pattern, 0, top );
            cairo_pattern_add_color_stop( pattern, 1, bottom );

            gdk_cairo_rounded_rectangle( context, &upperRect, 3.5, isMozilla ? CornersNone:CornersTop );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        GdkRectangle lowerRect = { 0, splitY, w, wh-splitY - verticalOffset };
        if( gdk_rectangle_intersect( &rect, &lowerRect, &lowerRect ) )
        {

            // lower part
            gdk_cairo_rounded_rectangle( context, &lowerRect, 3.5, isMozilla ? CornersNone:CornersBottom );
            cairo_set_source( context, bottom );
            cairo_fill( context );

        }

        return;

    }

    //__________________________________________________________________
    void Style::renderTooltipBackground( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, StyleOptions options ) const
    {

        // define colors
        ColorUtils::Rgba base(settings().palette().color( Palette::Tooltip ) );
        ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
        ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );

        // create context and translate
        Cairo::Context context( window, clipRect );
        cairo_translate( context, x, y );

        // paint translucent first
        const bool hasAlpha( (options&Alpha) );
        const bool isMozilla( settings().applicationName().isMozilla() );
        const bool isOpenOffice( settings().applicationName().isOpenOffice() );

        if( hasAlpha )
        {
            if( settings().tooltipTransparent() )
            {
                top.setAlpha( 0.86 );
                bottom.setAlpha( 0.86 );
            }

            cairo_rectangle( context, 0, 0, w, h );
            cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
            cairo_set_source( context, ColorUtils::alphaColor( base, 0 ) );
            cairo_fill( context );
        }

        // fill
        {
            GdkRectangle rect = { 0, 0, w, h };
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, h ) );
            cairo_pattern_add_color_stop( pattern, 0, top );
            cairo_pattern_add_color_stop( pattern, 1, bottom );

            gdk_cairo_rounded_rectangle( context, &rect, 4, (isMozilla||isOpenOffice) ? CornersNone:CornersAll );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        // contrast pixel
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, h ) );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::lightColor( bottom ) );
            cairo_pattern_add_color_stop( pattern, 0.9, bottom );

            cairo_rounded_rectangle( context, 0.5, 0.5, w-1, h-1, 4, (isMozilla||isOpenOffice) ? CornersNone:CornersAll );
            cairo_set_line_width( context, 1.0 );
            cairo_set_source( context, pattern );
            cairo_stroke( context );
        }

        return;

    }

    //__________________________________________________________________
    void Style::renderHeaderBackground( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h ) const
    {

        // load color
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );

        // render normal window background
        if( !renderWindowBackground( window, clipRect, x, y, w, h ) )
        {
            // if failed, fallback to flat background
            Cairo::Context context( window, clipRect );
            cairo_rectangle( context, x, y, w, h );
            cairo_set_source( context, base );
            cairo_fill( context );
        }

        // render lines
        renderHeaderLines( window, clipRect, x, y, w, h );

        // render side dots
        Cairo::Context context( window, clipRect );
        int yCenter( y + h/2 );
        int xDots( x + w - 1 );
        helper().renderDot( context, base, xDots, yCenter - 3 );
        helper().renderDot( context, base, xDots, yCenter );
        helper().renderDot( context, base, xDots, yCenter + 3 );

    }


    //__________________________________________________________________
    void Style::renderHeaderLines( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h ) const
    {

        // add horizontal lines
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );

        Cairo::Context context( window, clipRect );
        cairo_set_line_width( context, 1.0 );

        // dark line
        cairo_set_source( context, dark );
        cairo_move_to( context, x, y+h-0.5 );
        cairo_line_to( context, x+w, y+h-0.5 );
        cairo_stroke( context );

        // light line
        cairo_set_source( context, light );
        cairo_move_to( context, x, y+h-1.5 );
        cairo_line_to( context, x+w, y+h-1.5 );
        cairo_stroke( context );

    }

    //____________________________________________________________________________________
    void Style::renderTreeLines( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const Gtk::CellInfoFlags& cellFlags, StyleOptions options ) const
    {

        // define pen color
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base( ColorUtils::mix(
            settings().palette().color( group, Palette::Text ),
            settings().palette().color( group, Palette::Window ),
            0.8 ) );

        Cairo::Context context( window, clipRect );
        cairo_set_source( context, base );
        cairo_set_line_width( context, 1.0 );

        int cellIndent( cellFlags._levelIndent + cellFlags._expanderSize + 4 );
        int xStart( x + cellIndent/2 );
        for( unsigned int i=0; i< cellFlags._depth; ++i  )
        {

            const bool isLastCell( cellFlags._isLast[i] );

            const bool last( i == cellFlags._depth -1 );
            double xCenter = xStart;

            if( last )
            {

                double yCenter = int(y+h/2);
                const bool hasChildren( cellFlags._flags & Gtk::CellInfoFlags::HasChildren );

                if( hasChildren )
                {
                    // first vertical line
                    cairo_move_to( context, xCenter + 0.5 , y );
                    cairo_line_to( context, xCenter + 0.5, yCenter - int(cellFlags._expanderSize/3 ) );

                    // second vertical line
                    if( !isLastCell )
                    {

                        cairo_move_to( context, xCenter + 0.5, y+h );
                        cairo_line_to( context, xCenter + 0.5, yCenter + int( cellFlags._expanderSize/3 ) );
                    }

                    // horizontal line
                    cairo_move_to( context, xCenter + int( cellFlags._expanderSize/3 ), yCenter + 0.5 );
                    cairo_line_to( context, xCenter + cellFlags._expanderSize*2/3, yCenter + 0.5 );

                } else {

                    cairo_move_to( context, xCenter + 0.5, y );
                    if( isLastCell ) cairo_line_to( context, xCenter + 0.5, yCenter );
                    else cairo_line_to( context, xCenter + 0.5, y+h );

                    // horizontal line
                    cairo_move_to( context, xCenter, yCenter + 0.5 );
                    cairo_line_to( context, xCenter + cellFlags._expanderSize*2/3, yCenter + 0.5 );

                }

            } else if( !isLastCell ) {


                // vertical line
                cairo_move_to( context, xCenter + 0.5, y );
                cairo_line_to( context, xCenter + 0.5, y + h );

            }

            // render
            cairo_stroke( context );

            // increment
            xStart += cellIndent;

        }

        return;
    }

    //____________________________________________________________________________________
    bool Style::renderHoleBackground(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, TileSet::Tiles tiles ) const
    {

        // do nothing if not enough room
        if( w < 14 || h < 14 )  return false;

        /*
        this code is the same as in renderWindowBackground
        except that a mask is added to each portion of the background
        TODO: make as much code in common as possible between
        the two, to avoid code duplication.
        */

        // define colors
        ColorUtils::Rgba base(settings().palette().color( Palette::Window ) );

        // get window size and height
        gint ww, wh;
        gint wx, wy;
        if( !Gtk::gdk_map_to_toplevel( window, &wx, &wy, &ww, &wh, true ) )
        { return false; }

        // do nothing if window dimensions could not be found
        if( ww < 0 || wh < 0 ) return false;

        // the hard-coded metrics are copied for
        // kdebase/workspace/libs/oxygen/oxygenhelper.cpp
        // vertical shift to account for window decoration
        const int yShift = 23;
        wy += yShift;

        // translate to toplevel coordinates
        x+=wx;
        y+=wy;

        // create context and translate to toplevel coordinates
        Cairo::Context context( window, clipRect );
        cairo_translate( context, -wx, -wy );

        GdkRectangle rect = { x, y, w, h+1 };
        GdkRectangle mask = {x+2, y+1, w-4, h-2 };
        const double maskRadius = 3.5;
        Corners corners( CornersNone );
        if( tiles & TileSet::Top )
        {
            if( tiles & TileSet::Left ) corners |= CornersTopLeft;
            if( tiles & TileSet::Right ) corners |= CornersTopRight;
        }

        if( tiles & TileSet::Bottom )
        {
            if( tiles & TileSet::Left ) corners |= CornersBottomLeft;
            if( tiles & TileSet::Right ) corners |= CornersBottomRight;
        }

        // the hard-coded metrics are copied for
        // kdebase/workspace/libs/oxygen/oxygenhelper.cpp
        const int splitY( std::min(300, 3*wh/4 ) );

        // upper rect
        GdkRectangle upperRect = { 0, 0, ww, splitY };
        if( gdk_rectangle_intersect( &rect, &upperRect, &upperRect ) )
        {

            cairo_save( context );
            gdk_cairo_rectangle( context, &upperRect );
            cairo_clip( context );

            Cairo::Pattern pattern( verticalGradient( base, -yShift, splitY-yShift ) );
            cairo_set_source( context, pattern );

            gdk_cairo_rectangle( context, &upperRect );
            gdk_cairo_rounded_rectangle_negative( context, &mask, maskRadius, corners );
            cairo_fill( context );

            cairo_restore( context );
        }


        // lower rect
        GdkRectangle lowerRect = { 0, splitY, ww, wh - splitY + yShift };
        if( gdk_rectangle_intersect( &rect, &lowerRect, &lowerRect ) )
        {

            cairo_save( context );
            gdk_cairo_rectangle( context, &lowerRect );
            cairo_clip( context );

            ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );
            cairo_set_source( context, bottom );
            gdk_cairo_rectangle( context, &lowerRect );
            gdk_cairo_rounded_rectangle_negative( context, &mask, maskRadius, corners );
            cairo_fill( context );

            cairo_restore( context );
        }

        // radial pattern
        const int patternHeight = 64;
        const int radialW( std::min(600, ww ) );

        GdkRectangle radialRect = {  (ww - radialW)/2, 0, radialW, patternHeight };
        if( gdk_rectangle_intersect( &rect, &radialRect, &radialRect ) )
        {

            // get pattern
            Cairo::Pattern pattern( radialGradient( base, 64, patternHeight - 64, 64 ) );

            // add matrix transformation
            cairo_matrix_t transformation;
            cairo_matrix_init_identity( &transformation );
            cairo_matrix_scale( &transformation, 128.0/radialW, 1.0 );
            cairo_matrix_translate( &transformation, -(ww - radialW)/2, 0 );
            cairo_pattern_set_matrix( pattern, &transformation );

            gdk_cairo_rectangle( context, &radialRect );
            gdk_cairo_rounded_rectangle_negative( context, &mask, maskRadius, corners );
            cairo_set_source( context, pattern );
            cairo_fill( context );
        }

        return true;

    }

    //__________________________________________________________________
    void Style::renderSplitter( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, StyleOptions options ) const
    {

        // orientation
        const bool vertical( options&Vertical );

        // colors
        const ColorUtils::Rgba& base( settings().palette().color( Palette::Window ) );

        // context
        Cairo::Context context( window, clipRect );

        // hover rect
        if( options&Hover )
        {
            const ColorUtils::Rgba& highlight( ColorUtils::alphaColor( ColorUtils::lightColor( base ), 0.5 ) );

            Cairo::Context context( window, clipRect );
            Cairo::Pattern pattern;
            double a(0.1);
            if( vertical )
            {

                if( w > 30 ) a = 10.0/w;
                pattern.set( cairo_pattern_create_linear( x, 0, x+w, 0 ) );

            } else {

                if( h>30 ) a = 10.0/h;
                pattern.set( cairo_pattern_create_linear( 0, y, 0, y+h ) );

            }

            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( highlight, 0 ) );
            cairo_pattern_add_color_stop( pattern, a, highlight );
            cairo_pattern_add_color_stop( pattern, 1.0-a, highlight );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( highlight, 0 ) );

            cairo_set_source( context, pattern );
            cairo_rectangle( context, x, y, w, h );
            cairo_fill( context );
        }

        // dots
        if( vertical )
        {

            y += h/2;
            const int ngroups( std::max( 1, w/250 ) );
            int center = ( w-( ngroups-1 )*250 )/2 + x;
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                helper().renderDot( context, base, center-3, y-1 );
                helper().renderDot( context, base, center, y-1 );
                helper().renderDot( context, base, center+3, y-1 );
            }

        } else {

            x += w/2;
            const int ngroups( std::max( 1, h/250 ) );
            int center = ( h-( ngroups-1 )*250 )/2 + y;
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                helper().renderDot( context, base, x, center-3 );
                helper().renderDot( context, base, x, center );
                helper().renderDot( context, base, x, center+3 );
            }

        }

    }

    //____________________________________________________________________________________
    void Style::renderProgressBarHole(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options )
    {

        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base(settings().palette().color( group, Palette::Window ) );

        Cairo::Context context( window, clipRect );
        renderScrollBarHole( context, x, y, w, h, base, (options&Vertical) );

    }

    //____________________________________________________________________________________
    void Style::renderProgressBarHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options )
    {

        // colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base( settings().palette().color( Palette::Active, Palette::Window ) );
        const ColorUtils::Rgba glow( settings().palette().color( group, Palette::Selected ) );

        // context
        Cairo::Context context( window );

        // validate rect
        if(w<0 || h<0) return;

        // make sure that width is large enough
        const int indicatorSize( (options&Vertical ? h:w ) );
        if( indicatorSize >= 4 )
        {
            // get pixbuf
            GdkPixbuf* pixbuf( helper().progressBarIndicator( base, glow, w, h ) );
            cairo_translate( context, -1, -2 );
            cairo_translate( context, x, y );
            cairo_rectangle( context, 0, 0, gdk_pixbuf_get_width( pixbuf ), gdk_pixbuf_get_height( pixbuf ) );
            gdk_cairo_set_source_pixbuf( context, pixbuf, 0, 0 );
            cairo_fill( context );
        }

    }

    //____________________________________________________________________________________
    void Style::renderScrollBarHole(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options )
    {

        // colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base(settings().palette().color( group, Palette::Window ) );

        // context
        Cairo::Context context( window, clipRect );
        renderScrollBarHole( context, x, y, w, h, base, options&Vertical );

    }

    //____________________________________________________________________________________
    void Style::renderScrollBarHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options ) const
    {

        // store colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba color( settings().palette().color( group, Palette::Button ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( color ) );
        const ColorUtils::Rgba mid( ColorUtils::midColor( color ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( color ) );
        const ColorUtils::Rgba shadow( ColorUtils::shadowColor( color ) );
        const ColorUtils::Rgba base( ColorUtils::mix( dark, shadow, 0.5 ) );

        // glow color
        ColorUtils::Rgba glow;
        if( settings().scrollBarColored() ) glow = ColorUtils::mix( dark, shadow, 0.5 );
        else if( options&Hover ) glow = settings().palette().color( Palette::Hover );
        else glow = base;

        // vertical
        const bool vertical( options&Vertical );

        const double xf( vertical ? x+2 : x+3 );
        const double yf( vertical ? y+2 : y+1 );
        const double wf( vertical ? w-4 : w-6 );
        const double hf( vertical ? h-4 : h-3 );

        if( wf <= 0 || hf <= 0 ) return;

        // context
        Cairo::Context context( window, clipRect );

        // glow, shadow
        {
            cairo_rounded_rectangle( context, xf-0.8, yf-0.8, wf+1.6, hf+1.6, 3 );
            cairo_set_source( context, ColorUtils::alphaColor( glow, 0.6 ) );
            cairo_fill( context );

            cairo_rounded_rectangle( context, xf-1.2, yf-0.8, wf+2.4, hf+1.6, 3 );
            cairo_set_source( context, ColorUtils::alphaColor( glow, 0.3 ) );
            cairo_set_line_width( context, 1.5 );
            cairo_stroke( context );

        }

        // colored background
        if( settings().scrollBarColored() )
        {
            if( options&Hover ) cairo_set_source( context, settings().palette().color( Palette::Hover ) );
            else cairo_set_source( context, color );

            cairo_rounded_rectangle( context, xf, yf, wf, hf, 2 );
            cairo_fill( context );
        }


        // slider pattern
        {

            Cairo::Pattern pattern;
            if( vertical ) pattern.set( cairo_pattern_create_linear( xf, 0, xf+wf, 0 ) );
            else pattern.set( cairo_pattern_create_linear( 0, yf, 0, yf+hf ) );

            if( settings().scrollBarColored() )
            {

                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( light, 0.6 ) );
                cairo_pattern_add_color_stop( pattern, 0.3, ColorUtils::alphaColor( dark, 0.3 ) );
                cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( light, 0.8 ) );

            } else {

                cairo_pattern_add_color_stop( pattern, 0, color );
                cairo_pattern_add_color_stop( pattern, 1, mid );

            }

            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, xf, yf, wf, hf, 2 );
            cairo_fill( context );

        }

        // pattern
        if( settings().scrollBarBevel() )
        {

            Cairo::Pattern pattern;
            if( vertical ) pattern.set( cairo_pattern_create_linear( 0, 0, 0, 30 ) );
            else pattern.set( cairo_pattern_create_linear( 0, 0, 30, 0 ) );
            cairo_pattern_set_extend( pattern, CAIRO_EXTEND_REFLECT );
            if( settings().scrollBarColored() )
            {

                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.15 ) );
                cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( light, 0.15 ) );

            } else {

                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( shadow, 0.1 ) );
                cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( light, 0.1 ) );

            }

            cairo_set_source( context, pattern );
            cairo_rounded_rectangle( context, xf, yf, wf, hf, 2 );
            cairo_fill( context );

        }

        // bevel
        if( !settings().scrollBarColored() )
        {
            Cairo::Pattern pattern;
            if( vertical ) pattern.set( cairo_pattern_create_linear( 0, yf, 0, yf+hf ) );
            else pattern.set( cairo_pattern_create_linear( xf, 0, xf+wf, 0 ) );

            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( light, 0 ) );
            cairo_pattern_add_color_stop( pattern, 0.5, light );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( light, 0 ) );
            cairo_set_source( context, pattern );
            cairo_set_line_width( context, 1 );

            if( vertical )
            {
                cairo_move_to( context, xf + 0.5, yf + 0.5 );
                cairo_line_to( context, xf + 0.5, yf + hf -1.5 );
                cairo_stroke( context );

                cairo_move_to( context, xf + wf - 0.5, yf + 0.5 );
                cairo_line_to( context, xf + wf - 0.5, yf + hf - 0.5 );
                cairo_stroke( context );

            } else {

                cairo_move_to( context, xf + 0.5, yf + 0.5 );
                cairo_line_to( context, xf + wf - 0.5, yf + 0.5 );
                cairo_stroke( context );

                cairo_move_to( context, xf + 0.5, yf + hf - 0.5 );
                cairo_line_to( context, xf + wf - 0.5, yf + hf - 0.5 );
                cairo_stroke( context );

            }

        }

    }

    //____________________________________________________________________________________
    void Style::renderToolBarHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options ) const
    {

        const bool vertical( options & Vertical );
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );

        Cairo::Context context( window, clipRect );
        int counter(0);
        if( vertical )
        {

            const int xcenter( x+w/2 );
            for( int ycenter = y+2; ycenter <= y+h-3; ycenter+=3, ++counter )
            {
                if( counter%2 == 0 ) helper().renderDot( context, base, xcenter+1, ycenter );
                else helper().renderDot( context, base, xcenter-2, ycenter );
            }

        } else {

            const int ycenter( y + h/2 );
            for( int xcenter = x+2; xcenter < x+w-3; xcenter+=3, ++counter )
            {
                if( counter%2 == 0 ) helper().renderDot( context, base, xcenter, ycenter+1 );
                else helper().renderDot( context, base, xcenter, ycenter-2 );
            }

        }

        return;

    }

    //__________________________________________________________________
    void Style::drawFloatFrame( cairo_t* context, GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, StyleOptions options ) const
    {

        // define colors
        ColorUtils::Rgba base(settings().palette().color( Palette::Window ) );
        ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        ColorUtils::Rgba dark( ColorUtils::darkColor( ColorUtils::backgroundBottomColor( base ) ) );

        const bool hasAlpha( options&Alpha );
        const bool isMenu( options&Menu );
        const bool isMozilla( settings().applicationName().isMozilla() );
        const bool isOpenOffice( settings().applicationName().isOpenOffice() );
        const bool drawUglyShadow( !hasAlpha );
        const bool rounded( !(isMozilla||isOpenOffice) );

        // if we aren't drawing window decoration
        if( !context )
        {
            // create context
            // make it the old good way since context is cairo_t* instead Cairo::Context
            context=gdk_cairo_create(window);
            if(clipRect)
            {
                cairo_rectangle(context,clipRect->x,clipRect->y,clipRect->width,clipRect->height);
                cairo_clip(context);
            }
        }

        Cairo::Pattern pattern( cairo_pattern_create_linear( 0, double(y)+0.5, 0, y+h-1 ) );

        // add vertical offset
        if( isMenu && rounded )
        {
            y += Menu_VerticalOffset;
            h -= 2*Menu_VerticalOffset;
        }

        if( drawUglyShadow )
        {

            // adjust rectangle
            x++;
            y++;
            w-=2;
            h-=2;

            cairo_set_line_width(context,1);

            if( options&Focus ) // window is active - it's a glow, not a shadow
            {
                ColorUtils::Rgba frameColor(0x54/255.,0xa7/255.,0xf0/255.); // FIXME: where should this color be taken from?
                ColorUtils::Rgba glow=ColorUtils::mix(ColorUtils::Rgba(0.5,0.5,0.5),frameColor,0.7);
                cairo_set_source(context,glow);

                const double radius( 11*0.5 );
                cairo_move_to( context, x+4, y-0.5 ); cairo_line_to( context, x+w-4, y-0.5 );
                cairo_stroke( context );

                cairo_arc_negative( context, x-0.5+radius, y-0.5+radius, radius, -0.5*M_PI, -M_PI );
                cairo_stroke( context );
                cairo_arc_negative( context, x+w-11+0.5+radius, y-0.5+radius, radius, 0, -0.5*M_PI );
                cairo_stroke( context );

                cairo_move_to( context, x-0.5, y+4 ); cairo_line_to( context, x-0.5, y+h-4 );
                cairo_move_to( context, x+w+0.5, y+4 ); cairo_line_to( context, x+w+0.5, y+h-4 );
                cairo_stroke( context );

                cairo_arc_negative( context, x-0.5+radius, y+h-11+0.5+radius, radius, -M_PI, -1.5*M_PI );
                cairo_stroke( context );
                cairo_arc_negative( context, x+w-11+0.5+radius, y+h-11+0.5+radius, radius, 0.5*M_PI, 0 );
                cairo_stroke( context );

                cairo_move_to( context, x+4, y+h+0.5 ); cairo_line_to( context, x+w-4, y+h+0.5 );
                cairo_stroke( context );


                light=ColorUtils::mix(light, frameColor);
                dark=ColorUtils::mix(dark,frameColor);

            } else {

                // window inactive - draw something resembling shadow
                // fully desaturate
                ColorUtils::Rgba shadow( ColorUtils::darken( base, 0., 0. ) );

                if(rounded)
                {
                    const double radius( 11*0.5 );
                    cairo_set_source( context, ColorUtils::darken( shadow, 0.2 ));
                    cairo_move_to( context, x+4, y-0.5 ); cairo_line_to( context, x+w-4, y-0.5 );
                    cairo_stroke( context );

                    cairo_arc_negative( context, x-0.5+radius, y-0.5+radius, radius, -0.5*M_PI, -M_PI );
                    cairo_stroke( context );
                    cairo_arc_negative( context, x+w-11+0.5+radius, y-0.5+radius, radius, 0, -0.5*M_PI );
                    cairo_stroke( context );

                    cairo_set_source( context, ColorUtils::darken( shadow, 0.35 ));
                    cairo_move_to( context, x-0.5, y+4 ); cairo_line_to( context, x-0.5, y+h-4 );
                    cairo_move_to( context, x+w+0.5, y+4 ); cairo_line_to( context, x+w+0.5, y+h-4 );
                    cairo_stroke( context );

                    cairo_set_source( context, ColorUtils::darken( shadow, 0.45 ));
                    cairo_arc_negative( context, x-0.5+radius, y+h-11+0.5+radius, radius, -M_PI, -1.5*M_PI );
                    cairo_stroke( context );
                    cairo_arc_negative( context, x+w-11+0.5+radius, y+h-11+0.5+radius, radius, 0.5*M_PI, 0 );
                    cairo_stroke( context );

                    cairo_set_source( context, ColorUtils::darken( shadow, 0.6 ));
                    cairo_move_to( context, x+4, y+h+0.5 ); cairo_line_to( context, x+w-4, y+h+0.5 );
                    cairo_stroke( context );

                } else {
                    cairo_set_source( context, ColorUtils::darken( shadow, 0.2 ));
                    cairo_move_to( context, x-0.5, y-0.5 ); cairo_line_to( context, x+w+0.5, y-0.5 );
                    cairo_stroke( context );

                    cairo_set_source( context, ColorUtils::darken( shadow, 0.35 ));
                    cairo_move_to( context, x-0.5, y-0.5 ); cairo_line_to( context, x-0.5, y+h+0.5 );
                    cairo_move_to( context, x+w+0.5, y-0.5 ); cairo_line_to( context, x+w+0.5, y+h+0.5 );
                    cairo_stroke( context );

                    cairo_set_source( context, ColorUtils::darken( shadow, 0.6 ));
                    cairo_move_to( context, x-0.5, y+h+0.5 ); cairo_line_to( context, x+w+0.5, y+h+0.5 );
                    cairo_stroke( context );
                }
            }
        }

        cairo_pattern_add_color_stop( pattern, 0, light );

        if( !rounded )
        {

            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::alphaColor( dark, 0 ) );
            cairo_rectangle( context, double(x)+0.5, double(y)+0.5, w-1, h-1 );

        } else {

            if( h > 20.5 ) cairo_pattern_add_color_stop( pattern, std::max( 0.0, 1.0 - 12.0/( double(h)-5.5 ) ), ColorUtils::alphaColor( light, 0.5 ) );
            else if( h > 8.5 ) cairo_pattern_add_color_stop( pattern, std::max( 0.0, 3.0/( double(h)-5.5 ) ), ColorUtils::alphaColor( light, 0.5 ) );
            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( light ) );
            cairo_rounded_rectangle( context, double(x)+0.5, double(y)+0.5, w-1, h-1, 3.5 );

        }

        cairo_rounded_rectangle( context, x+0.5, y+0.5, w-1, h-1, 3.5, rounded ? CornersAll : CornersNone );

        cairo_set_source( context, pattern );
        cairo_set_line_width( context, 0.8 );
        cairo_stroke( context );
    }

    //__________________________________________________________________
    void Style::renderButtonSlab(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options, TileSet::Tiles tiles,
        const ColorUtils::Rgba& color
        )
    {

        // flat buttons are only rendered with a simple Rect, and only when either focused or sunken
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        if( options & Flat )
        {
            if( options & Sunken )
            {

                Cairo::Context context( window, clipRect );
                ColorUtils::Rgba base( color.isValid() ? color : settings().palette().color( group, Palette::Window ) );

                if( options & Hover )
                {

                    const ColorUtils::Rgba glow( settings().palette().color( group, Palette::Hover ) );
                    helper().holeFocused( base, glow, 0, 7 ).render( context, x, y, w, h );

                } else helper().hole( base, 0, 7 ).render( context, x, y, w, h );

            } else if( options&Hover ) {

                const ColorUtils::Rgba glow( settings().palette().color( group, Palette::Focus ) );
                Cairo::Context context( window, clipRect );
                helper().slitFocused( glow ).render( context, x, y, w, h, tiles );

            }

            return;

        }

        // do nothing if not enough room
        if( w < 14 ) return;

        // define colors
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( color.isValid() ? color : settings().palette().color( group, Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = color.isValid() ? color : settings().palette().color( group, Palette::Button );

        }

        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );

        // fill with appropriate pattern
        Cairo::Pattern pattern;
        if( options&Sunken )
        {

            pattern.set( cairo_pattern_create_linear( 0, y-h, 0, y+h ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 1.0, base );

        } else {

            pattern.set( cairo_pattern_create_linear( 0, double(y)-0.2*h, 0, double(y) + h + 0.4*h ) );
            cairo_pattern_add_color_stop( pattern, 0, light );
            cairo_pattern_add_color_stop( pattern, 0.6, base );

        }

        cairo_set_source( context, pattern );
        helper().fillSlab( context, x, y, w, h, tiles );

        if( options&Sunken )
        {

            helper().slabSunken( base, 0 ).render( context, x, y, w, h, tiles );

        } else {

            const bool enabled( !(options&Disabled ) );
            if( enabled && (options & Hover) )
            {

                ColorUtils::Rgba glow( settings().palette().color( Palette::Hover ) );
                helper().slabFocused( base, glow, 0 ).render( context, x, y, w, h, tiles );

            } else if( enabled && (options & Focus) ) {

                ColorUtils::Rgba glow( settings().palette().color( Palette::Focus) );
                helper().slabFocused( base, glow, 0 ).render( context, x, y, w, h, tiles );

            } else {

                helper().slab( base, 0 ).render( context, x, y, w, h, tiles );

            }

        }

    }

    //__________________________________________________________________
    void Style::renderSlab(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap, StyleOptions options )
    {

        // define colors
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( settings().palette().color( Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = settings().palette().color( Palette::Button );

        }

        // create context
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );
        renderSlab( context, x, y, w, h, base, options, TileSet::Ring );

    }

    //__________________________________________________________________
    void Style::renderInfoBar(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        const ColorUtils::Rgba& glow )
    {

        // define colors
        gint wh, wy;
        Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
        const ColorUtils::Rgba base( ColorUtils::backgroundColor( settings().palette().color( Palette::Button ), wh, y+wy+h/2 ) );

        // create context
        Cairo::Context context( window, clipRect );

        // fill
        {
            Cairo::Pattern pattern;
            const ColorUtils::Rgba shadow( ColorUtils::shadowColor( base ) );
            pattern.set( cairo_pattern_create_linear( 0, y-h, 0, y+h ) );
            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::lightColor( base ) );
            cairo_pattern_add_color_stop( pattern, 1.0, base );

            cairo_set_source( context, pattern );
            helper().fillSlab( context, x, y, w, h );
        }

        // slab
        helper().slabFocused( base, glow, 0 ).render( context, x, y, w, h );

    }

    //__________________________________________________________________
    void Style::renderCheckBox(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, GtkShadowType shadow, StyleOptions options )
    {

        // define checkbox rect
        gint cbw = CheckBox_Size;
        if( options & Flat ) cbw -= 5;
        GdkRectangle parent = {x, y, w, h };
        GdkRectangle child = {0, 0, cbw, cbw };
        centerRect( &parent, &child );

        // define colors
        ColorUtils::Rgba base;
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const Palette::Role role( options&Flat ? Palette::Window : Palette::Button );
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( settings().palette().color( group, role ), wh, y+wy+h/2 );

        } else {

            base = settings().palette().color( group, role );

        }

        // draw slab
        Cairo::Context context( window, clipRect );

        if( options & Flat )
        {

            helper().holeFlat( base, 0 ).render( context, child.x+1, child.y-1, child.width, child.height, TileSet::Full );
            cairo_translate( context, 0, -2 );

        } else {

            StyleOptions localOptions( options );
            localOptions &= ~Sunken;
            renderSlab( context, child.x, child.y, child.width, child.height, base, localOptions );

        }

        // draw mark
        x = int( double(child.x + child.width/2) - 3.5 );
        y = int( double(child.y + child.height/2) - 2.5 );

        if( shadow == GTK_SHADOW_IN || shadow == GTK_SHADOW_ETCHED_IN )
        {

            cairo_set_line_cap( context, CAIRO_LINE_CAP_ROUND );
            cairo_set_line_join( context, CAIRO_LINE_JOIN_ROUND );
            if( shadow == GTK_SHADOW_IN ) cairo_set_line_width( context, 2.0 );

            Palette::Group group( (options&Disabled) ? Palette::Disabled : Palette::Active );
            const ColorUtils::Rgba& color( settings().palette().color( group, ( options&Sunken ) ? Palette::WindowText : Palette::ButtonText ) );
            const ColorUtils::Rgba& background( settings().palette().color( ( options&Sunken ) ? Palette::Window : Palette::Button ) );
            const ColorUtils::Rgba& base( ColorUtils::decoColor( background, color ) );
            const ColorUtils::Rgba& contrast( ColorUtils::lightColor( background ) );

            cairo_translate( context, 0.5, 0.5 );

            const double offset( 1.0 );
            if( settings().checkBoxStyle() == QtSettings::CS_CHECK )
            {

                // dask pattern for tristate buttons
                if( shadow == GTK_SHADOW_ETCHED_IN )
                {
                    cairo_set_line_width( context, 1.3 );
                    double dashes[2] = { 1.3, 2.6 };
                    cairo_set_dash( context, &dashes[0], 2, 0 );
                }

                cairo_save( context );
                cairo_translate( context, 0, offset );
                cairo_move_to( context, x+9, y );
                cairo_line_to( context, x+3, y+7 );
                cairo_line_to( context, x, y+4 );
                cairo_restore( context );
                cairo_set_source( context, contrast );
                cairo_stroke( context );

                cairo_move_to( context, x+9, y );
                cairo_line_to( context, x+3, y+7 );
                cairo_line_to( context, x, y+4 );
                cairo_set_source( context, base );
                cairo_stroke( context );

            } else {

                // dask pattern for tristate buttons
                if( shadow == GTK_SHADOW_ETCHED_IN )
                {
                    double dashes[2] = { 0.8, 4.0 };
                    cairo_set_dash( context, &dashes[0], 2, 0 );
                }

                if( options&Flat )
                {

                    cairo_save( context );
                    cairo_translate( context, 0, offset );
                    cairo_move_to( context, x+8, y ); cairo_line_to( context, x+1, y+7 );
                    cairo_move_to( context, x+8, y+7 ); cairo_line_to( context, x+1, y );
                    cairo_restore( context );
                    cairo_set_source( context, contrast );
                    cairo_stroke( context );

                    cairo_move_to( context, x+8, y ); cairo_line_to( context, x+1, y+7 );
                    cairo_move_to( context, x+8, y+7 ); cairo_line_to( context, x+1, y );
                    cairo_set_source( context, base );
                    cairo_stroke( context );

                } else {

                    cairo_save( context );
                    cairo_translate( context, 0, offset );
                    cairo_move_to( context, x+8, y-1 ); cairo_line_to( context, x, y+7 );
                    cairo_move_to( context, x+8, y+7 ); cairo_line_to( context, x, y-1 );
                    cairo_restore( context );
                    cairo_set_source( context, contrast );
                    cairo_stroke( context );

                    cairo_move_to( context, x+8, y-1 ); cairo_line_to( context, x, y+7 );
                    cairo_move_to( context, x+8, y+7 ); cairo_line_to( context, x, y-1 );
                    cairo_set_source( context, base );
                    cairo_stroke( context );

                }

            }

        }

    }

    //___________________________________________________________________
    void Style::renderRadioButton(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, GtkShadowType shadow, StyleOptions options )
    {

        // define checkbox rect
        gint cbw( CheckBox_Size );
        gint tileSize( CheckBox_Size/3 );
        double scale( 1.0 );

        if( settings().applicationName().isOpenOffice() )
        {
            const gint dimension = std::min( w, h );
            cbw = std::min( 3*( 1 + dimension/3 ), (int)CheckBox_Size );
            scale = double(cbw)/CheckBox_Size;
            tileSize = cbw/3;
        }

        GdkRectangle parent = {x, y, w, h };
        GdkRectangle child = {0, 0, cbw, cbw };
        centerRect( &parent, &child );
        x = child.x;
        y = child.y;

        // define colors
        ColorUtils::Rgba base;
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );

            if( options & Menu ) base = ColorUtils::menuBackgroundColor( settings().palette().color( group, Palette::Button ), wh, y+wy+h/2 );
            else base = ColorUtils::backgroundColor( settings().palette().color( group, Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = settings().palette().color( group, Palette::Button );

        }

        const ColorUtils::Rgba glow( slabShadowColor( options ) );

        // get the pixmap
        GdkPixbuf* pixbuf( glow.isValid() ? helper().roundSlabFocused( base, glow, 0, tileSize ):helper().roundSlab( base, 0, tileSize ) );

        // create context
        Cairo::Context context( window, clipRect );
        cairo_save( context );
        cairo_translate( context, x, y );
        cairo_rectangle( context, 0, 0, child.width, child.height );
        gdk_cairo_set_source_pixbuf( context, pixbuf, 0, 0 );
        cairo_fill( context );
        cairo_restore( context );

        if( shadow == GTK_SHADOW_IN || shadow == GTK_SHADOW_ETCHED_IN )
        {
            double radius( shadow == GTK_SHADOW_IN ? 2.6:1.4 );
            radius *= scale;

            const double dx( 0.5*child.width - radius );
            const double dy( 0.5*child.height - radius );

            const ColorUtils::Rgba background( settings().palette().color( Palette::Button ) );
            const ColorUtils::Rgba color( settings().palette().color( group, Palette::ButtonText ) );

            cairo_save( context );
            cairo_translate( context, 0, radius/2 );
            cairo_ellipse( context, x+dx, y+dy, child.width - 2*dx, child.height -2*dy );
            cairo_restore( context );
            if( shadow == GTK_SHADOW_IN )
            {

                cairo_set_source( context, ColorUtils::lightColor( background ) );
                cairo_fill( context );

                cairo_set_source( context, ColorUtils::decoColor( background, color ) );
                cairo_ellipse( context, x+dx, y+dy, child.width - 2*dx, child.height -2*dy );
                cairo_fill( context );

            } else {

                cairo_set_line_width( context, 1.3 );
                cairo_set_source( context, ColorUtils::lightColor( background ) );
                cairo_stroke( context );

                cairo_set_source( context, ColorUtils::decoColor( background, color ) );
                cairo_ellipse( context, x+dx, y+dy, child.width - 2*dx, child.height -2*dy );
                cairo_stroke( context );

            }

        }

        return;
    }

    //____________________________________________________________________________________
    void Style::renderHole(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap, StyleOptions options,
        TileSet::Tiles tiles )
    {

        // do nothing if not enough room
        if( w < 14 || h < 14 )  return;

        // enable state
        const bool enabled( !(options&Disabled ) );

        // load color
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        ColorUtils::Rgba fill;
        if( !( options&NoFill ) )
        {
            const Palette::Group group( enabled ? Palette::Active : Palette::Disabled );
            fill = settings().palette().color( group, Palette::Base );
        }

        // create context, add mask, and render hole
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );

        if( fill.isValid() ) tiles |= TileSet::Center;

        if( enabled && (options & Focus) )
        {

            const ColorUtils::Rgba glow( settings().palette().color( Palette::Focus ) );
            helper().holeFocused( base, fill, glow, 0, 7 ).render( context, x, y, w, h, tiles );

        } else if( enabled && (options & Hover) ) {

            const ColorUtils::Rgba glow( settings().palette().color( Palette::Hover ) );
            helper().holeFocused( base, fill, glow, 0, 7 ).render( context, x, y, w, h, tiles );

        } else {

            helper().hole( base, fill, 0, 7 ).render( context, x, y, w, h, tiles );

        }

    }

    //____________________________________________________________________________________
    void Style::renderDockFrame(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap, StyleOptions options )
    {

        // do nothing if not enough room
        if( w < 9 || h < 9 )  return;

        // define colors
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( settings().palette().color( Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = settings().palette().color( Palette::Button );

        }

        // create context, add mask, and render
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );
        helper().dockFrame( base, w ).render( context, x, y, w, h );
    }

    //____________________________________________________________________________________
    void Style::renderMenuItemRect(
        GdkWindow* window,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        gint x, gint y, gint w, gint h, StyleOptions options )
    {
        ColorUtils::Rgba base;
        gint wh, wy;
        Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
        const bool isInMenu( Gtk::gtk_parent_menu( widget ) );
        const bool isInMenuBar( Gtk::gtk_parent_menubar( widget ) );

        if( wh > 0 )
        {
            if( isInMenu ) base = ColorUtils::menuBackgroundColor( ColorUtils::midColor( settings().palette().color( Palette::Window ) ), wh, y+wy+h/2 );
            else base = ColorUtils::backgroundColor( ColorUtils::midColor( settings().palette().color( Palette::Window ) ), wh, y+wy+h/2 );

        } else {

            base = ColorUtils::midColor( settings().palette().color( Palette::Window ) );

        }

        // more color customization, based on menuHighlight mode
        ColorUtils::Rgba color( base );
        if( settings().menuHighlightMode() == QtSettings::MM_STRONG )
        {

            if( (options & Sunken) || isInMenu ) color = settings().palette().color( Palette::Selected );
            else color = ColorUtils::tint( color, settings().palette().color( Palette::Hover ) );

        } else if( settings().menuHighlightMode() == QtSettings::MM_SUBTLE ) {

            if( (options & Sunken) || isInMenu ) color = ColorUtils::mix( color, ColorUtils::tint( color, settings().palette().color( Palette::Selected ), 0.6 ) );
            else color = ColorUtils::mix( color, ColorUtils::tint( color, settings().palette().color( Palette::Hover ), 0.6 ) );

        }

        if( isInMenuBar )
        {

            // we force ytickness in gtkrc to emulate Qt menubar/menu space separation
            // so adjust vertical extent of the rect in menubar
            if( settings().applicationName().isMozilla() )
            {

                y+=3;
                h-=6;

            } else {

                y+=1;
                h-=2;

            }
        } else if ( isInMenu ) {

            // we force ytickness to 5 in gtkrc to emulate Qt menuitems space separation
            // so adjust vertical extent of the rect in menus to 21 (size with standard ytickness)
            if( h > 27 )
            {

                y+=4;
                h-=7;

            } else {

                y+=(h-21)/2;
                h=21;

            }

        }

        bool hasSubMenu( isInMenu && GTK_IS_MENU_ITEM( widget ) && gtk_menu_item_get_submenu( GTK_MENU_ITEM( widget ) ) );
        if( hasSubMenu )
        {

            // draw item rect in a pixbuf
            GdkPixbuf* pixbuf( gdk_pixbuf_new( GDK_COLORSPACE_RGB, true, 8, w, h ) );
            gdk_pixbuf_fill( pixbuf, ColorUtils::Rgba::transparent( color ).toInt() );
            {
                Cairo::Context context( pixbuf );
                helper().holeFlat( color, 0 ).render( context, 0, 0, w, h, TileSet::Full  );
                context.updateGdkPixbuf();
            }

            Cairo::Context context( window, clipRect );
            cairo_translate( context, x, y );
            gdk_cairo_set_source_pixbuf( context, pixbuf, 0, 0 );

            // generate linear gradient for masking
            Cairo::Pattern mask( cairo_pattern_create_linear( w-40, 0, w-4, 0 ) );
            cairo_pattern_add_color_stop( mask, 0,  ColorUtils::Rgba::black() );
            cairo_pattern_add_color_stop( mask, 1,  ColorUtils::Rgba::transparent() );
            cairo_mask( context, mask );

            g_object_unref( pixbuf );

        } else {

            Cairo::Context context( window, clipRect );
            helper().holeFlat( color, 0 ).render( context, x, y, w, h, TileSet::Full  );
        }
    }

    //____________________________________________________________________________________
    void Style::renderSelection(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        TileSet::Tiles tiles,
        StyleOptions options
        )
    {

        // do nothing if not selected nor hovered
        if( !options & (Hover|Selected ) ) return;

        Palette::Group group( (options & Focus) ? Palette::Active : Palette::Inactive );
        ColorUtils::Rgba base( settings().palette().color( group, Palette::Selected ) );
        if( options & Hover  )
        {
            if( !( options & Selected ) ) base.setAlpha( 0.2 );
            else base = base.light( 110 );
        }

        // create context
        Cairo::Context context( window, clipRect );
        if( !(tiles&TileSet::Left) ) { x -= 8; w+=8; }
        if( !(tiles&TileSet::Right) ) { w += 8; }
        helper().selection( base, h, false ).render( context, x, y, w, h, tiles );

    }

    //____________________________________________________________________________________
    void Style::renderArrow(
        GdkWindow* window,
        GdkRectangle* clipRect,
        GtkArrowType orientation,
        gint x, gint y, gint w, gint h,
        QtSettings::ArrowSize arrowSize, StyleOptions options ) const
    {

        // get polygon
        const Polygon arrow( genericArrow( orientation, arrowSize ) );

        // retrieve colors
        ColorUtils::Rgba base;
        if( options&Disabled ) base = settings().palette().color( Palette::Disabled, Palette::ButtonText );
        else if( options&Hover ) base = settings().palette().color( Palette::Hover );
        else base = settings().palette().color( Palette::Active, Palette::ButtonText );

        // need odd width and height
        if( !(w%2) ) w--;
        if( !(h%2) ) h--;
        const int xcenter = x + w/2;
        const int ycenter = y + h/2;

        // create context and translate to center
        Cairo::Context context( window, clipRect );
        cairo_translate( context, xcenter, ycenter );

        switch( orientation )
        {
            case GTK_ARROW_UP:
            //cairo_translate( context, 0., -0.5 );
            break;

            case GTK_ARROW_DOWN:
            if( arrowSize == QtSettings::ArrowSmall ) cairo_translate( context, 0, 0.5 );
            else cairo_translate( context, 0, 1 );
            break;

            case GTK_ARROW_LEFT:
            case GTK_ARROW_RIGHT:
            if( arrowSize == QtSettings::ArrowSmall )
            { cairo_translate( context, 0.5, 0 ); }
            break;

            default:
            break;
        }

        switch( arrowSize )
        {
            case QtSettings::ArrowTiny:
            case QtSettings::ArrowSmall:
            cairo_set_line_width( context, 1.2 );
            break;

            default:
            case QtSettings::ArrowNormal:
            cairo_set_line_width( context, 1.6 );
            break;
        }

        cairo_set_line_cap( context, CAIRO_LINE_CAP_ROUND );
        cairo_set_line_join( context, CAIRO_LINE_JOIN_ROUND );

        // contrast
        if( options&Contrast )
        {

            // contrast color
            const ColorUtils::Rgba contrast( ColorUtils::lightColor( settings().palette().color( Palette::Window ) ) );

            cairo_save( context );
            cairo_translate( context, 0, 1 );
            cairo_polygon( context, arrow );
            cairo_restore( context );

            cairo_set_source( context, contrast );
            cairo_stroke( context );
        }

        // base
        cairo_polygon( context, arrow );
        cairo_set_source( context, base );
        cairo_stroke( context );

    }

    //____________________________________________________________________________________
    void Style::renderSliderGroove(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options )
    {

        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );

        const bool vertical( options & Vertical );
        GdkRectangle parent = { x, y, w, h };

        GdkRectangle child = { 0, 0, vertical ? Slider_GrooveWidth:w, vertical ? h:Slider_GrooveWidth };
        centerRect( &parent, &child );

        Cairo::Context context( window, clipRect );
        helper().groove( base, 0 ).render( context, child.x, child.y, child.width, child.height );

    }

    //____________________________________________________________________________________
    void Style::renderSliderHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, StyleOptions options )
    {

        // define colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( settings().palette().color( group, Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = settings().palette().color( group, Palette::Button );

        }

        // render slab
        options &= ~Sunken;
        Cairo::Context context( window, clipRect );
        renderSlab( context, x, y, w, h, base, options );

    }

    //____________________________________________________________________________________
    void Style::renderSizeGrip(
        GdkWindow* window,
        GdkRectangle* clipRect,
        GdkWindowEdge edge,
        gint x, gint y, gint w, gint h ) const
    {

        gint dimension = std::min( w, h );

        // edges
        Polygon a;
        switch( edge )
        {

            case GDK_WINDOW_EDGE_NORTH_WEST:
            w = dimension;
            h = dimension;
            a << Point( double(x)+0.5, double(y)+0.5 ) << Point( double(x+w)-0.5, double(y)+0.5 ) << Point( double(x)+0.5, double(y+h)-0.5);
            break;

            case GDK_WINDOW_EDGE_NORTH_EAST:
            x+= w-dimension;
            w = dimension;
            h = dimension;
            a << Point( double(x)+0.5, double(y)+0.5 ) << Point( double(x+w)-0.5, double(y)+0.5 ) << Point( double(x+w)-0.5, double(y+h)-0.5 );
            break;

            case GDK_WINDOW_EDGE_SOUTH_WEST:
            y+= h-dimension;
            w = dimension;
            h = dimension;
            a << Point( double(x)+0.5, double(y)+0.5 ) << Point( double(x+w)-0.5, double(y+h)-0.5 ) << Point( double(x)+0.5, double(y+h)-0.5 );
            break;

            case GDK_WINDOW_EDGE_SOUTH_EAST:
            x+= w-dimension;
            y+= h-dimension;
            w = dimension;
            h = dimension;
            a << Point( double(x)+0.5, double(y+h)-0.5 ) << Point( double(x+w)-0.5, double(y)+0.5 ) << Point( double(x+w)-0.5, double(y+h)-0.5 );
            break;

            default: return;

        }


        // colors
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );

        // context
        Cairo::Context context( window, clipRect );
        cairo_set_line_width( context, 1.0 );

        // fill
        cairo_polygon( context, a );
        cairo_set_source( context, base );
        cairo_fill( context );

        // draw edges
        cairo_move_to( context, a[0].x(), a[0].y() );
        cairo_line_to( context, a[1].x(), a[1].y() );
        cairo_set_source( context, dark );
        cairo_stroke( context );

        cairo_move_to( context, a[1].x(), a[1].y() );
        cairo_line_to( context, a[2].x(), a[2].y() );
        cairo_line_to( context, a[0].x(), a[0].y() );
        cairo_set_source( context, light );
        cairo_stroke( context );

    }

    //____________________________________________________________________________________
    void Style::renderTab(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        StyleOptions options,
        TabOptions tabOptions
        )
    {

        if( tabOptions & CurrentTab )
        {

            return renderActiveTab( window, clipRect, x, y, w, h, side, options, tabOptions );

        } else {

            switch( settings().tabStyle() )
            {
                case QtSettings::TS_SINGLE: return renderInactiveTab_Single( window, clipRect, x, y, w, h, side, options, tabOptions );
                case QtSettings::TS_PLAIN: return renderInactiveTab_Plain( window, clipRect, x, y, w, h, side, options, tabOptions );
                default: return;
            }

        }

    }

    //____________________________________________________________________________________
    void Style::renderTabBarBase(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        Gtk::Gap gap,
        StyleOptions options,
        TabOptions tabOptions
        )
    {

        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );

        // adjust gap
        if( tabOptions & FirstTabAligned ) { gap.setX( gap.x()-3 ); gap.setWidth( gap.width()+3 ); }
        if( tabOptions & LastTabAligned ) { gap.setWidth( gap.width()+3 ); }

        // create context
        Cairo::Context context( window, 0L );

        // generate mask and main slab
        SlabRect tabSlab;
        const TileSet::Tiles tabTiles( Style::tabTiles( side )  );
        switch( side )
        {
            case GTK_POS_BOTTOM:
            tabSlab = SlabRect( x, y+h-4, w, 15, tabTiles );
            generateGapMask( context, x-1, y-4, w+2, h+8, gap );
            break;

            case GTK_POS_TOP:
            tabSlab = SlabRect( x, y-11, w, 15, tabTiles );
            generateGapMask( context, x-1, y-4, w+2, h+8, gap );
            break;

            case GTK_POS_RIGHT:
            tabSlab = SlabRect( x+w-4, y, 15, h, tabTiles );
            generateGapMask( context, x-4, y-1, w+8, h+2, gap );
            break;


            case GTK_POS_LEFT:
            tabSlab = SlabRect( x-11, y, 15, h, tabTiles );
            generateGapMask( context, x-4, y-1, w+8, h+2, gap );
            break;

            default: break;

        }

        // render
        helper().slab( base, 0 ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );
        return;

    }

    //__________________________________________________________________
    void Style::renderTabBarFrame(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap, StyleOptions options )
    {

        // define colors
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );

        // create context
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );
        renderSlab( context, x, y, w, h, base, options, TileSet::Ring );

    }

    //____________________________________________________________________________________
    void Style::renderTreeExpander(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkExpanderStyle style,
        StyleOptions options
        ) const
    {

        // retrieve colors
        ColorUtils::Rgba base;
        if( options&Disabled ) base = settings().palette().color( Palette::Disabled, Palette::ButtonText );
        else if( options&Hover ) base = settings().palette().color( Palette::Hover );
        else base = settings().palette().color( Palette::Active, Palette::ButtonText );

        const int xcenter( x + w/2 );
        const int ycenter( y + h/2 );

        // expander 'radius' (copied from oxygen-qt)
        const int radius( ( 9 - 4 ) / 2 );

        // create context and translate to center
        Cairo::Context context( window, clipRect );
        cairo_translate( context, xcenter, ycenter );

        cairo_set_line_width( context, 1.0 );
        cairo_set_source( context, base );

        // horizontal line
        cairo_move_to( context, 0.5-radius, 0.5 );
        cairo_line_to( context, 0.5+radius, 0.5 );

        // vertical line
        if( style == GTK_EXPANDER_COLLAPSED || style == GTK_EXPANDER_SEMI_COLLAPSED )
        {
            cairo_move_to( context, 0.5, 0.5-radius );
            cairo_line_to( context, 0.5, 0.5+radius );
        }

        cairo_stroke( context );

    }

    //__________________________________________________________________
    void Style::drawWindowDecoration( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h )
    {
        bool hasAlpha( wopt & WinDeco::Alpha );
        bool drawResizeHandle( !(wopt & WinDeco::Shaded) && (wopt & WinDeco::Resizable) );
        bool isMaximized( wopt & WinDeco::Maximized );

        cairo_save( context );
        cairo_set_source_rgba( context, 0, 0, 0, 0 );
        cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
        cairo_paint( context );
        cairo_set_operator( context, CAIRO_OPERATOR_OVER );

        if( hasAlpha )
        {
            // cut round corners using alpha
            cairo_rounded_rectangle(context,x,y,w,h,3.5);
            cairo_clip(context);
        }
        renderWindowBackground( context, 0L, 0L, 0L, x, y, w, h );

        StyleOptions options( hasAlpha ? Alpha : Blend );

        // focus
        if(wopt & WinDeco::Active) options|=Focus;

        if( !isMaximized )
        { drawFloatFrame( context, 0L, 0L, x, y, w, h, options); }

        if( drawResizeHandle )
        {
            ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
            renderWindowDots( context, x, y, w, h, base, wopt);
        }
    }

    //__________________________________________________________________
    void Style::drawWindecoButton( cairo_t* context, WinDeco::ButtonType type, WinDeco::ButtonStatus buttonState, WinDeco::Options windowState, gint x, gint y, gint w,gint h)
    {
        // validate arguments
        if(type>=WinDeco::ButtonTypeCount || buttonState>=WinDeco::ButtonStatusCount)
        { return; }

        if( !(windowState & WinDeco::Active) && buttonState == WinDeco::Normal )
        {
            // draw Oxygen-way disabled button on inactive window
            buttonState=WinDeco::Disabled;
        }
        if( !(windowState & WinDeco::Alpha) && !(windowState & WinDeco::Maximized) )
        { y++; }

        WinDeco::Button button( settings(), helper(), type );
        button.setState(buttonState);
        int buttonSize=settings().buttonSize();
        button.render( context, x+(w-buttonSize)/2+1,y+(h-buttonSize)/2+1, buttonSize, buttonSize );
    }

    //__________________________________________________________________
    void Style::drawWindecoShapeMask( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h )
    {
        cairo_save(context);
        cairo_set_source_rgba(context,0,0,0,0);
        cairo_set_operator(context,CAIRO_OPERATOR_SOURCE);
        cairo_paint(context);

        cairo_set_source_rgba(context,1,1,1,1);
        cairo_set_operator(context,CAIRO_OPERATOR_OVER);
        cairo_set_antialias(context,CAIRO_ANTIALIAS_NONE);
        cairo_rounded_rectangle(context,x,y,w,h,6);
        cairo_fill(context);
        cairo_restore(context);

    }

    //__________________________________________________________________
    void Style::sanitizeSize( GdkWindow* window, gint& w, gint& h ) const
    {
        if( w < 0 && h < 0 ) gdk_drawable_get_size( window, &w, &h );
        else if( w < 0 ) gdk_drawable_get_size( window, &w, 0L );
        else if( h < 0 ) gdk_drawable_get_size( window, 0L, &h );
    }

    //__________________________________________________________________
    void Style::adjustScrollBarHole( int& x, int& y, int& w, int& h, StyleOptions options ) const
    {

        const int buttonSize( 14 );
        const int subLineOffset( buttonSize*settings().scrollBarSubLineButtons() );
        const int addLineOffset( buttonSize*settings().scrollBarAddLineButtons() );
        if( options&Vertical )
        {

            y += subLineOffset;
            h -= (subLineOffset+addLineOffset);

        } else {

            x += subLineOffset;
            w -= (subLineOffset+addLineOffset);

        }

        return;

    }

    //____________________________________________________________________________________
    void Style::renderActiveTab(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        StyleOptions options,
        TabOptions tabOptions
        )
    {

        const bool isFirstTabAligned( tabOptions & FirstTabAligned );
        const bool isLastTabAligned( tabOptions & LastTabAligned );

        // get color
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );

        // borders and connections to tabs
        // this is quite painfull and slipery code.
        // the same is true with oxygen-qt
        int offset = 2;
        int adjust = ( tabOptions&Mozilla ) ? 0:2;
        const TileSet::Tiles tabTiles( Style::tabTiles( side )  );

        SlabRect tabSlab;
        SlabRect::List slabs;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            {
                // main slab
                y += adjust; h -= 2*adjust;
                tabSlab = SlabRect( x, y-offset, w, h+9+offset, tabTiles );
                tabSlab._h+=1;

                if( isFirstTabAligned ) { tabSlab._x-=1; tabSlab._w+=1; }
                if( isLastTabAligned ) { tabSlab._w+=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x-1, y+h+offset-6, 8, 18, TileSet::Left ) );
                if( isLastTabAligned ) slabs.push_back( SlabRect( x+w-7, y+h+offset-6, 8, 18, TileSet::Right ) );

                break;
            }

            case GTK_POS_TOP:
            {

                // main slab
                y += adjust; h -= 2*adjust;
                tabSlab = SlabRect( x, y-9, w, h+11+offset, tabTiles );
                tabSlab._y-=1; tabSlab._h+=1;

                if( isFirstTabAligned ) { tabSlab._x-=1; tabSlab._w+=1; }
                if( isLastTabAligned ) { tabSlab._w-=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x-1, y-14, 8, 18, TileSet::Left ) );
                if( isLastTabAligned ) slabs.push_back( SlabRect( x+w-7, y-14, 8, 18, TileSet::Right ) );

                break;
            }

            case GTK_POS_RIGHT:
            {

                // main slab
                x += adjust; w -= 2*adjust;
                tabSlab = SlabRect( x-offset, y, w+9+offset, h, tabTiles );
                tabSlab._w+=1;
                if( isFirstTabAligned ) { tabSlab._y-=1; tabSlab._h+=1; }
                if( isLastTabAligned ) { tabSlab._h+=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x+w+offset-6, y-1, 18, 8, TileSet::Top ) );
                if( isLastTabAligned ) slabs.push_back( SlabRect( x+w+offset-6, y+h-7, 18, 8, TileSet::Top ) );

                break;
            }


            case GTK_POS_LEFT:
            {

                // main slab
                x += adjust; w -= 2*adjust;
                tabSlab = SlabRect( x-9, y, w+11+offset, h, tabTiles );
                tabSlab._x-=1; tabSlab._w+=1;
                if( isFirstTabAligned ) { tabSlab._y-=1; tabSlab._h+=1; }
                if( isLastTabAligned ) { tabSlab._h+=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x-14, y-1, 18, 8, TileSet::Top ) );
                if( isLastTabAligned )  slabs.push_back( SlabRect( x-14, y+h-7, 18, 8, TileSet::Top ) );

                break;

            }

            default: return;
        }

        // render tab
        helper().slab( base, 0 ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );

        // adjust rect for filling
        SlabRect fillSlab( tabSlab );
        fillSlab._x += 4;
        fillSlab._y += 4;
        fillSlab._w -= 8;
        fillSlab._h -= 8;

        // fill
        Cairo::Pattern pattern;
        int dimension = 0;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            dimension = fillSlab._h;
            fillSlab._h -= 2;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y, 0, fillSlab._y + fillSlab._h ) );
            break;

            case GTK_POS_TOP:
            dimension = fillSlab._h;
            fillSlab._y += 2;
            fillSlab._h -= 2;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y + fillSlab._h, 0, fillSlab._y ) );
            break;

            case GTK_POS_RIGHT:
            dimension = fillSlab._w;
            fillSlab._w -= 2;
            pattern.set( cairo_pattern_create_linear( fillSlab._x, 0, fillSlab._x + fillSlab._w, 0 ) );
            break;

            case GTK_POS_LEFT:
            dimension = fillSlab._w;
            fillSlab._x += 2;
            fillSlab._w -= 2;
            pattern.set( cairo_pattern_create_linear( fillSlab._x + fillSlab._w, 0, fillSlab._x, 0 ) );
            break;

            default: return;

        }

        cairo_pattern_add_color_stop( pattern, 0.1, ColorUtils::alphaColor( light, 0.5 ) );
        cairo_pattern_add_color_stop( pattern, 0.25, ColorUtils::alphaColor( light, 0.3 ) );
        cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( light, 0.2 ) );
        cairo_pattern_add_color_stop( pattern, 0.75, ColorUtils::alphaColor( light, 0.1 ) );
        cairo_pattern_add_color_stop( pattern, 0.9, ColorUtils::Rgba::transparent( light ) );

        // in firefox a solid background must be filled
        if( tabOptions&Mozilla )
        {
            cairo_set_source( context, base );
            cairo_rectangle( context, fillSlab._x, fillSlab._y, fillSlab._w, fillSlab._h );
            cairo_fill( context );
        }

        // draw pattern
        cairo_set_source( context, pattern );
        cairo_rectangle( context, fillSlab._x, fillSlab._y, fillSlab._w, fillSlab._h );
        cairo_fill( context );

        // render connections to frame
        for( SlabRect::List::const_iterator iter = slabs.begin(); iter != slabs.end(); ++iter )
        { helper().slab(base, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles ); }

    }

    //____________________________________________________________________________________
    void Style::renderInactiveTab_Single(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        StyleOptions options,
        TabOptions tabOptions
        )
    {

        // convenience flags
        const bool isFirstTabAligned( tabOptions & FirstTabAligned );
        const bool isLastTabAligned( tabOptions & LastTabAligned );

        // get color
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );

        // borders and connections to tabs
        // this is quite painfull and slipery code.
        // the same is true with oxygen-qt
        int offset = 2;
        int adjust = (tabOptions&Mozilla) ? 0:2;
        const TileSet::Tiles tabTiles( Style::tabTiles( side )  );

        SlabRect tabSlab;
        SlabRect::List slabs;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            {
                // main slab
                y += adjust; h -= 2*adjust;
                tabSlab = SlabRect( x, y-offset, w, h+9+offset, tabTiles );
                if( isFirstTabAligned ) { tabSlab._x-=1; tabSlab._w+=1; }
                if( isLastTabAligned ) { tabSlab._w+=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x-1, y+h+offset-7, 8, 17, TileSet::Left, Hover ) );
                if( isLastTabAligned ) slabs.push_back( SlabRect( x+w-7, y+h+offset-7, 8, 17, TileSet::Right, Hover ) );
                break;
            }

            case GTK_POS_TOP:
            {

                // main slab
                y += adjust; h -= 2*adjust;
                tabSlab = SlabRect( x, y-9, w, h+11+offset, tabTiles );
                if( isFirstTabAligned ) { tabSlab._x-=1; tabSlab._w+=1; }
                if( isLastTabAligned ) { tabSlab._w-=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x-1, y-13+offset, 8, 16, TileSet::Left, Hover ) );
                if( isLastTabAligned ) slabs.push_back( SlabRect( x+w-7, y-13+offset, 8, 16, TileSet::Right, Hover ) );
                break;
            }

            case GTK_POS_RIGHT:
            {

                // main slab
                x += adjust; w -= 2*adjust;
                tabSlab = SlabRect( x-offset, y, w+9+offset, h, tabTiles );
                if( isFirstTabAligned ) { tabSlab._y-=1; tabSlab._h+=1; }
                if( isLastTabAligned ) { tabSlab._h+=1; }

                // connections to frame
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x+w+offset-7, y-1, 17, 8, TileSet::Top, Hover ) );
                if( isLastTabAligned ) slabs.push_back( SlabRect( x+w+offset-7, y+h-7, 17, 8, TileSet::Top, Hover ) );
                break;
            }


            case GTK_POS_LEFT:
            {

                // main slab
                x += adjust; w -= 2*adjust;
                tabSlab = SlabRect( x-9, y, w+11+offset, h, tabTiles );
                if( isFirstTabAligned ) { tabSlab._y-=1; tabSlab._h+=1; }
                if( isLastTabAligned ) { tabSlab._h+=1; }

                // connections to frame
                SlabRect baseSlab( x-10+1, y-6, 10, h+12, TileSet::Right );
                if( isFirstTabAligned ) slabs.push_back( SlabRect( x-13 + offset, y-1, 16, 8, TileSet::Top, Hover ) );
                if( isLastTabAligned )slabs.push_back( SlabRect( x-13 + offset, y+h-7, 16, 8, TileSet::Top, Hover ) );
                break;
            }

            default: return;
        }

        // render tab
        const ColorUtils::Rgba glow( settings().palette().color( Palette::Hover ) );
        if( (options&Hover) )
        {

            helper().slabFocused( base, glow, 0 ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );

        } else {

            helper().slab( base, 0 ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );

        }

        // adjust rect for filling
        SlabRect fillSlab( tabSlab );
        fillSlab._x += 4;
        fillSlab._y += 4;
        fillSlab._w -= 8;
        fillSlab._h -= 8;

        // fill
        Cairo::Pattern pattern;
        int dimension = 0;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            dimension = fillSlab._h;
            fillSlab._h -= 3;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y, 0, fillSlab._y + fillSlab._h ) );
            break;

            case GTK_POS_TOP:
            dimension = fillSlab._h;
            fillSlab._y += 3;
            fillSlab._h -= 3;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y + fillSlab._h, 0, fillSlab._y ) );
            break;

            case GTK_POS_RIGHT:
            dimension = fillSlab._w;
            fillSlab._w -= 3;
            pattern.set( cairo_pattern_create_linear( fillSlab._x, 0, fillSlab._x + fillSlab._w, 0 ) );
            break;

            case GTK_POS_LEFT:
            dimension = fillSlab._w;
            fillSlab._x += 3;
            fillSlab._w -= 3;
            pattern.set( cairo_pattern_create_linear( fillSlab._x + fillSlab._w, 0, fillSlab._x, 0 ) );
            break;

            default: return;

        }

        cairo_pattern_add_color_stop( pattern, 0.0, ColorUtils::alphaColor( light, 0.1 ) );
        cairo_pattern_add_color_stop( pattern, 0.4, ColorUtils::alphaColor( dark, 0.5 ) );
        cairo_pattern_add_color_stop( pattern, 0.8, ColorUtils::alphaColor( dark, 0.4 ) );

        // draw pattern
        cairo_set_source( context, pattern );
        cairo_rectangle( context, fillSlab._x, fillSlab._y, fillSlab._w, fillSlab._h );
        cairo_fill( context );

        // render connections to frame
        for( SlabRect::List::const_iterator iter = slabs.begin(); iter != slabs.end(); ++iter )
        {

            if( (iter->_options&Hover) && (options&Hover) )
            {

                helper().slabFocused(base, glow, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles );

            } else {

                helper().slab(base, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles );

            }
        }
    }

    //____________________________________________________________________________________
    void Style::renderInactiveTab_Plain(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        StyleOptions options,
        TabOptions tabOptions
        )
    {
        // convenience flags
        const bool isFirstTabAligned( tabOptions & FirstTabAligned );
        const bool isLastTabAligned( tabOptions & LastTabAligned );

        const bool isLeftOfSelected( tabOptions & LeftOfSelected );
        const bool isRightOfSelected( tabOptions & RightOfSelected );

        // get color
        const ColorUtils::Rgba base( settings().palette().color( Palette::Window ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );

        // borders and connections to tabs
        // this is quite painfull and slipery code.
        // the same is true with oxygen-qt
        int offset = 2;
        int adjust = (tabOptions&Mozilla) ? 0:2;
        const TileSet::Tiles tabTiles( Style::tabTiles( side )  );

        SlabRect tabSlab;
        SlabRect::List slabs;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            {
                // main slab
                y += adjust; h -= 2*adjust;
                tabSlab = SlabRect( x, y-offset, w, h+10 + offset, tabTiles );
                if( isFirstTabAligned ) { tabSlab._x-=1; tabSlab._w+=1; }
                if( isLastTabAligned ) { tabSlab._w+=1; }

                // connections to frame
                SlabRect baseSlab( x-4-1, y+h-1, w+8+2, 10, TileSet::Top );
                if( isFirstTabAligned )
                {
                    baseSlab._x += 4; baseSlab._w -= 4; baseSlab._tiles |= TileSet::Left;
                    slabs.push_back( SlabRect( x-1, y+h+offset-6, 8, 16, TileSet::Left ) );
                }

                if( isLastTabAligned )
                {
                    baseSlab._w -= 4; baseSlab._tiles |= TileSet::Right;
                    slabs.push_back( SlabRect( x+w-7, y+h+offset-6, 8, 16, TileSet::Right ) );
                }

                if( isLeftOfSelected ) { baseSlab._w += 3; }
                else if( isRightOfSelected ) { baseSlab._x -= 3; baseSlab._w += 4; }
                else { baseSlab._w += 2; }

                slabs.push_back( baseSlab );
                break;
            }

            case GTK_POS_TOP:
            {

                // main slab
                y += adjust; h -= 2*adjust;
                tabSlab = SlabRect( x, y-10, w, h+10+offset, tabTiles );
                if( isFirstTabAligned ) { tabSlab._x-=1; tabSlab._w+=1; }
                if( isLastTabAligned ) { tabSlab._w-=1; }

                // connections to frame
                SlabRect baseSlab( x-4-1, y-10+1, w+8+2, 10, TileSet::Bottom );
                if( isFirstTabAligned ) { baseSlab._x += 4; baseSlab._w -= 4; baseSlab._tiles |= TileSet::Left; }
                if( isLastTabAligned ) { baseSlab._w -=4; baseSlab._tiles |= TileSet::Right; }
                if( isLeftOfSelected ) { baseSlab._w += 3; }
                else if( isRightOfSelected ) { baseSlab._x -= 3; baseSlab._w += 4; }
                else { baseSlab._w += 2; }

                slabs.push_back( baseSlab );
                break;
            }

            case GTK_POS_RIGHT:
            {

                // main slab
                x += adjust; w -= 2*adjust;
                tabSlab = SlabRect( x-offset, y, w+10+offset, h, tabTiles );
                if( isFirstTabAligned ) { tabSlab._y-=1; tabSlab._h+=1; }
                if( isLastTabAligned ) { tabSlab._h+=1; }

                // connections to frame
                SlabRect baseSlab( x+w-1, y-4-1, 10, h+8+3, TileSet::Left );
                if( isFirstTabAligned )
                {
                    baseSlab._y += 4; baseSlab._h -= 4; baseSlab._tiles |= TileSet::Top;
                    slabs.push_back( SlabRect( x+w+offset-6, y-1, 16, 8, TileSet::Top ) );
                }

                if( isLastTabAligned )
                {
                    baseSlab._h -= 4; baseSlab._tiles |= TileSet::Bottom;
                    slabs.push_back( SlabRect( x+w+offset-6, y+h-7, 16, 8, TileSet::Top ) );
                }

                if( isLeftOfSelected ) { baseSlab._h += 3; }
                else if( isRightOfSelected ) { baseSlab._y -= 3; baseSlab._h += 3; }
                else { baseSlab._h += 1; }

                slabs.push_back( baseSlab );
                break;
            }


            case GTK_POS_LEFT:
            {

                // main slab
                x += adjust; w -= 2*adjust;
                tabSlab = SlabRect( x-10, y, w+10+offset, h, tabTiles );
                if( isFirstTabAligned ) { tabSlab._y-=1; tabSlab._h+=1; }
                if( isLastTabAligned ) { tabSlab._h+=1; }

                // connections to frame
                SlabRect baseSlab( x-10+1, y-4-1, 10, h+8+3, TileSet::Right );
                if( isFirstTabAligned )
                {
                    baseSlab._y += 4; baseSlab._h -= 4; baseSlab._tiles |= TileSet::Top;
                    slabs.push_back( SlabRect( x-10+1 + offset-5, y-1, 16, 8, TileSet::Top ) );
                }

                if( isLastTabAligned )
                {
                    baseSlab._h -= 4; baseSlab._tiles |= TileSet::Bottom;
                    slabs.push_back( SlabRect( x-10+1 + offset-5, y+h-7, 16, 8, TileSet::Top ) );
                }

                if( isLeftOfSelected ) { baseSlab._h += 3; }
                else if( isRightOfSelected ) { baseSlab._y -= 3; baseSlab._h += 3; }
                else { baseSlab._h += 1; }

                slabs.push_back( baseSlab );
                break;
            }

            default: return;
        }

        const bool isFirstTab( tabOptions & FirstTab );
        const bool isLastTab( tabOptions & LastTab );

        const double radius( 5 );
        double xF( 0.5 + x );
        double yF( 0.5 + y );
        double wF( w-1 );
        double hF( h-1 );

        switch( side )
        {

            case GTK_POS_BOTTOM:
            {
                xF += 1.0;
                wF -= 1.0;
                hF += 2;
                if( isLeftOfSelected ) wF += 1;
                else if( isRightOfSelected ) { xF -= 2; wF += 2; }

                if( isFirstTab )
                {

                    if( isFirstTabAligned ) cairo_move_to( context, xF, yF + hF + 2 );
                    else cairo_move_to( context, xF, yF + hF );

                    cairo_line_to( context, xF, yF + radius );
                    cairo_arc( context, xF + radius, yF + radius, radius, M_PI, 3.0*M_PI/2 );
                    cairo_line_to( context, xF + wF, yF );
                    cairo_line_to( context, xF + wF, yF + hF );

                } else if( isLastTab ) {

                    cairo_move_to( context, xF, yF + hF );
                    cairo_line_to( context, xF, yF );
                    cairo_line_to( context, xF + wF - radius, yF );
                    cairo_arc( context, xF + wF - radius, yF + radius, radius, 3.0*M_PI/2, 2.0*M_PI );
                    if( isLastTabAligned ) cairo_line_to( context, xF + wF, yF + hF + 2 );
                    else cairo_line_to( context, xF + wF, yF + hF );

                } else {

                    cairo_move_to( context, xF, yF + hF );
                    cairo_line_to( context, xF, yF );
                    cairo_line_to( context, xF + wF, yF );
                    cairo_line_to( context, xF + wF, yF + hF );

                }

            }

            break;

            case GTK_POS_TOP:
            {
                xF += 1.0;
                wF -= 1.0;
                yF -= 1;
                hF += 1;
                if( isLeftOfSelected ) wF += 1;
                else if( isRightOfSelected ) { xF -= 2; wF += 2; }


                if( isFirstTab )
                {

                    cairo_move_to( context, xF+wF, yF );
                    cairo_line_to( context, xF+wF, yF + hF );
                    cairo_line_to( context, xF+radius, yF + hF );
                    cairo_arc( context, xF+radius, yF + hF -radius, radius, M_PI/2, M_PI );
                    if( isFirstTabAligned ) cairo_line_to( context, xF, yF - 2 );
                    else cairo_line_to( context, xF, yF );

                } else if( isLastTab ) {

                    if( isLastTabAligned ) cairo_move_to( context, xF+wF, yF-2 );
                    else  cairo_move_to( context, xF+wF, yF-2 );
                    cairo_line_to( context, xF+wF, yF+hF-radius );
                    cairo_arc( context, xF+wF-radius, yF+hF-radius, radius, 0, M_PI/2 );
                    cairo_line_to( context, xF, yF+hF );
                    cairo_line_to( context, xF, yF );

                } else {

                    cairo_move_to( context, xF+wF, yF );
                    cairo_line_to( context, xF+wF, yF + hF );
                    cairo_line_to( context, xF, yF+hF );
                    cairo_line_to( context, xF, yF );

                }

            }

            break;

            case GTK_POS_RIGHT:
            {

                yF += 1.0;
                hF -= 1.0;
                wF += 2;

                if( isLeftOfSelected ) hF += 1;
                else if( isRightOfSelected ) { yF -= 2; hF += 2; }

                if( isFirstTab )
                {

                    cairo_move_to( context, xF+wF, yF+hF );
                    cairo_line_to( context, xF, yF+hF );
                    cairo_line_to( context, xF, yF+radius );
                    cairo_arc( context, xF+radius, yF+radius, radius, M_PI, 3.0*M_PI/2 );
                    if( isFirstTabAligned ) cairo_line_to( context, xF+wF+2, yF );
                    else cairo_line_to( context, xF+wF, yF );

                } else if( isLastTab ) {

                    if( isLastTabAligned ) cairo_line_to( context, xF + wF + 2, yF + hF );
                    else cairo_line_to( context, xF + wF, yF + hF );
                    cairo_line_to( context, xF+radius, yF+hF );
                    cairo_arc( context, xF+radius, yF+hF - radius, radius, M_PI/2, M_PI );
                    cairo_line_to( context, xF, yF );
                    cairo_line_to( context, xF + wF, yF );

                } else {

                    cairo_move_to( context, xF+wF, yF+hF );
                    cairo_line_to( context, xF, yF+hF );
                    cairo_line_to( context, xF, yF );
                    cairo_line_to( context, xF+wF, yF );

                }
            }
            break;

            case GTK_POS_LEFT:
            {
                yF += 1.0;
                hF -= 1.0;
                xF -= 2;
                wF += 2;

                if( isLeftOfSelected ) hF += 1;
                else if( isRightOfSelected ) { yF -= 2; hF += 2; }

                if( isFirstTab )
                {

                    if( isFirstTabAligned ) cairo_move_to( context, xF-2, yF );
                    else cairo_move_to( context, xF, yF );
                    cairo_line_to( context, xF + wF - radius, yF );
                    cairo_arc( context, xF + wF - radius, yF + radius, radius, 3.0*M_PI/2, 2*M_PI );
                    cairo_line_to( context, xF+wF, yF+hF );
                    cairo_line_to( context, xF, yF+hF );

                } else if( isLastTab ) {

                    cairo_move_to( context, xF, yF );
                    cairo_line_to( context, xF+wF, yF );
                    cairo_line_to( context, xF+wF, yF + hF - radius );
                    cairo_arc( context, xF+wF-radius, yF + hF - radius, radius, 0, M_PI/2 );
                    if( isLastTabAligned ) cairo_line_to( context, xF-2, yF+hF );
                    else cairo_line_to( context, xF, yF+hF );

                } else {

                    cairo_move_to( context, xF, yF );
                    cairo_line_to( context, xF+wF, yF );
                    cairo_line_to( context, xF+wF, yF+hF );
                    cairo_line_to( context, xF, yF+hF );

                }
            }
            break;

            default: return;

        }

        ColorUtils::Rgba backgroundColor( base );
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            if( wh > 0 )
            {  backgroundColor = ColorUtils::backgroundColor( settings().palette().color( Palette::Window ), wh, y+wy+h/2 ); }

        }

        const ColorUtils::Rgba midColor( ColorUtils::alphaColor( ColorUtils::darkColor( backgroundColor ), 0.4 ) );
        const ColorUtils::Rgba darkColor( ColorUtils::alphaColor( ColorUtils::darkColor( backgroundColor ), 0.8 ) );

        cairo_set_line_width( context, 1.0 );
        cairo_set_source( context, midColor );
        cairo_fill_preserve( context );

        cairo_set_source( context, darkColor );
        cairo_stroke( context );

        for( SlabRect::List::const_iterator iter = slabs.begin(); iter != slabs.end(); ++iter )
        {
            // render tab
            if( options&Hover )
            {

                const ColorUtils::Rgba glow( settings().palette().color( Palette::Hover ) );
                helper().slabFocused(base, glow, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles );

            } else {

                helper().slab(base, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles );

            }

        }

    }

    //____________________________________________________________________________________
    ColorUtils::Rgba Style::slabShadowColor( StyleOptions options ) const
    {

        ColorUtils::Rgba glow;
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );

        const bool enabled( !(options&Disabled ) );
        if( enabled && (options & Hover) ) glow = settings().palette().color( group, Palette::Hover );
        else if( enabled && (options & Focus) ) glow =  settings().palette().color( group, Palette::Focus );
        return glow;

    }

    //__________________________________________________________________
    void Style::renderSlab( Cairo::Context& context, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& base, StyleOptions options, TileSet::Tiles tiles )
    {

        // do nothing if not enough room
        if( w<14 || h<14 ) return;

        // additional adjustment for sunken frames
        // TODO: double check
        if( options & Sunken)
        {

            x -= 1;
            w += 2;
            h += 2;

        }

        // fill
        if( !(options & NoFill))
        {

            Cairo::Pattern pattern;
            const ColorUtils::Rgba shadow( ColorUtils::shadowColor( base ) );
            if( shadow.value() > base.value() && (options & Sunken) )
            {

                pattern.set( cairo_pattern_create_linear( 0, y, 0, y+2*h ) );
                cairo_pattern_add_color_stop( pattern, 0, base );
                cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::lightColor( base ) );

            } else {

                pattern.set( cairo_pattern_create_linear( 0, y-h, 0, y+h ) );
                cairo_pattern_add_color_stop( pattern, 0, ColorUtils::lightColor( base ) );
                cairo_pattern_add_color_stop( pattern, 1.0, base );

            }

            cairo_set_source( context, pattern );
            helper().fillSlab( context, x, y, w, h, tiles );

        }

        if( !(options&Sunken) ) {

            // calculate glow color
            const TileSet* tile;
            ColorUtils::Rgba glow( slabShadowColor( options ) );
            if( glow.isValid() ) tile = &helper().slabFocused( base, glow , 0);
            else if( base.isValid() ) tile = &helper().slab( base, 0 );
            else return;

            if( tile ) tile->render( context, x, y, w, h );

        } else if( base.isValid() ) {

            helper().slabSunken( base, 0 ).render( context, x, y, w, h );

        }

    }

    //__________________________________________________________________
    void Style::renderScrollBarHole( Cairo::Context& context, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& base, bool vertical, TileSet::Tiles tiles )
    {

        // use tileset from helper
        helper().scrollHole( base, vertical ).render( context, x, y, w, h, tiles );

    }

    //__________________________________________________________________
    Polygon Style::genericArrow( GtkArrowType orientation, QtSettings::ArrowSize size ) const
    {

        Polygon a;
        switch( orientation )
        {

            case GTK_ARROW_UP:
            {
                if( size == QtSettings::ArrowTiny ) a << Point( -1.75, 1.125 ) << Point( 0.5, -1.125 ) << Point( 2.75, 1.125 );
                else if( size == QtSettings::ArrowSmall ) a << Point( -2,1.5 ) << Point( 0.5, -1.5 ) << Point( 3,1.5 );
                else a << Point( -3,2.5 ) << Point( 0.5, -1.5 ) << Point( 4,2.5 );
                break;
            }

            case GTK_ARROW_DOWN:
            {
                if( size == QtSettings::ArrowTiny ) a << Point( -1.75, -1.125 ) << Point( 0.5, 1.125 ) << Point( 2.75, -1.125 );
                else if( size == QtSettings::ArrowSmall ) a << Point( -2,-1.5 ) << Point( 0.5, 1.5 ) << Point( 3,-1.5 );
                else a << Point( -3,-1.5 ) << Point( 0.5, 2.5 ) << Point( 4,-1.5 );
                break;
            }

            case GTK_ARROW_LEFT:
            {
                if( size == QtSettings::ArrowTiny ) a << Point( 1.125, -1.75 ) << Point( -1.125, 0.5 ) << Point( 1.125, 2.75 );
                else if( size == QtSettings::ArrowSmall ) a << Point( 1.5,-2 ) << Point( -1.5, 0.5 ) << Point( 1.5,3 );
                else a << Point( 2.5,-3 ) << Point( -1.5, 0.5 ) << Point( 2.5,4 );
                break;
            }

            case GTK_ARROW_RIGHT:
            {
                if( size == QtSettings::ArrowTiny ) a << Point( -1.125, -1.75 ) << Point( 1.125, 0.5 ) << Point( -1.125, 2.75 );
                else if( size == QtSettings::ArrowSmall ) a << Point( -1.5,-2 ) << Point( 1.5, 0.5 ) << Point( -1.5,3 );
                else a << Point( -1.5,-3 ) << Point( 2.5, 0.5 ) << Point( -1.5,4 );
                break;
            }

            default: break;

        }

        return a;



    }

    //__________________________________________________________________
    cairo_pattern_t* Style::verticalGradient( const ColorUtils::Rgba& base, int y1, int y2 ) const
    {

        ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
        ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );
        cairo_pattern_t* pattern = cairo_pattern_create_linear( 0, y1, 0, y2 );
        cairo_pattern_add_color_stop( pattern, 0, top );
        cairo_pattern_add_color_stop( pattern, 0.5, base );
        cairo_pattern_add_color_stop( pattern, 1, bottom );
        return pattern;
    }

    //__________________________________________________________________
    cairo_pattern_t* Style::radialGradient( const ColorUtils::Rgba& base, int x, int y, int r ) const
    {
        // create radial pattern
        ColorUtils::Rgba radial( ColorUtils::backgroundRadialColor( base ) );
        cairo_pattern_t* pattern( cairo_pattern_create_radial( x, y, r ) );
        cairo_pattern_add_color_stop( pattern, 0, radial );
        cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::alphaColor( radial, 101.0/255 ) );
        cairo_pattern_add_color_stop( pattern, 0.75, ColorUtils::alphaColor( radial, 37.0/255 ) );
        cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( radial, 0 ) );
        return pattern;
    }

    //__________________________________________________________________
    void Style::renderWindowDots(cairo_t* context, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& color, WinDeco::Options wopt)
    {
        bool isMaximized( wopt & WinDeco::Maximized );
        bool hasAlpha( wopt & WinDeco::Alpha );
        int offset( hasAlpha ? 0 : -1 );
        if( settings().frameBorder() >= QtSettings::BorderTiny )
        {
            if( !isMaximized )
            {
                // Draw right side 3-dots resize handles
                int cenY = int(h/2+y);
                int posX = int(w+x-3) + 1;
                helper().renderDot(context,color,posX+offset, cenY-3);
                helper().renderDot(context,color,posX+offset, cenY);
                helper().renderDot(context,color,posX+offset, cenY+3);
            }

            // Draw bottom-right corner 3-dots resize handles
            // if( !config.drawSizeGrip )
            {
                cairo_save(context);
                cairo_translate(context,x+w-8,y+h-8);
                helper().renderDot(context,color,2+offset,6+offset);
                helper().renderDot(context,color,5+offset,5+offset);
                helper().renderDot(context,color,6+offset,2+offset);
                cairo_restore(context);
            }
        }
    }

    //__________________________________________________________________
    void Style::centerRect( GdkRectangle* parent, GdkRectangle* child ) const
    {
        if( !( parent && child ) ) return;
        child->x = parent->x + (parent->width - child->width)/2;
        child->y = parent->y + (parent->height - child->height)/2;
        return;
    }

    //__________________________________________________________________
    void Style::generateGapMask( Cairo::Context& context, gint x, gint y, gint w, gint h, const Gtk::Gap& gap ) const
    {

        if( gap.width() <= 0 ) return;

        // store current rect in
        GdkRectangle r = { x, y, w, h };
        GdkRegion* region = gdk_region_rectangle( &r );

        GdkRectangle mask_r;
        GdkRegion* mask = 0L;
        switch( gap.position() )
        {
            case GTK_POS_TOP:
            {
                mask_r = Gtk::gdk_rectangle( x+gap.x(), y, gap.width(), gap.height() );
                mask = gdk_region_rectangle( &mask_r );
                break;
            }

            case GTK_POS_BOTTOM:
            {
                mask_r = Gtk::gdk_rectangle( x+gap.x(), y+h-gap.height(), gap.width(), gap.height() );
                mask = gdk_region_rectangle( &mask_r );
                break;
            }

            case GTK_POS_LEFT:
            {
                mask_r = Gtk::gdk_rectangle( x, y+gap.x(), gap.height(), gap.width() );
                mask = gdk_region_rectangle( &mask_r );
                break;
            }

            case GTK_POS_RIGHT:
            {
                mask_r = Gtk::gdk_rectangle( x + w - gap.height(), y+gap.x(), gap.height(), gap.width() );
                mask = gdk_region_rectangle( &mask_r );
                break;
            }

            default: return;
        }

        if( false )
        {
            cairo_set_source( context, ColorUtils::Rgba( 1, 0, 0, 0.3 ) );
            gdk_cairo_rectangle( context, &mask_r );
            cairo_fill( context );
        }

        gdk_region_subtract( region, mask );
        gdk_cairo_region( context, region );
        cairo_clip( context );

        gdk_region_destroy( region );
        gdk_region_destroy( mask );

        return;

    }

}
