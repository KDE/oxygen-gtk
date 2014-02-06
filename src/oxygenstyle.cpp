/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "oxygenfontinfo.h"
#include "oxygengtkutils.h"
#include "oxygenmetrics.h"
#include "oxygenwindecobutton.h"
#include "oxygenwindowshadow.h"

#include "oxygengtktypenames.h"

#include <algorithm>
#include <cmath>

#ifdef GDK_WINDOWING_X11
#include <X11/Xatom.h>
#endif

namespace Oxygen
{

    //__________________________________________________________________
    Style* Style::_instance = 0;
    Style& Style::instance( void )
    {
        if( !_instance )
        {
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::Style::instance - creating new style." << std::endl;
            #endif

            _instance = new Style();
        }

        return *_instance;
    }

    //__________________________________________________________________
    Style::Style( void )
    {
        #ifdef GDK_WINDOWING_X11
        _blurAtom = None;
        #endif
    }

    //__________________________________________________________________
    bool Style::initialize( unsigned int flags )
    {

        // initialize ref surface
        _helper.initializeRefSurface();

        // reinitialize settings
        if( !_settings.initialize( flags ) ) return false;

        // reset caches if colors have changed
        if( flags&QtSettings::Colors )
        {
            _helper.clearCaches();
            ColorUtils::clearCaches();
        }

        // connect files
        QtSettings::FileMap& monitoredFiles( _settings.monitoredFiles() );
        for( QtSettings::FileMap::iterator iter = monitoredFiles.begin(); iter != monitoredFiles.end(); ++iter )
        {
            if( !iter->second.signal.isConnected() )
            { iter->second.signal.connect( G_OBJECT( iter->second.monitor ), "changed", G_CALLBACK(fileChanged), this ); }
        }

        // reinitialize animations
        _animations.initialize( _settings );

        if( flags&QtSettings::Oxygen )
        {
            // pass window drag mode to window manager
            if( !_settings.windowDragEnabled() ) _windowManager.setDragMode( WindowManager::Disabled );
            else if( _settings.windowDragMode() == QtSettings::WD_MINIMAL ) _windowManager.setDragMode( WindowManager::Minimal );
            else _windowManager.setDragMode( WindowManager::Full );

            // use window manager to handle window drag
            _windowManager.setUseWMMoveResize( _settings.useWMMoveResize() );

        }

        if( flags&QtSettings::KdeGlobals )
        {
            // pass drag distance and delay to window manager
            _windowManager.setDragDistance( _settings.startDragDist() );
            _windowManager.setDragDelay( _settings.startDragTime() );
        }

        // background surface
        if( !_settings.backgroundPixmap().empty() ) setBackgroundSurface( _settings.backgroundPixmap() );

        // create window shadow
        WindowShadow shadow( _settings, _helper );
        _shadowHelper.setApplicationName( _settings.applicationName() );
        _shadowHelper.initialize( _settings.palette().color(Palette::Window), shadow );

        #ifdef GDK_WINDOWING_X11
        if( _blurAtom == None )
        {

            GdkDisplay *display( gdk_display_get_default() );
            if( display )
            { _blurAtom = XInternAtom(GDK_DISPLAY_XDISPLAY( display ),"_KDE_NET_WM_BLUR_BEHIND_REGION",False); }

        }

        #endif

        return true;

    }

    //__________________________________________________________________
    Cairo::Surface Style::tabCloseButton( const StyleOptions& options )
    {

        // active tab
        if( options&Focus )
        {
            // create button
            if( !_tabCloseButtons.active )
            {
                const std::string filename( std::string(GTK_THEME_DIR)+ "/special-icons/standardbutton-closetab-down-16.png" );
                _tabCloseButtons.active = Cairo::Surface( cairo_image_surface_create_from_png( filename.c_str() ) );
            }

            return _tabCloseButtons.active;
        }

        // prelight
        if( options&Hover )
        {
            // create button
            if( !_tabCloseButtons.prelight )
            {
                const std::string filename( std::string(GTK_THEME_DIR) + "/special-icons/standardbutton-closetab-hover-16.png" );
                _tabCloseButtons.prelight = Cairo::Surface( cairo_image_surface_create_from_png( filename.c_str() ) );
            }

            return _tabCloseButtons.prelight;

        }

        // normal or inactive
        if( !_tabCloseButtons.normal )
        {
            const std::string filename( std::string(GTK_THEME_DIR) + "/special-icons/standardbutton-closetab-16.png" );
            _tabCloseButtons.normal = Cairo::Surface( cairo_image_surface_create_from_png( filename.c_str() ) );
        }

        // inactive
        if( (options&Disabled) && _tabCloseButtons.normal )
        {

            if( !_tabCloseButtons.inactive )
            {

                // make deep copy of the normal image
                _tabCloseButtons.inactive = Cairo::Surface( cairo_surface_copy( _tabCloseButtons.normal ) );
                cairo_surface_add_alpha(  _tabCloseButtons.inactive, 0.5 );
                cairo_image_surface_saturate( _tabCloseButtons.inactive, 0.1 );

            }

            return _tabCloseButtons.inactive;

        }

        // fallback to normal
        return _tabCloseButtons.normal;

    }

    //____________________________________________________________________________________
    bool Style::hasBackgroundSurface( void ) const
    {
        if( !_backgroundSurface.isValid() ) return false;
        const cairo_status_t status( cairo_surface_status( _backgroundSurface ) );
        return
            status != CAIRO_STATUS_NO_MEMORY &&
            status != CAIRO_STATUS_FILE_NOT_FOUND &&
            status != CAIRO_STATUS_READ_ERROR;
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
    void Style::outline( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& color ) const
    {

        // define colors
        Cairo::Context context( window, clipRect );
        cairo_rectangle( context, 0.5+x, 0.5+y, w-1, h-1 );
        cairo_set_line_width( context, 1 );
        cairo_set_source( context, color );
        cairo_stroke( context );

    }

    //__________________________________________________________________
    void Style::drawSeparator( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const StyleOptions& options )
    {

        // define colors
        ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
        if( options & Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            if( wh > 0 )
            {
                if( options & Menu ) base = ColorUtils::menuBackgroundColor( _settings.palette().color( Palette::Window ), wh, y+wy+h/2 );
                else base = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wh, y+wy+h/2 );
            }

        }

        Cairo::Context context( window, clipRect );
        _helper.drawSeparator( context, base, x, y, w, h, (options&Vertical) );

    }

    //__________________________________________________________________
    bool Style::renderWindowBackground(
        cairo_t* context, GdkWindow* window, GtkWidget* widget,
        GdkRectangle* clipRect, gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        bool isMaximized )
    {
        if( _settings.useBackgroundGradient() )
        {

            if( !renderBackgroundGradient( context, window, widget, clipRect, x, y, w, h, options, isMaximized ) )
            { return false; }

        } else {

            ColorUtils::Rgba base( color( Palette::Window, options ) );
            const bool needToDestroyContext( !context );
            if( !context )
            {

                // create context
                context = gdk_cairo_create(window);

                // set clip rect
                if( clipRect )
                {
                    cairo_rectangle(context,clipRect->x,clipRect->y,clipRect->width,clipRect->height);
                    cairo_clip(context);
                }

            } else cairo_save( context );

            cairo_set_source(context,base);
            cairo_rectangle(context,x,y,w,h);
            cairo_fill(context);

            if( needToDestroyContext )
            {
                cairo_destroy(context);
                context = 0;
            }
            else cairo_restore(context);

        }

        // background pixmap
        if( hasBackgroundSurface() )
        { renderBackgroundPixmap( context, window, widget, clipRect, x, y, w, h, isMaximized ); }

        return true;

    }

    //__________________________________________________________________
    bool Style::renderBackgroundGradient(
        cairo_t* context, GdkWindow* window, GtkWidget* widget,
        GdkRectangle* clipRect, gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        bool isMaximized )
    {

        // define colors
        ColorUtils::Rgba base( color( Palette::Window, options ) );
        bool renderingWindeco(context && !window);

        // the hard-coded metrics are copied for
        // kdebase/workspace/libs/oxygen/oxygenhelper.cpp
        // vertical shift to account for window decoration
        const int yShift = 23;

        // toplevel window information and relative positioning
        gint ww(0), wh(0);
        gint wx(0), wy(0);

        // if we aren't going to draw window decorations...
        bool needToDestroyContext( false );
        if( renderingWindeco )
        {
            // drawing window decorations, so logic is simplified
            ww=w;
            wh=h;
            cairo_save(context);
            cairo_translate(context,x,y);
            x=0;
            y=0;

        } else {


            if( !context )
            {
              // create context and translate to toplevel coordinates
              // make it the old good way since context is cairo_t* instead Cairo::Context
              context = gdk_cairo_create(window);
              needToDestroyContext=true;

              if( clipRect )
              {
                  cairo_rectangle(context,clipRect->x,clipRect->y,clipRect->width,clipRect->height);
                  cairo_clip(context);
              }

            } else cairo_save( context );

            // get window dimension and position
            // paint flat background when mapping failed
            if( !Gtk::gdk_map_to_toplevel( window, widget, &wx, &wy, &ww, &wh, true ) )
            {

                #if OXYGEN_DEBUG
                std::cerr << "Oxygen::Style::renderBackgroundGradient - map_to_toplevel failed" << std::endl;
                std::cerr << "original xywh: ("<<x<<","<<y<<","<<w<<","<<h<<")\n";
                #endif

                if( _settings.applicationName().isOpenOffice() && GTK_IS_WINDOW(widget))
                {
                    gtk_window_get_size(GTK_WINDOW(widget),&ww,&wh);
                    wx=0;
                    wy=0;
                    cairo_translate(context,x,y);
                    if(clipRect)
                    {
                        #if OXYGEN_DEBUG
                        std::cerr << "original clipRect: " << *clipRect << std::endl;
                        #endif
                        clipRect->x-=x;
                        clipRect->y-=y;
                        clipRect->width-=x;
                        clipRect->height-=y;
                    }
                    x=y=0;
                    #if OXYGEN_DEBUG
                    std::cerr <<"Oxygen::Style::renderBackgroundGradient - setting openoffice-specific coords:"<<wx<<","<<wy<<","<<ww<<","<<wh<<"\n\n";
                    #endif
                }
                else
                {
                    // flat painting for all other apps
                    cairo_set_source(context,base);
                    cairo_rectangle(context,x,y,w,h);
                    cairo_fill(context);
                    if( needToDestroyContext ) cairo_destroy(context);
                    else cairo_restore(context);
                    return false;
                }

            }

            // translate to toplevel coordinates
            wy += yShift;
            x+=wx;
            y+=wy;

            // no sense in context saving since it will be destroyed
            cairo_translate( context, -wx, -wy );

        }

        if(options&DrawAlphaChannel)
        {
            base.setAlpha(_settings.backgroundOpacity()/255.);
            cairo_set_operator(context,CAIRO_OPERATOR_SOURCE);
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

            const Cairo::Surface& surface( _helper.verticalGradient( base, splitY ) );
            cairo_set_source_surface( context, surface, 0, 0 );
            cairo_pattern_set_extend( cairo_get_source( context ), CAIRO_EXTEND_REPEAT );
            gdk_cairo_rectangle( context, &upperRect );
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

        // gradient should be rendered with full opacity
        base.setAlpha(1);
        cairo_set_operator(context,CAIRO_OPERATOR_OVER);

        // radial pattern
        const int patternHeight = 64;
        const int radialW( std::min(600, ww ) );

        GdkRectangle radialRect = {  (ww - radialW)/2, 0, radialW, patternHeight };
        if( gdk_rectangle_intersect( &rect, &radialRect, &radialRect ) )
        {

            const Cairo::Surface& surface( _helper.radialGradient( base, 64 ) );
            cairo_set_source_surface( context, surface, 0, 0 );

            // add matrix transformation
            cairo_matrix_t transformation;
            cairo_matrix_init_identity( &transformation );
            cairo_matrix_scale( &transformation, 128.0/radialW, 1.0 );
            cairo_matrix_translate( &transformation, -(ww - radialW)/2, 0 );
            cairo_pattern_set_matrix( cairo_get_source( context ), &transformation );

            gdk_cairo_rectangle( context, &radialRect );
            cairo_fill( context );

        }

        if(needToDestroyContext) cairo_destroy(context);
        else cairo_restore(context);

        return true;

    }

    //__________________________________________________________________
    bool Style::renderBackgroundPixmap(
        cairo_t* context, GdkWindow* window, GtkWidget* widget,
        GdkRectangle* clipRect, gint x, gint y, gint w, gint h,
        bool isMaximized )
    {

        // define colors
        bool renderingWindeco(context && !window);

        // the hard-coded metrics are copied for
        // kdebase/workspace/libs/oxygen/oxygenhelper.cpp
        // vertical shift to account for window decoration
        const int yShift = 23;

        // toplevel window information and relative positioning
        gint ww(0), wh(0);
        gint wx(0), wy(0);

        // if we aren't going to draw window decorations...
        bool needToDestroyContext( false );
        if( renderingWindeco )
        {
            // drawing window decorations, so logic is simplified
            ww=w;
            wh=h;
            cairo_save(context);
            cairo_translate(context,x,y);
            x=0;
            y=0;

        } else {

            if( !context )
            {
              // create context and translate to toplevel coordinates
              // make it the old good way since context is cairo_t* instead Cairo::Context
              context = gdk_cairo_create(window);
              needToDestroyContext=true;

              if( clipRect )
              {
                  cairo_rectangle(context,clipRect->x,clipRect->y,clipRect->width,clipRect->height);
                  cairo_clip(context);
              }

            } else cairo_save( context );

            // get window dimension and position
            // paint flat background when mapping failed
            if( !Gtk::gdk_map_to_toplevel( window, widget, &wx, &wy, &ww, &wh, true ) )
            {
                if( needToDestroyContext ) cairo_destroy(context);
                else cairo_restore(context);
                return false;
            }

            // translate to toplevel coordinates
            wy += yShift;
            x+=wx;
            y+=wy;

            // no sense in context saving since it will be destroyed
            cairo_translate( context, -wx, -wy );

        }

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

        // Additional clip constraint so that no extra space is filled (important for LibreOffice)
        cairo_rectangle(context,x,y,w,h);
        cairo_clip(context);

        if(renderingWindeco)
        {
            // Take border sizes into account
            int bgShiftX=isMaximized?0:WinDeco::getMetric(WinDeco::BorderLeft);
            int bgShiftY=WinDeco::getMetric(WinDeco::BorderTop)-yShift;
            cairo_translate(context,bgShiftX,bgShiftY);
        }

        // no sense in context saving since it will be either destroyed or restored to earlier state
        cairo_translate( context, -40, -(48-20) );
        cairo_set_source_surface( context, _backgroundSurface, 0, 0 );
        cairo_rectangle( context, 0, 0, ww + wx + 40, wh + wy + 48 - 20 );
        cairo_fill( context );

        if(needToDestroyContext) cairo_destroy(context);
        else cairo_restore(context);

        return true;

    }

    //__________________________________________________________________
    bool Style::renderGroupBoxBackground(
        cairo_t* context,
        GdkWindow* window, GtkWidget* widget,
        GdkRectangle* clipRect, gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        TileSet::Tiles tiles )
    {

        // find groupbox parent
        GtkWidget* parent( Gtk::gtk_parent_groupbox( widget ) );
        if( !( parent && _animations.groupBoxEngine().contains( parent ) ) ) return false;

        // toplevel window information and relative positioning
        gint ww(0), wh(0);
        gint wx(0), wy(0);

        // map to parent
        if( !Gtk::gtk_widget_map_to_parent( widget, parent, &wx, &wy, &ww, &wh ) )
        { return false; }

        // create context and translate
        bool needToDestroyContext( false );
        if( !context )
        {
            // create context and translate to toplevel coordinates
            // make it the old good way since context is cairo_t* instead Cairo::Context
            context = gdk_cairo_create(window);
            needToDestroyContext=true;

            if( clipRect )
            {
              cairo_rectangle(context,clipRect->x,clipRect->y,clipRect->width,clipRect->height);
              cairo_clip(context);
            }

        } else cairo_save( context );

        const int margin( 1 );
        wh += 2*margin;
        ww += 2*margin;
        x+=wx;
        y+=wy;
        cairo_translate( context, -wx, -wy );

        // define colors
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wwh, wwy;
            Gtk::gtk_widget_map_to_toplevel( parent, 0L, &wwy, 0L, &wwh );
            base = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wwh, wwy-1+wh/2 );

        } else {

            base = _settings.palette().color( Palette::Window );

        }

        const int xGroupBox = x - wx - margin;
        const int yGroupBox = y - wy - margin;
        renderGroupBox( context, base, xGroupBox, yGroupBox, ww, wh, options );

        if(needToDestroyContext) cairo_destroy(context);
        else cairo_restore(context);

        return true;

    }

    //__________________________________________________________________
    bool Style::renderMenuBackground( GdkWindow* window, Cairo::Context& context, gint x, gint y, gint w, gint h, const StyleOptions& options ) const
    {
        // define colors
        ColorUtils::Rgba base( color( Palette::Window, options ) );
        ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
        ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );

        // get window dimension and position
        gint ww, wh;
        gint wx, wy;
        if( !Gtk::gdk_map_to_toplevel( window, 0L, &wx, &wy, &ww, &wh, true ) )
        { return false; }

        // translate to toplevel coordinates
        x+=wx;
        y+=wy;

        cairo_translate( context, -wx, -wy );
        const bool hasAlpha( options&Alpha );
        const bool isMenu( options&Menu );
        const bool round( options&Round );

        GdkRectangle rect = { x, y, w, h };

        // paint translucent first
        if( hasAlpha )
        {
            cairo_rectangle( context, 0, 0, ww, wh );
            cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
            cairo_set_source( context, ColorUtils::alphaColor( base, 0 ) );
            cairo_fill( context );

            if(_settings.backgroundOpacity()<255)
            {
                double opacity(_settings.backgroundOpacity()/255.);
                top.setAlpha(opacity);
                bottom.setAlpha(opacity);
            }
        }

        const int splitY( std::min(200, 3*wh/4 ) );
        const int verticalOffset( (isMenu && round) ? Menu_VerticalOffset:0 );

        GdkRectangle upperRect = { 0, verticalOffset, ww, splitY - verticalOffset };
        if( gdk_rectangle_intersect( &rect, &upperRect, &upperRect ) )
        {
            // upper rect
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, splitY ) );
            cairo_pattern_add_color_stop( pattern, 0, top );
            cairo_pattern_add_color_stop( pattern, 1, bottom );

            gdk_cairo_rounded_rectangle( context, &upperRect, 3.5, round ? CornersTop:CornersNone );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        GdkRectangle lowerRect = { 0, splitY, ww, wh-splitY - verticalOffset };
        if( gdk_rectangle_intersect( &rect, &lowerRect, &lowerRect ) )
        {

            // lower part
            gdk_cairo_rounded_rectangle( context, &lowerRect, 3.5, round ? CornersBottom:CornersNone );
            cairo_set_source( context, bottom );
            cairo_fill( context );

        }

        return true;

    }

    //__________________________________________________________________
    void Style::renderTooltipBackground( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const StyleOptions& options ) const
    {

        // define colors
        ColorUtils::Rgba base(_settings.palette().color( Palette::Tooltip ) );
        ColorUtils::Rgba top( ColorUtils::backgroundTopColor( base ) );
        ColorUtils::Rgba bottom( ColorUtils::backgroundBottomColor( base ) );

        // create context and translate
        Cairo::Context context( window, clipRect );
        cairo_translate( context, x, y );

        // paint translucent first
        const bool hasAlpha( (options&Alpha) );
        bool round( GDK_IS_WINDOW( window ) && (options&Round) );

        if( hasAlpha )
        {
            if( _settings.tooltipTransparent() )
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

            gdk_cairo_rounded_rectangle( context, &rect, 3.5, round ? CornersAll:CornersNone );
            cairo_set_source( context, pattern );
            cairo_fill( context );

        }

        // contrast pixel
        {
            Cairo::Pattern pattern( cairo_pattern_create_linear( 0, 0, 0, h ) );
            cairo_pattern_add_color_stop( pattern, 0.5, ColorUtils::lightColor( bottom ) );
            cairo_pattern_add_color_stop( pattern, 0.9, bottom );

            cairo_rounded_rectangle( context, 0.5, 0.5, w-1, h-1, 3.5, round ? CornersAll:CornersNone );
            cairo_set_line_width( context, 1.0 );
            cairo_set_source( context, pattern );
            cairo_stroke( context );
        }

        return;

    }

    //__________________________________________________________________
    void Style::renderHeaderBackground( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h )
    {

        // load color
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );

        // render normal window background
        renderWindowBackground( window, clipRect, x, y, w, h );

        // render lines
        renderHeaderLines( window, clipRect, x, y, w, h );

        // render side dots
        Cairo::Context context( window, clipRect );
        int yCenter( y + h/2 );
        int xDots( x + w - 1 );
        _helper.renderDot( context, base, xDots, yCenter - 3 );
        _helper.renderDot( context, base, xDots, yCenter );
        _helper.renderDot( context, base, xDots, yCenter + 3 );

    }


    //__________________________________________________________________
    void Style::renderHeaderLines( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h ) const
    {

        // add horizontal lines
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
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
    void Style::renderTreeLines( GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const Gtk::CellInfoFlags& cellFlags, const StyleOptions& options ) const
    {

        // define pen color
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base( ColorUtils::mix(
            _settings.palette().color( group, Palette::Text ),
            _settings.palette().color( group, Palette::Window ),
            0.8 ) );

        Cairo::Context context( window, clipRect );
        cairo_set_source( context, base );
        cairo_set_line_width( context, 1.0 );

        const bool reversed( cellFlags._flags & Gtk::CellInfoFlags::Reversed );

        int cellIndent( cellFlags._levelIndent + cellFlags._expanderSize + 4 );
        int xStart( x + cellIndent/2 );

        if( reversed ) {

            xStart += w - cellIndent;
            cellIndent *= -1;

        }

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
                    cairo_line_to( context, xCenter + 0.5, yCenter - int(cellFlags._expanderSize/3 )-1 );

                    // second vertical line
                    if( !isLastCell )
                    {

                        cairo_move_to( context, xCenter + 0.5, y+h );
                        cairo_line_to( context, xCenter + 0.5, yCenter + int( cellFlags._expanderSize/3 )+2 );
                    }

                    // horizontal line
                    if( reversed )
                    {

                        cairo_move_to( context, xCenter - 1 - int( cellFlags._expanderSize/3 ), yCenter + 0.5 );
                        cairo_line_to( context, xCenter + 1  - cellFlags._expanderSize*2/3, yCenter + 0.5 );

                    } else {

                        cairo_move_to( context, xCenter + 2 + int( cellFlags._expanderSize/3 ), yCenter + 0.5 );
                        cairo_line_to( context, xCenter + cellFlags._expanderSize*2/3, yCenter + 0.5 );

                    }

                } else {

                    // vertical line
                    cairo_move_to( context, xCenter + 0.5, y );
                    if( isLastCell ) cairo_line_to( context, xCenter + 0.5, yCenter );
                    else cairo_line_to( context, xCenter + 0.5, y+h );

                    // horizontal line
                    if( reversed )
                    {

                        cairo_move_to( context, xCenter + 1 , yCenter + 0.5 );
                        cairo_line_to( context, xCenter + 1  - cellFlags._expanderSize*2/3, yCenter + 0.5 );

                    } else {

                        cairo_move_to( context, xCenter, yCenter + 0.5 );
                        cairo_line_to( context, xCenter + cellFlags._expanderSize*2/3, yCenter + 0.5 );

                    }

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
    void Style::renderHoleBackground(
        GdkWindow* window,
        GtkWidget* widget,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const StyleOptions& options, TileSet::Tiles tiles,
        gint sideMargin )
    {

        // do nothing if not enough room
        if( w < 14 || h < 14 )  return;

        // add hole mask
        Cairo::Context context( window, clipRect );
        renderHoleMask( context, x, y, w, h, tiles, sideMargin );

        if( (options&Flat) || _settings.applicationName().useFlatBackground( widget ) )
        {

            // create a rounded-rect antimask for renderHoleBackground
            cairo_set_source( context, _settings.palette().color( Palette::Window ) );
            cairo_rectangle( context, x, y, w, h );
            cairo_fill( context );

        } else if( GtkWidget* parent = _animations.flatWidgetEngine().flatParent( widget ) ) {

            // get background color and fill
            if( Gtk::gtk_widget_style_is_modified( parent, GTK_STATE_NORMAL, GTK_RC_BG ) )
            {

                cairo_set_source( context, Gtk::gdk_get_color( gtk_widget_get_modifier_style( parent )->bg[GTK_STATE_NORMAL] ) );

            } else {

                cairo_set_source( context, _settings.palette().color( Palette::Window ) );

            }

            cairo_rectangle( context, x, y, w, h );
            cairo_fill( context );

        } else {

            // normal window background
            renderWindowBackground( context, window, 0L, clipRect, x, y, w, h, options, tiles);

            // possible groupbox background
            // Pass NoFill option in order not to render the surrounding frame
            if( widget )
            { renderGroupBoxBackground( context, window, widget, clipRect, x, y, w, h, options | Blend | NoFill, tiles ); }

        }

    }

    //__________________________________________________________________
    void Style::renderSplitter(
        GdkWindow* window, GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        const AnimationData& data ) const
    {

        // orientation
        const bool vertical( options&Vertical );

        // colors
        const ColorUtils::Rgba& base( _settings.palette().color( Palette::Window ) );

        // context
        Cairo::Context context( window, clipRect );

        // hover color
        ColorUtils::Rgba highlight;
        if( data._mode == AnimationHover )
        {

            highlight = ColorUtils::alphaColor( ColorUtils::lightColor( base ), 0.5*data._opacity );

        } else if( options&Hover ) {

            highlight = ColorUtils::alphaColor( ColorUtils::lightColor( base ), 0.5 );

        }

        // render hover rect
        if( highlight.isValid() )
        {

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
                _helper.renderDot( context, base, center-3, y );
                _helper.renderDot( context, base, center, y );
                _helper.renderDot( context, base, center+3, y );
            }

        } else {

            x += w/2;
            const int ngroups( std::max( 1, h/250 ) );
            int center = ( h-( ngroups-1 )*250 )/2 + y;
            for( int k = 0; k < ngroups; k++, center += 250 )
            {
                _helper.renderDot( context, base, x, center-3 );
                _helper.renderDot( context, base, x, center );
                _helper.renderDot( context, base, x, center+3 );
            }

        }

    }

    //____________________________________________________________________________________
    void Style::renderProgressBarHole(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const StyleOptions& options )
    {

        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base(_settings.palette().color( group, Palette::Window ) );

        Cairo::Context context( window, clipRect );
        renderScrollBarHole( context, x, y, w, h, base, (options&Vertical) );

    }

    //____________________________________________________________________________________
    void Style::renderProgressBarHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const StyleOptions& options )
    {

        // colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Active, Palette::Window ) );
        const ColorUtils::Rgba glow( _settings.palette().color( group, Palette::Selected ) );

        // context
        Cairo::Context context( window, clipRect );

        // validate rect
        if(w<0 || h<0) return;

        // make sure that width is large enough
        const int indicatorSize( (options&Vertical ? h:w ) );

        if( indicatorSize >= 3 && w > 0 && h > 1 )
        {
            // get surface
            const Cairo::Surface& surface( _helper.progressBarIndicator( base, glow, w, h-1 ) );
            cairo_translate( context, x, y );
            cairo_rectangle( context, 0, 0, w, h-1 );
            cairo_set_source_surface( context, surface, 0, 0 );
            cairo_fill( context );
        }

    }

    //____________________________________________________________________________________
    void Style::renderScrollBarHole(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const StyleOptions& options )
    {

        // colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba base(_settings.palette().color( group, Palette::Window ) );

        // context
        Cairo::Context context( window, clipRect );
        renderScrollBarHole( context, x, y, w, h, base, options&Vertical );

    }

    //____________________________________________________________________________________
    void Style::renderHoleMask( cairo_t* context, gint x, gint y, gint w, gint h, TileSet::Tiles tiles, gint sideMargin )
    {

        GdkRectangle mask = {x+2, y+1, w-4, h-3 };
        const double maskRadius = 3.5;
        Corners corners( CornersNone );
        if( tiles & TileSet::Left )
        {
            mask.x += sideMargin;
            mask.width -= sideMargin;
            if( tiles & TileSet::Top ) corners |= CornersTopLeft;
            if( tiles & TileSet::Bottom ) corners |= CornersBottomLeft;
        }

        if( tiles & TileSet::Right )
        {
            mask.width -= sideMargin;
            if( tiles & TileSet::Top ) corners |= CornersTopRight;
            if( tiles & TileSet::Bottom ) corners |= CornersBottomRight;
        }

        // set clipping mask
        gdk_cairo_rounded_rectangle_negative(context,&mask,maskRadius,CornersAll);
        cairo_rectangle(context,x,y,w,h);
        cairo_clip(context);

        return;

    }

    //____________________________________________________________________________________
    void Style::renderScrollBarHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        const AnimationData& data )
    {

        // vertical
        const bool vertical( options&Vertical );

        const double xf( vertical ? x+3 : x+4 );
        const double yf( vertical ? y+3 : y+2 );
        const double wf( vertical ? w-6 : w-8 );
        const double hf( vertical ? h-6 : h-5 );

//         const double xf( vertical ? x+2 : x+3 );
//         const double yf( vertical ? y+2 : y+1 );
//         const double wf( vertical ? w-4 : w-6 );
//         const double hf( vertical ? h-4 : h-3 );

        if( wf <= 0 || hf <= 0 ) return;

        // context
        Cairo::Context context( window, clipRect );

        // store colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        const ColorUtils::Rgba color( _settings.palette().color( group, Palette::Button ) );

        const double radius( 3.5 );

        // glow color
        ColorUtils::Rgba glow;
        const ColorUtils::Rgba shadow( ColorUtils::alphaColor( ColorUtils::shadowColor( color ), 0.4 ) );
        const ColorUtils::Rgba hovered( _settings.palette().color( Palette::Hover ) );
        if( data._mode == AnimationHover ) glow = ColorUtils::mix( shadow, hovered, data._opacity );
        else if( options&Hover ) glow = hovered;
        else glow = shadow;

        _helper.scrollHandle( color, glow ).render( context, xf-3, yf-3, wf+6, hf+6, TileSet::Full );

        // contents
        const ColorUtils::Rgba mid( ColorUtils::midColor( color ) );
        Cairo::Pattern pattern( cairo_pattern_create_linear( 0, yf, 0, yf+hf ) );
        cairo_pattern_add_color_stop( pattern, 0, color );
        cairo_pattern_add_color_stop( pattern, 1, mid );
        cairo_set_source( context, pattern );
        cairo_rounded_rectangle( context, xf+1, yf+1, wf-2, hf-2, radius - 2 );
        cairo_fill( context );

        // bevel pattern
        {
            const ColorUtils::Rgba light( ColorUtils::lightColor( color ) );
            Cairo::Pattern pattern;
            if( vertical ) pattern.set( cairo_pattern_create_linear( 0, 0, 0, 30 ) );
            else pattern.set( cairo_pattern_create_linear( 0, 0, 30, 0 ) );
            cairo_pattern_set_extend( pattern, CAIRO_EXTEND_REFLECT );

            cairo_pattern_add_color_stop( pattern, 0, ColorUtils::Rgba::transparent() );
            cairo_pattern_add_color_stop( pattern, 1.0, ColorUtils::alphaColor( light, 0.1 ) );

            cairo_set_source( context, pattern );
            if( vertical ) cairo_rectangle( context, xf+3, yf, wf-6, hf );
            else cairo_rectangle( context, xf, yf+3, wf, hf-6 );
            cairo_fill( context );
        }

    }

    //____________________________________________________________________________________
    void Style::renderToolBarHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const StyleOptions& options ) const
    {

        const bool vertical( options & Vertical );
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );

        Cairo::Context context( window, clipRect );
        int counter(0);
        if( vertical )
        {

            const int xcenter( x+w/2 );
            for( int ycenter = y+2; ycenter <= y+h-3; ycenter+=3, ++counter )
            {
                if( counter%2 == 0 ) _helper.renderDot( context, base, xcenter+1, ycenter );
                else _helper.renderDot( context, base, xcenter-2, ycenter );
            }

        } else {

            const int ycenter( y + h/2 );
            for( int xcenter = x+2; xcenter < x+w-3; xcenter+=3, ++counter )
            {
                if( counter%2 == 0 ) _helper.renderDot( context, base, xcenter, ycenter+1 );
                else _helper.renderDot( context, base, xcenter, ycenter-2 );
            }

        }

        return;

    }

    //__________________________________________________________________
    void Style::drawFloatFrame( cairo_t* context, GdkWindow* window, GdkRectangle* clipRect, gint x, gint y, gint w, gint h, const StyleOptions& options, Palette::Role role ) const
    {

        // define colors
        ColorUtils::Rgba base(_settings.palette().color( role ) );
        ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        ColorUtils::Rgba dark( ColorUtils::darkColor( ColorUtils::backgroundBottomColor( base ) ) );

        const bool hasAlpha( options&Alpha );
        const bool isMenu( options&Menu );
        const bool drawUglyShadow( !hasAlpha );
        const bool rounded( options&Round );

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

            if( options&Focus )
            {

                // window is active - it's a glow, not a shadow
                const ColorUtils::Rgba frameColor( _settings.palette().color( Palette::ActiveWindowBackground ) );
                const ColorUtils::Rgba glow = ColorUtils::mix(ColorUtils::Rgba(0.5,0.5,0.5),frameColor,0.7);
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

        } else {

            if( h > 20.5 ) cairo_pattern_add_color_stop( pattern, std::max( 0.0, 1.0 - 12.0/( double(h)-5.5 ) ), ColorUtils::alphaColor( light, 0.5 ) );
            else if( h > 8.5 ) cairo_pattern_add_color_stop( pattern, std::max( 0.0, 3.0/( double(h)-5.5 ) ), ColorUtils::alphaColor( light, 0.5 ) );
            cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( light ) );

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
        gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        const AnimationData& animationData,
        TileSet::Tiles tiles
        )
    {

        // flat buttons are only rendered with a simple Rect, and only when either focused or sunken
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );

        // glow color (depending on hover/glow
        const ColorUtils::Rgba glow( slabShadowColor( options, animationData ) );

        if( options & Flat )
        {
            if( options & Sunken )
            {

                Cairo::Context context( window, clipRect );
                const ColorUtils::Rgba base( color( group, Palette::Window, options ) );

                const double bias( 0.75 );
                double opacity( 1.0 );
                if( glow.isValid() ) opacity -= bias*glow.alpha();

                // fill hole
                ColorUtils::Rgba color( ColorUtils::midColor( base ) );
                color = ColorUtils::alphaColor( color, opacity );
                cairo_save( context );
                cairo_set_source( context, color );
                cairo_rounded_rectangle( context, x+1, y+1, w-2, h-2, 3.5 );
                cairo_fill( context );
                cairo_restore( context );

                if( glow.isValid() ) _helper.holeFocused( base, glow, 7, true ).render( context, x, y, w, h );
                else _helper.hole( base, 7, true ).render( context, x, y, w, h );

            } else if( glow.isValid() ) {

                Cairo::Context context( window, clipRect );
                _helper.slitFocused( glow ).render( context, x, y, w, h, tiles );

            }

            return;

        }

        // forces minimum size to 14x14
        if( w < 14 || h < 14 )
        {
            GdkRectangle parent( Gtk::gdk_rectangle( x, y, w, h ) );
            GdkRectangle child( Gtk::gdk_rectangle( x, y, std::max( w, 14), std::max( h, 14 ) ) );
            centerRect( &parent, &child );
            x = child.x;
            y = child.y;
            w = child.width;
            h = child.height;
            clipRect = 0L;
        }


        // define colors
        ColorUtils::Rgba base( color( group, Palette::Button, options ) );
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( base, wh, y+wy+h/2 );

        }

        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );
        cairo_save( context );

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
        _helper.fillSlab( context, x, y, w, h+1, tiles );
        cairo_restore( context );

        if( options&Sunken )
        {

            _helper.slabSunken( base ).render( context, x, y, w, h, tiles );

        } else {

            _helper.slab( base, glow, 0 ).render( context, x, y, w, h, tiles );

        }

    }

    //__________________________________________________________________
    void Style::renderSlab(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap,
        const StyleOptions& options,
        const AnimationData& animationData )
    {

        // define colors
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wh, y+wy+h/2 );

        } else {

            base = _settings.palette().color( Palette::Window );

        }

        // create context
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );
        renderSlab( context, x, y, w, h, base, options, animationData, TileSet::Ring );

    }

    //__________________________________________________________________
    void Style::renderInfoBar(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        const ColorUtils::Rgba& glow )
    {

        // create context
        Cairo::Context context( window, clipRect );

        // content
        cairo_rounded_rectangle( context, x+1, y+1, w-2, h-2, 5 );
        cairo_set_source( context, glow );
        cairo_fill( context );

        // border
        cairo_set_line_width( context, 1.0 );
        cairo_rounded_rectangle( context, 1.5+x, 1.5+y, w-3, h-3, 4.5 );
        cairo_set_source( context, ColorUtils::darken( glow ) );
        cairo_stroke( context );

    }

    //__________________________________________________________________
    void Style::renderCheckBox(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, GtkShadowType shadow,
        const StyleOptions& options,
        const AnimationData& animationData )
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
            base = ColorUtils::backgroundColor( _settings.palette().color( group, role ), wh, y+wy+h/2 );

        } else {

            base = _settings.palette().color( group, role );

        }

        // draw slab
        Cairo::Context context( window, clipRect );

        if( options & Flat )
        {

            _helper.holeFlat( base, 0, false ).render( context, child.x+1, child.y-1, child.width, child.height, TileSet::Full );
            cairo_translate( context, 0, -2 );

        } else {

            StyleOptions localOptions( options );
            localOptions &= ~Sunken;
            renderSlab( context, child.x, child.y, child.width, child.height, base, localOptions, animationData );

        }

        // draw mark
        x = int( double(child.x + child.width/2) - 3.5 );
        y = int( double(child.y + child.height/2) - 2.5 );

        if( shadow == GTK_SHADOW_IN || shadow == GTK_SHADOW_ETCHED_IN || options&Active )
        {

            cairo_set_line_cap( context, CAIRO_LINE_CAP_ROUND );
            cairo_set_line_join( context, CAIRO_LINE_JOIN_ROUND );
            if( shadow == GTK_SHADOW_IN ) cairo_set_line_width( context, 2.0 );

            Palette::Group group( (options&Disabled) ? Palette::Disabled : Palette::Active );
            const ColorUtils::Rgba& color( _settings.palette().color( group, ( options&Flat ) ? Palette::WindowText : Palette::ButtonText ) );
            const ColorUtils::Rgba& background( _settings.palette().color( ( options&Flat ) ? Palette::Window : Palette::Button ) );

            ColorUtils::Rgba base( ColorUtils::decoColor( background, color ) );
            ColorUtils::Rgba contrast( ColorUtils::lightColor( background ) );

            // We don't want to change color on active state for menu checkboxes (it's never passed by GTK)
            // Also, if we ignore active state, we get correct render for LibreOffice
            if( options&Active && !(options&Flat) )
            {
                base = ColorUtils::alphaColor( base, 0.3 );
                contrast = ColorUtils::alphaColor( contrast, 0.3 );
            }

            cairo_translate( context, 0.5, 0.5 );

            const double offset( 1.0 );
            if( _settings.checkBoxStyle() == QtSettings::CS_CHECK )
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
        gint x, gint y, gint w, gint h, GtkShadowType shadow,
        const StyleOptions& options,
        const AnimationData& animationData )
    {

        // define checkbox rect
        gint cbw( CheckBox_Size );
        gint tileSize( CheckBox_Size/3 );
        double scale( 1.0 );

        if( _settings.applicationName().isOpenOffice() )
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

            if( options & Menu ) base = ColorUtils::menuBackgroundColor( _settings.palette().color( group, Palette::Button ), wh, y+wy+h/2 );
            else base = ColorUtils::backgroundColor( _settings.palette().color( group, Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = _settings.palette().color( group, Palette::Button );

        }

        // glow
        const ColorUtils::Rgba glow( slabShadowColor( options, animationData ) );

        // get the pixmap
        const Cairo::Surface& surface( _helper.roundSlab( base, glow, 0, tileSize ) );

        // create context
        Cairo::Context context( window, clipRect );
        cairo_save( context );
        cairo_translate( context, x, y );
        if(options&NoFill)
        {
            // Only render the glow, leave the bullet clipped out
            const int border(4);
            cairo_ellipse_negative(context,border,border,child.width-border*2,child.height-border*2);
            cairo_rectangle(context,0,0,child.width,child.height);
            cairo_clip(context);
        }
        cairo_rectangle( context, 0, 0, child.width, child.height );
        cairo_set_source_surface( context, surface, 0, 0 );
        cairo_fill( context );
        cairo_restore( context );

        if( shadow == GTK_SHADOW_IN || shadow == GTK_SHADOW_ETCHED_IN || options&Active )
        {
            double radius( shadow == GTK_SHADOW_ETCHED_IN ? 1.4:2.6 );
            radius *= scale;

            const double dx( 0.5*child.width - radius );
            const double dy( 0.5*child.height - radius );

            const ColorUtils::Rgba& background( _settings.palette().color( Palette::Button ) );
            const ColorUtils::Rgba& color( _settings.palette().color( group, Palette::ButtonText ) );

            ColorUtils::Rgba base( ColorUtils::decoColor( background, color ) );
            ColorUtils::Rgba contrast( ColorUtils::lightColor( background ) );

            // We don't want to change color on active state for menu radiobuttons (it's never passed by GTK)
            // Also, if we ignore active state, we get correct render for LibreOffice
            if( options&Active && !(options&Menu ) )
            {
                base = ColorUtils::alphaColor( base, 0.3 );
                contrast = ColorUtils::alphaColor( contrast, 0.3 );
            }


            cairo_save( context );
            cairo_translate( context, 0, radius/2 );
            cairo_ellipse( context, x+dx, y+dy, child.width - 2*dx, child.height -2*dy );
            cairo_restore( context );
            if( shadow == GTK_SHADOW_ETCHED_IN )
            {

                cairo_set_line_width( context, 1.3 );
                cairo_set_source( context, contrast );
                cairo_stroke( context );

                cairo_set_source( context, base );
                cairo_ellipse( context, x+dx, y+dy, child.width - 2*dx, child.height -2*dy );
                cairo_stroke( context );

            } else {

                cairo_set_source( context, contrast );
                cairo_fill( context );

                cairo_set_source( context, base );
                cairo_ellipse( context, x+dx, y+dy, child.width - 2*dx, child.height -2*dy );
                cairo_fill( context );

            }

        }

        return;
    }

    //____________________________________________________________________________________
    void Style::renderHole(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap,
        const StyleOptions& options,
        const AnimationData& animationData,
        TileSet::Tiles tiles )
    {

        // do nothing if not enough room
        if( w < 14 || h < 14 )  return;

        // enable state
        const bool enabled( !(options&Disabled ) );

        // load color
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
        ColorUtils::Rgba fill;
        if( !( options&NoFill ) )
        {
            const Palette::Group group( enabled ? Palette::Active : Palette::Disabled );
            fill = _settings.palette().color( group, Palette::Base );
        }

        // create context, add mask, and render hole
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );

        if( fill.isValid() ) tiles |= TileSet::Center;

        const ColorUtils::Rgba glow( holeShadowColor( options, animationData ) );
        if( glow.isValid() ) _helper.holeFocused( base, fill, glow ).render( context, x, y, w, h, tiles );
        else _helper.hole( base, fill ).render( context, x, y, w, h, tiles );

    }

    //____________________________________________________________________________________
    void Style::renderDockFrame(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const Gtk::Gap& gap, const StyleOptions& options )
    {

        // do nothing if not enough room
        if( w < 9 || h < 9 )  return;

        // define colors
        ColorUtils::Rgba top;
        ColorUtils::Rgba bottom;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            top = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wh, y+wy );
            bottom = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wh, y+h+wy );

        } else {

            top = _settings.palette().color( Palette::Window );
            bottom = _settings.palette().color( Palette::Window );

        }

        // create context, add mask, and render
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );
        _helper.dockFrame( top, bottom ).render( context, x, y, w, h );
    }

    //____________________________________________________________________________________
    void Style::renderGroupBoxFrame(
        GdkWindow* window,
        GtkWidget* widget,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h, const StyleOptions& options )
    {

        // register
        if( widget )
        { _animations.groupBoxEngine().registerWidget( widget ); }

        // define colors
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wh, y+wy+h/2 );

        } else {

            base = _settings.palette().color( Palette::Window );

        }

        Cairo::Context context( window, clipRect );
        renderGroupBox( context, base, x, y, w, h, options );

    }

    //____________________________________________________________________________________
    void Style::renderMenuItemRect(
        GdkWindow* window,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        const AnimationData& data )
    {
        ColorUtils::Rgba base;

        gint wh, wy;
        Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
        const bool isInMenu( Gtk::gtk_parent_menu( widget ) );
        const bool isInMenuBar( Gtk::gtk_parent_menubar( widget ) );

        if( wh > 0 )
        {
            if( isInMenu ) base = ColorUtils::menuBackgroundColor( ColorUtils::midColor( _settings.palette().color( Palette::Window ) ), wh, y+wy+h/2 );
            else if( options&Blend ) base = ColorUtils::backgroundColor( ColorUtils::midColor( _settings.palette().color( Palette::Window ) ), wh, y+wy+h/2 );
            else base = ColorUtils::midColor( _settings.palette().color( Palette::Window ) );

        } else {

            base = ColorUtils::midColor( _settings.palette().color( Palette::Window ) );

        }

        // more color customization, based on menuHighlight mode
        ColorUtils::Rgba color( base );
        if( _settings.menuHighlightMode() == QtSettings::MM_STRONG )
        {

            if( (options & Sunken) || isInMenu ) color = _settings.palette().color( Palette::Selected );
            else color = ColorUtils::tint( color, _settings.palette().color( Palette::Hover ) );

        } else if( _settings.menuHighlightMode() == QtSettings::MM_SUBTLE ) {

            if( (options & Sunken) || isInMenu ) color = ColorUtils::mix( color, ColorUtils::tint( color, _settings.palette().color( Palette::Selected ), 0.6 ) );
            else color = ColorUtils::mix( color, ColorUtils::tint( color, _settings.palette().color( Palette::Hover ), 0.6 ) );

        }

        // apply animation data
        if( data._mode == AnimationHover )
        {
            if( data._opacity > 0 ) color = ColorUtils::alphaColor( color, data._opacity );
            else return;
        }


        if( isInMenuBar )
        {

            // we force ytickness in gtkrc to emulate Qt menubar/menu space separation
            // so adjust vertical extent of the rect in menubar
            if( _settings.applicationName().isXul() )
            {

                y+=3;
                h-=6;

            } else {

                x+=1;
                w-=2;

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
            Cairo::Context context( window, clipRect );
            cairo_translate( context, x, y );

            // draw item rect in a group
            cairo_push_group( context );
            _helper.holeFlat( color, 0 ).render( context, 0, 0, w, h, TileSet::Full );
            cairo_pop_group_to_source( context );

            // generate linear gradient for masking
            if( Gtk::gtk_widget_layout_is_reversed( widget ) )
            {

                Cairo::Pattern mask( cairo_pattern_create_linear( 4, 0, 40, 0 ) );
                cairo_pattern_add_color_stop( mask, 0,  ColorUtils::Rgba::transparent() );
                cairo_pattern_add_color_stop( mask, 1,  ColorUtils::Rgba::black() );
                cairo_mask( context, mask );

            } else {

                Cairo::Pattern mask( cairo_pattern_create_linear( w-40, 0, w-4, 0 ) );
                cairo_pattern_add_color_stop( mask, 0,  ColorUtils::Rgba::black() );
                cairo_pattern_add_color_stop( mask, 1,  ColorUtils::Rgba::transparent() );
                cairo_mask( context, mask );

            }

        } else {

            Cairo::Context context( window, clipRect );
            _helper.holeFlat( color, 0 ).render( context, x, y, w, h, TileSet::Full  );

        }
    }

    //____________________________________________________________________________________
    void Style::renderSelection(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        TileSet::Tiles tiles,
        const StyleOptions& options
        )
    {

        // do nothing if not selected nor hovered
        if( !options & (Hover|Selected ) ) return;

        Palette::Group group( (options & Focus) ? Palette::Active : Palette::Inactive );
        ColorUtils::Rgba base( _settings.palette().color( group, Palette::Selected ) );
        if( options & Hover  )
        {
            if( !( options & Selected ) ) base.setAlpha( 0.2 );
            else base = base.light( 110 );
        }

        // create context
        Cairo::Context context( window, clipRect );
        if( !(tiles&TileSet::Left) ) { x -= 8; w+=8; }
        if( !(tiles&TileSet::Right) ) { w += 8; }
        _helper.selection( base, h, false ).render( context, x, y, w, h, tiles );

    }

    //____________________________________________________________________________________
    void Style::renderArrow(
        GdkWindow* window,
        GdkRectangle* clipRect,
        GtkArrowType orientation,
        gint x, gint y, gint w, gint h,
        QtSettings::ArrowSize arrowSize,
        const StyleOptions& options,
        const AnimationData& data,
        Palette::Role role ) const
    {

        // get polygon
        const Polygon arrow( genericArrow( orientation, arrowSize ) );

        // retrieve colors
        ColorUtils::Rgba base;
        if( options&Disabled ) base = _settings.palette().color( Palette::Disabled, role );
        else if( data._mode == AnimationHover ) base = ColorUtils::mix(
            _settings.palette().color( Palette::Active, role ),
            _settings.palette().color( Palette::Hover ),
            data._opacity );
        else if( options&Hover ) base = _settings.palette().color( Palette::Hover );
        else base = _settings.palette().color( Palette::Active, role );

        // merge base color with relevant background, if needed
        const Palette::Group group( (options&Disabled) ? Palette::Disabled : Palette::Active );
        switch( role )
        {

            case Palette::WindowText:
            base = ColorUtils::decoColor( _settings.palette().color( group, Palette::Window ), base );
            break;

            case Palette::ButtonText:
            base = ColorUtils::decoColor( _settings.palette().color( group, Palette::Button ), base );
            break;

            default: break;

        }

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
            const ColorUtils::Rgba contrast( ColorUtils::lightColor( _settings.palette().color( Palette::Window ) ) );

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
        gint x, gint y, gint w, gint h, const StyleOptions& options )
    {

        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );

        const bool vertical( options & Vertical );
        GdkRectangle parent = { x, y, w, h };

        GdkRectangle child;
        if( vertical ) child = Gtk::gdk_rectangle( 0, 0, Slider_GrooveWidth, h );
        else child = Gtk::gdk_rectangle( 0, 0, w, Slider_GrooveWidth );
        centerRect( &parent, &child );

        if( !vertical )
        {
            // more adjustment needed due to contrast pixel
            child.y += 1;
            child.height -= 1;
        }

        Cairo::Context context( window, clipRect );
        _helper.scrollHole( base, vertical, true ).render( context, child.x, child.y, child.width, child.height, TileSet::Full );

    }

    //____________________________________________________________________________________
    void Style::renderSliderHandle(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        const StyleOptions& options,
        const AnimationData& animationData )
    {

        // define colors
        const Palette::Group group( options&Disabled ? Palette::Disabled : Palette::Active );
        ColorUtils::Rgba base;
        if( options&Blend )
        {

            gint wh, wy;
            Gtk::gdk_map_to_toplevel( window, 0L, &wy, 0L, &wh );
            base = ColorUtils::backgroundColor( _settings.palette().color( group, Palette::Button ), wh, y+wy+h/2 );

        } else {

            base = _settings.palette().color( group, Palette::Button );

        }

        // render slab
        Cairo::Context context( window, clipRect );

        GdkRectangle parent = { x, y, w, h };
        GdkRectangle child = {0, 0, 21, 21 };
        centerRect( &parent, &child );

        x = child.x;
        y = child.y;

        const ColorUtils::Rgba glow( slabShadowColor( options, animationData ) );
        const Cairo::Surface& surface( _helper.sliderSlab( base, glow, (options&Sunken), 0 ) );
        cairo_translate( context, x, y );
        cairo_rectangle( context, 0, 0, child.width, child.height );
        cairo_set_source_surface( context, surface, 0, 0 );
        cairo_fill( context );

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
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
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
        const StyleOptions& options,
        const TabOptions& tabOptions,
        const AnimationData& data
        )
    {

        if( tabOptions & CurrentTab )
        {

            return renderActiveTab( window, clipRect, x, y, w, h, side, options, tabOptions );

        } else {

            switch( _settings.tabStyle() )
            {
                case QtSettings::TS_SINGLE: return renderInactiveTab_Single( window, clipRect, x, y, w, h, side, options, tabOptions, data );
                case QtSettings::TS_PLAIN: return renderInactiveTab_Plain( window, clipRect, x, y, w, h, side, options, tabOptions, data );
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
        const StyleOptions& options,
        const TabOptions& tabOptions
        )
    {

        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );

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
        _helper.slab( base, 0 ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );
        return;

    }

    //__________________________________________________________________
    void Style::renderTabBarFrame(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        const Gtk::Gap& gap,
        const StyleOptions& options )
    {

        // define colors
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );

        // create context
        Cairo::Context context( window, clipRect );
        generateGapMask( context, x, y, w, h, gap );
        renderSlab( context, x, y, w, h, base, options );

    }

    //____________________________________________________________________________________
    void Style::renderTreeExpander(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkExpanderStyle style,
        const StyleOptions& options,
        const AnimationData& data,
        Palette::Role role
        ) const
    {

        // retrieve colors
        ColorUtils::Rgba base;
        if( options&Disabled ) base = _settings.palette().color( Palette::Disabled, role );
        else if( data._mode == AnimationHover ) base = ColorUtils::mix(
            _settings.palette().color( Palette::Active, role ),
            _settings.palette().color( Palette::Hover ),
            data._opacity );
        else if( options&Hover ) base = _settings.palette().color( Palette::Hover );
        else base = _settings.palette().color( Palette::Active, role );

        const int xcenter( x + w/2 );
        const int ycenter( y + h/2 );

        // expander 'radius' (copied from oxygen-qt)
        const int radius( ( 9 - 4 ) / 2 );

        // create context and translate to center
        Cairo::Context context( window, clipRect );
        cairo_set_line_width( context, 1.0 );
        cairo_set_source( context, base );

        cairo_translate( context, -0.5+xcenter, -0.5+ycenter );

        // horizontal line
        cairo_move_to( context, -radius, 0 );
        cairo_line_to( context, radius, 0 );

        // vertical line
        if( style == GTK_EXPANDER_COLLAPSED || style == GTK_EXPANDER_SEMI_COLLAPSED )
        {
            cairo_move_to( context, 0, -radius );
            cairo_line_to( context, 0, radius );
        }

        cairo_stroke( context );

    }

    //__________________________________________________________________
    void Style::renderWindowDecoration( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h, const gchar** windowStrings, gint titleIndentLeft, gint titleIndentRight, bool gradient )
    {
        bool hasAlpha( wopt & WinDeco::Alpha );
        bool drawResizeHandle( !(wopt & WinDeco::Shaded) && (wopt & WinDeco::Resizable) );
        bool isMaximized( wopt & WinDeco::Maximized );
        bool drawAlphaChannel( wopt & WinDeco::DrawAlphaChannel );
        StyleOptions options( hasAlpha ? Alpha : Blend );

        if( hasAlpha && !isMaximized )
        {
            // cut round corners using alpha
            cairo_rounded_rectangle(context,x,y,w,h,3.5);
            cairo_clip(context);
        }

        if(drawAlphaChannel)
            options|=DrawAlphaChannel;

        if( gradient ) renderWindowBackground( context, x, y, w, h, options, isMaximized );
        else
        {
            cairo_set_source( context, _settings.palette().color( Palette::Active, Palette::Window ) );
            cairo_paint( context );
        }

        options|=Round;

        // focus
        if(wopt & WinDeco::Active) options|=Focus;

        if( !isMaximized )
        { drawFloatFrame( context, 0L, 0L, x, y, w, h, options ); }

        if( drawResizeHandle )
        {
            ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
            renderWindowDots( context, x, y, w, h, base, wopt);
        }

        if(windowStrings)
        {
            // caption is drawn in drawWindowDecoration
            if( windowStrings[1] )
            {
                // TODO: use WMCLASS and caption to enable per-window style exceptions
            }
        }
    }

    //__________________________________________________________________
    void Style::drawWindowDecoration( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h, const gchar** windowStrings, gint titleIndentLeft, gint titleIndentRight )
    {

        #ifdef GDK_WINDOWING_X11
        /*
           (any element of windowStrings[] may be NULL - will be understood as "")
           windowStrings may also be NULL

           elements:
            windowStrings[0]: caption
            windowStrings[1]: WMCLASS
            windowStrings[2]: (gpointer)XID
        */
        /*
           caches layout:
               left&right border height: h
               top&bottom border width: w-BorderLeft-BorderRight
        */

        // enable gradient if XID is not passed
        bool gradient=true;

        const int buttonSpacing(WinDeco::getMetric(WinDeco::ButtonSpacing));
        titleIndentLeft+=2*buttonSpacing;
        titleIndentRight+=2*buttonSpacing;

        QtSettings::WindecoBlendType blendType(_settings.windecoBlendType());
        if( blendType==QtSettings::SolidColor )
        {

            gradient=false;

        } else if( blendType==QtSettings::FollowStyleHint && windowStrings && windowStrings[2] ) {

            Window window((Window)windowStrings[2]);
            Display* display( GDK_DISPLAY_XDISPLAY(gdk_display_get_default()) );
            if( _animations.backgroundHintEngine().backgroundGradientAtom() != None )
            {
                Atom typeRet;
                int formatRet;
                unsigned long itemsRet;
                unsigned long afterRet;
                unsigned char *data = 0;

                if( !( XGetWindowProperty(display, window, _animations.backgroundHintEngine().backgroundGradientAtom(), 0, G_MAXLONG, False,
                    XA_CARDINAL, &typeRet, &formatRet, &itemsRet, &afterRet, &data) == Success
                    && itemsRet == 1
                    && formatRet == 32) )
                {
                    // if the window doesn't have this property set, it's likely
                    // non-oxygenized, thus shouldn't have windeco bg gradient
                    gradient=false;
                }
            }
        }

        WindecoBorderKey key(wopt,w,h,gradient);

        {
            // draw left border with cache
            Cairo::Surface left( _helper.windecoLeftBorderCache().value(key) );
            int sw=WinDeco::getMetric(WinDeco::BorderLeft);
            if(sw)
            {

                if( !left )
                {

                    #if OXYGEN_DEBUG
                    std::cerr<<"drawWindowDecoration: drawing left border; width: " << w << "; height: " << h << "; wopt: " << wopt << std::endl;
                    #endif
                    left=_helper.createSurface(sw,h);

                    Cairo::Context context(left);
                    renderWindowDecoration( context, wopt, 0, 0, w, h, windowStrings, titleIndentLeft, titleIndentRight, gradient);

                    _helper.windecoLeftBorderCache().insert(key,left);

                } else {

                    #if OXYGEN_DEBUG
                    std::cerr << "drawWindowDecoration: using saved left border" << std::endl;
                    #endif

                }

                cairo_set_source_surface(context, left, x, y);
                cairo_rectangle(context,x,y,sw,h);
                cairo_fill(context);
            }
        }

        {
            // draw right border with cache
            Cairo::Surface right( _helper.windecoRightBorderCache().value(key) );
            int sw=WinDeco::getMetric(WinDeco::BorderRight);
            if(sw)
            {

                if( !right )
                {

                    #if OXYGEN_DEBUG
                    std::cerr<<"drawWindowDecoration: drawing right border; width: " << w << "; height: " << h << "; wopt: " << wopt << std::endl;
                    #endif

                    right=_helper.createSurface(sw,h);

                    Cairo::Context context(right);
                    renderWindowDecoration( context, wopt, -(w-sw), 0, w, h, windowStrings, titleIndentLeft, titleIndentRight, gradient );

                    _helper.windecoRightBorderCache().insert(key,right);

                } else {

                    #if OXYGEN_DEBUG
                    std::cerr << "drawWindowDecoration: using saved right border" << std::endl;
                    #endif

                }

                cairo_set_source_surface(context, right, x+w-sw, y);
                cairo_rectangle(context,x+w-sw,y,sw,h);
                cairo_fill(context);
            }
        }

        {
            // draw top border with cache
            Cairo::Surface top( _helper.windecoTopBorderCache().value(key) );
            int left=WinDeco::getMetric(WinDeco::BorderLeft);
            int right=WinDeco::getMetric(WinDeco::BorderRight);
            int sh=WinDeco::getMetric(WinDeco::BorderTop);
            int sw=w-left-right;
            if(sh && sw)
            {
                if( !top )
                {

                    #if OXYGEN_DEBUG
                    std::cerr<<"drawWindowDecoration: drawing top border; width: " << w << "; height: " << h << "; wopt: " << wopt << std::endl;
                    #endif
                    top=_helper.createSurface(sw,sh);

                    Cairo::Context context(top);
                    renderWindowDecoration( context, wopt, -left, 0, w, h, windowStrings, titleIndentLeft, titleIndentRight, gradient );

                    _helper.windecoTopBorderCache().insert(key,top);

                } else {

                    #if OXYGEN_DEBUG
                    std::cerr << "drawWindowDecoration: using saved top border" << std::endl;
                    #endif

                }

                cairo_set_source_surface(context, top, x+left, y);
                cairo_rectangle(context,x+left,y,sw,sh);
                cairo_fill(context);

                // caption shouldn't be saved in the cache
                if( windowStrings && windowStrings[0] )
                {
                    // draw caption
                    const gchar* &caption(windowStrings[0]);
                    const FontInfo& font( _settings.WMFont() );
                    gint layoutWidth=w-(titleIndentLeft+titleIndentRight);
                    if( font.isValid() && layoutWidth>0 )
                    {
                        PangoFontDescription* fdesc( pango_font_description_new() );
                        const Palette::Group group( wopt & WinDeco::Active ? Palette::Active : Palette::Disabled );
                        const int H=WinDeco::getMetric(WinDeco::BorderTop);
                        int textHeight;

                        pango_font_description_set_family( fdesc, font.family().c_str() );
                        pango_font_description_set_weight( fdesc, PangoWeight( (font.weight()+2)*10 ) );
                        pango_font_description_set_style( fdesc, font.italic() ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL );
                        pango_font_description_set_absolute_size( fdesc, int(font.size()*PANGO_SCALE) );

                        PangoLayout* layout( pango_cairo_create_layout(context) );
                        pango_layout_set_text( layout,caption, -1 );
                        pango_layout_set_font_description( layout, fdesc );
                        pango_layout_set_width( layout, layoutWidth*PANGO_SCALE );
                        pango_layout_set_ellipsize( layout, PANGO_ELLIPSIZE_END );
                        pango_layout_set_alignment( layout, _settings.TitleAlignment() );
                        pango_layout_get_pixel_size( layout, NULL, &textHeight );

                        cairo_save( context );

                        ColorUtils::Rgba titleContrastColor(ColorUtils::lightColor(_settings.palette().color( Palette::Disabled, Palette::Window )));
                        cairo_set_source( context, titleContrastColor );
                        cairo_translate( context, x+titleIndentLeft, y+(H-textHeight)/2.+1 );
                        pango_cairo_update_layout( context, layout );
                        pango_cairo_show_layout( context, layout );

                        ColorUtils::Rgba titleTextColor(_settings.palette().color( group, Palette::WindowText ));
                        cairo_set_source( context, titleTextColor );
                        cairo_translate( context, 0, -1 );
                        pango_cairo_update_layout( context, layout );
                        pango_cairo_show_layout( context, layout );

                        cairo_restore( context );

                        g_object_unref(layout);
                        pango_font_description_free(fdesc);
                    }
                }
            }
        }

        {
            // draw bottom border with cache
            Cairo::Surface bottom( _helper.windecoBottomBorderCache().value(key) );
            int left=WinDeco::getMetric(WinDeco::BorderLeft);
            int right=WinDeco::getMetric(WinDeco::BorderRight);
            int sh=WinDeco::getMetric(WinDeco::BorderBottom);
            int sw=w-left-right;
            int Y=y+h-sh;
            if(sh && sw)
            {
                if( !bottom)
                {

                    #if OXYGEN_DEBUG
                    std::cerr<<"drawWindowDecoration: drawing bottom border; width: " << w << "; height: " << h << "; wopt: " << wopt << std::endl;
                    #endif
                    bottom=_helper.createSurface(sw,sh);

                    Cairo::Context context(bottom);
                    renderWindowDecoration( context, wopt, -left, y-Y, w, h, windowStrings, titleIndentLeft, titleIndentRight, gradient );

                    _helper.windecoBottomBorderCache().insert(key,bottom);

                } else {

                    #if OXYGEN_DEBUG
                    std::cerr << "drawWindowDecoration: using saved bottom border" << std::endl;
                    #endif

                }

                cairo_set_source_surface(context, bottom, x+left, Y);
                cairo_rectangle(context,x+left,Y,sw,sh);
                cairo_fill(context);
            }
        }

        #endif

    }

    //__________________________________________________________________
    void Style::drawWindowShadow( cairo_t* context, WinDeco::Options wopt, gint x, gint y, gint w, gint h )
    {
        cairo_set_source_rgba( context, 0, 0, 0, 0 );
        cairo_set_operator( context, CAIRO_OPERATOR_SOURCE );
        cairo_paint( context );
        cairo_set_operator( context, CAIRO_OPERATOR_OVER );

        WindowShadow shadow(_settings, _helper);
        shadow.setWindowState(wopt);
        shadow.render(context, x,y,w,h);
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

        WinDeco::Button button( _settings, _helper, type );
        button.setState(buttonState);
        int buttonSize=_settings.buttonSize();
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
    void Style::adjustScrollBarHole( int& x, int& y, int& w, int& h, const StyleOptions& options ) const
    {

        const int buttonSize( 12 );
        const int subLineOffset( buttonSize*_settings.scrollBarSubLineButtons() );
        const int addLineOffset( buttonSize*_settings.scrollBarAddLineButtons() );
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
    void Style::setBackgroundSurface( const std::string& filename )
    {
        if( _backgroundSurface.isValid() ) _backgroundSurface.free();
        _backgroundSurface.set( cairo_image_surface_create_from_png( filename.c_str() ) );
    }

    //____________________________________________________________________________________
    void Style::renderActiveTab(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        const StyleOptions& options,
        const TabOptions& tabOptions
        )
    {

        const bool isFirstTabAligned( tabOptions & FirstTabAligned );
        const bool isLastTabAligned( tabOptions & LastTabAligned );

        // get color
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );

        // borders and connections to tabs
        // this is quite painfull and slipery code.
        // the same is true with oxygen-qt
        int offset = 2;
        int adjust = ( tabOptions&Xul ) ? 0:2;
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
        _helper.slab( base, 0 ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );

        // adjust rect for filling
        SlabRect fillSlab( tabSlab );
        fillSlab._x += 4;
        fillSlab._y += 4;
        fillSlab._w -= 8;
        fillSlab._h -= 8;

        // fill
        Cairo::Pattern pattern;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            fillSlab._h -= 2;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y, 0, fillSlab._y + fillSlab._h ) );
            break;

            case GTK_POS_TOP:
            fillSlab._y += 2;
            fillSlab._h -= 2;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y + fillSlab._h, 0, fillSlab._y ) );
            break;

            case GTK_POS_RIGHT:
            fillSlab._w -= 2;
            pattern.set( cairo_pattern_create_linear( fillSlab._x, 0, fillSlab._x + fillSlab._w, 0 ) );
            break;

            case GTK_POS_LEFT:
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
        if( tabOptions&Xul )
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
        { _helper.slab(base, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles ); }

    }

    //____________________________________________________________________________________
    void Style::renderInactiveTab_Single(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        const StyleOptions& options,
        const TabOptions& tabOptions,
        const AnimationData& data
        )
    {

        // convenience flags
        const bool isFirstTabAligned( tabOptions & FirstTabAligned );
        const bool isLastTabAligned( tabOptions & LastTabAligned );

        // get color
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        const ColorUtils::Rgba dark( ColorUtils::darkColor( base ) );

        // create context
        Cairo::Context context( window, clipRect );

        // borders and connections to tabs
        // this is quite painfull and slipery code.
        // the same is true with oxygen-qt
        int offset = 2;
        int adjust = (tabOptions&Xul) ? 0:2;
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
        ColorUtils::Rgba glow;
        if( data._mode == AnimationHover ) glow = ColorUtils::alphaColor( _settings.palette().color( Palette::Hover ), data._opacity );
        else if( options&Hover ) glow = _settings.palette().color( Palette::Hover );

        _helper.slab( base, glow ).render( context, tabSlab._x, tabSlab._y, tabSlab._w, tabSlab._h, tabSlab._tiles );

        // adjust rect for filling
        SlabRect fillSlab( tabSlab );
        fillSlab._x += 4;
        fillSlab._y += 4;
        fillSlab._w -= 8;
        fillSlab._h -= 8;

        // fill
        Cairo::Pattern pattern;
        switch( side )
        {
            case GTK_POS_BOTTOM:
            fillSlab._h -= 3;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y, 0, fillSlab._y + fillSlab._h ) );
            break;

            case GTK_POS_TOP:
            fillSlab._y += 3;
            fillSlab._h -= 3;
            pattern.set( cairo_pattern_create_linear( 0, fillSlab._y + fillSlab._h, 0, fillSlab._y ) );
            break;

            case GTK_POS_RIGHT:
            fillSlab._w -= 3;
            pattern.set( cairo_pattern_create_linear( fillSlab._x, 0, fillSlab._x + fillSlab._w, 0 ) );
            break;

            case GTK_POS_LEFT:
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

            if( (iter->_options&Hover) && glow.isValid() )
            {

                _helper.slab(base, glow).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles );

            } else {

                _helper.slab(base).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles );

            }
        }
    }

    //____________________________________________________________________________________
    void Style::renderInactiveTab_Plain(
        GdkWindow* window,
        GdkRectangle* clipRect,
        gint x, gint y, gint w, gint h,
        GtkPositionType side,
        const StyleOptions& options,
        const TabOptions& tabOptions,
        const AnimationData& data
        )
    {
        // convenience flags
        const bool isFirstTabAligned( tabOptions & FirstTabAligned );
        const bool isLastTabAligned( tabOptions & LastTabAligned );

        const bool isLeftOfSelected( tabOptions & LeftOfSelected );
        const bool isRightOfSelected( tabOptions & RightOfSelected );

        // get color
        const ColorUtils::Rgba base( _settings.palette().color( Palette::Window ) );

        // create context
        Cairo::Context context( window, clipRect );

        // borders and connections to tabs
        // this is quite painfull and slipery code.
        // the same is true with oxygen-qt
        int offset = 2;
        int adjust = (tabOptions&Xul) ? 0:2;
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
            {  backgroundColor = ColorUtils::backgroundColor( _settings.palette().color( Palette::Window ), wh, y+wy+h/2 ); }

        }

        const ColorUtils::Rgba midColor( ColorUtils::alphaColor( ColorUtils::darkColor( backgroundColor ), 0.4 ) );
        const ColorUtils::Rgba darkColor( ColorUtils::alphaColor( ColorUtils::darkColor( backgroundColor ), 0.8 ) );

        cairo_set_line_width( context, 1.0 );
        cairo_set_source( context, midColor );
        cairo_fill_preserve( context );

        cairo_set_source( context, darkColor );
        cairo_stroke( context );

        ColorUtils::Rgba glow;
        if( data._mode == AnimationHover ) glow = ColorUtils::alphaColor( _settings.palette().color( Palette::Hover ), data._opacity );
        else if( options&Hover ) glow = _settings.palette().color( Palette::Hover );

        for( SlabRect::List::const_iterator iter = slabs.begin(); iter != slabs.end(); ++iter )
        { _helper.slab(base, glow, 0).render( context, iter->_x, iter->_y, iter->_w, iter->_h, iter->_tiles ); }

    }

    //____________________________________________________________________________________
    ColorUtils::Rgba Style::slabShadowColor( const StyleOptions& options, const AnimationData& data ) const
    {

        // no glow when widget is disabled
        if( options&Disabled ) return ColorUtils::Rgba();

        if( (options&Flat) && !(options&Sunken) )
        {

            /*
            flat buttons have special handling of colors: hover and focus are both assigned
            the hover color. This is consistent with oxygen-qt, though quite inconsistent with
            other widgets.
            */
            if(
                ( data._mode == AnimationHover && (options&Focus) ) ||
                ( data._mode == AnimationFocus && (options&Hover) ) )
            {
                return _settings.palette().color( Palette::Focus );

            } else if( data._mode & (AnimationHover|AnimationFocus) ) {

                return ColorUtils::alphaColor( _settings.palette().color( Palette::Focus ), data._opacity );

            } else if( options&(Focus|Hover) ) {

                return _settings.palette().color( Palette::Focus );

            } else return ColorUtils::Rgba();

        } else if( data._mode == AnimationHover ) {

            if( options & Focus )
            {
                return ColorUtils::mix(
                    _settings.palette().color( Palette::Focus ),
                    _settings.palette().color( Palette::Hover ), data._opacity );

            } else {

                return ColorUtils::alphaColor( _settings.palette().color( Palette::Hover ), data._opacity );

            }

        } else if( options&Hover ) {

            return _settings.palette().color( Palette::Hover );

        } else if( data._mode == AnimationFocus ) {

            return ColorUtils::alphaColor( _settings.palette().color( Palette::Focus ), data._opacity );

        } else if( options&Focus ) {

            return _settings.palette().color( Palette::Focus );

        } else return ColorUtils::Rgba();

    }

    //____________________________________________________________________________________
    ColorUtils::Rgba Style::holeShadowColor( const StyleOptions& options, const AnimationData& data ) const
    {

        // no glow when widget is disabled
        if( options&Disabled ) return ColorUtils::Rgba();

        if( data._mode == AnimationFocus && data._opacity >= 0 )
        {

            if( options & Hover )
            {

                return ColorUtils::mix(
                    _settings.palette().color( Palette::Hover ),
                    _settings.palette().color( Palette::Focus ), data._opacity );

            } else return ColorUtils::alphaColor( _settings.palette().color( Palette::Focus ), data._opacity );

        } else if( options & Focus ) {

            return _settings.palette().color( Palette::Focus );

        } else if( data._mode == AnimationHover && data._opacity >= 0 ) {

            return ColorUtils::alphaColor( _settings.palette().color( Palette::Hover ), data._opacity );

        } else if( options & Hover ) {

            return _settings.palette().color( Palette::Hover );

        } else return ColorUtils::Rgba();

    }

    //__________________________________________________________________
    void Style::renderGroupBox(
        cairo_t* context,
        const ColorUtils::Rgba& base,
        gint x, gint y, gint w, gint h,
        const StyleOptions& options )
    {

        cairo_push_group( context );
        Cairo::Pattern pattern( cairo_pattern_create_linear( 0, y - h + 12, 0,  y + 2*h - 19 ) );
        const ColorUtils::Rgba light( ColorUtils::lightColor( base ) );
        cairo_pattern_add_color_stop( pattern, 0, ColorUtils::alphaColor( light, 0.4 ) );
        cairo_pattern_add_color_stop( pattern, 1, ColorUtils::Rgba::transparent( light ) );
        cairo_set_source( context, pattern );

        if( !_settings.applicationName().useFlatBackground( 0L ) )
            _helper.fillSlab( context, x, y, w, h );

        if( !(options&NoFill) )
        { _helper.slope( base, 0.0 ).render( context, x, y, w, h ); }

        cairo_pop_group_to_source( context );

        Cairo::Pattern mask( cairo_pattern_create_linear( 0,  y + h - 19, 0,  y + h ) );
        cairo_pattern_add_color_stop( mask, 0, ColorUtils::Rgba::black() );
        cairo_pattern_add_color_stop( mask, 1.0, ColorUtils::Rgba::transparent() );
        cairo_mask( context, mask );

        return;

    }

    //__________________________________________________________________
    void Style::renderSlab(
        Cairo::Context& context,
        gint x, gint y, gint w, gint h,
        const ColorUtils::Rgba& base,
        const StyleOptions& options,
        const AnimationData& animationData,
        TileSet::Tiles tiles )
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
            _helper.fillSlab( context, x, y, w, h, tiles );

        }

        if( !(options&Sunken) ) {

            // calculate glow color
            const TileSet* tile;
            const ColorUtils::Rgba glow( slabShadowColor( options, animationData ) );
            if( glow.isValid() || base.isValid() ) tile = &_helper.slab( base, glow , 0);
            else return;

            if( tile ) tile->render( context, x, y, w, h );

        } else if( base.isValid() ) {

            _helper.slabSunken( base ).render( context, x, y, w, h );

        }

    }

    //__________________________________________________________________
    void Style::renderScrollBarHole( Cairo::Context& context, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& base, bool vertical, TileSet::Tiles tiles )
    {

        // use tileset from helper
        _helper.scrollHole( base, vertical ).render( context, x, y, w, h, tiles );

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
    void Style::renderWindowDots(cairo_t* context, gint x, gint y, gint w, gint h, const ColorUtils::Rgba& color, WinDeco::Options wopt)
    {
        bool isMaximized( wopt & WinDeco::Maximized );
        bool hasAlpha( wopt & WinDeco::Alpha );
        int offset( hasAlpha ? 0 : -1 );
        if( _settings.frameBorder() >= QtSettings::BorderTiny )
        {
            if( !isMaximized )
            {
                // Draw right side 3-dots resize handles
                int cenY = int(h/2+y);
                int posX = int(w+x-3) + 1;
                _helper.renderDot(context,color,posX+offset, cenY-3);
                _helper.renderDot(context,color,posX+offset, cenY);
                _helper.renderDot(context,color,posX+offset, cenY+3);
            }

            // Draw bottom-right corner 3-dots resize handles
            // if( !config.drawSizeGrip )
            {
                cairo_save(context);
                cairo_translate(context,x+w-8,y+h-8);
                _helper.renderDot(context,color,2+offset,6+offset);
                _helper.renderDot(context,color,5+offset,5+offset);
                _helper.renderDot(context,color,6+offset,2+offset);
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
        GdkRectangle mask( Gtk::gdk_rectangle() );

        switch( gap.position() )
        {
            case GTK_POS_TOP:
            {
                mask = Gtk::gdk_rectangle( x+gap.x(), y, gap.width(), gap.height() );
                break;
            }

            case GTK_POS_BOTTOM:
            {
                mask = Gtk::gdk_rectangle( x+gap.x(), y+h-gap.height(), gap.width(), gap.height() );
                break;
            }

            case GTK_POS_LEFT:
            {
                mask = Gtk::gdk_rectangle( x, y+gap.x(), gap.height(), gap.width() );
                break;
            }

            case GTK_POS_RIGHT:
            {
                mask = Gtk::gdk_rectangle( x + w - gap.height(), y+gap.x(), gap.height(), gap.width() );
                break;
            }

            default: return;
        }

        if( false )
        {
            cairo_set_source( context, ColorUtils::Rgba( 1, 0, 0, 0.3 ) );
            gdk_cairo_rectangle( context, &mask );
            cairo_fill( context );
        }

        cairo_rectangle( context, x, y, w, h );
        cairo_rectangle_negative( context, mask.x, mask.y, mask.width, mask.height );
        cairo_clip( context );

        return;

    }

    //_________________________________________________________
    void Style::fileChanged( GFileMonitor*, GFile* file, GFile*, GFileMonitorEvent event, gpointer data )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::Style::fileChanged -"
            << " file: " << g_file_get_path( file )
            << " event: " << Gtk::TypeNames::fileMonitorEvent( event )
            << std::endl;
        #endif

        Style& style( *static_cast<Style*>( data ) );
        if( style.initialize( QtSettings::All|QtSettings::Forced ) )
        { gtk_rc_reset_styles( gtk_settings_get_default() ); }

    }

    //_______________________________________________________________________
    void Style::renderTabCloseIcon(cairo_t* context, GdkRectangle* r) const
    {
        cairo_save(context);
        cairo_translate(context,r->x,r->y);

        cairo_move_to( context, 5.5, 5.5 ); cairo_line_to( context, 10.5, 10.5 );
        cairo_move_to( context, 10.5, 5.5 ); cairo_line_to( context, 5.5, 10.5 );
        cairo_stroke( context );

        cairo_restore(context);
    }

    //_______________________________________________________________________________
    void Style::renderTabCloseButton(cairo_t* context, GdkRectangle* r, const ColorUtils::Rgba& base, const ColorUtils::Rgba& color)
    {
        cairo_save(context);

        cairo_set_source_surface(context,_helper.dockWidgetButton(base,true,r->width),0,0);
        cairo_rectangle(context,r->x,r->y,r->width,r->height);
        cairo_fill(context);

        const double width(1.1);

        // contrast
        cairo_translate(context,0,0.5);
        cairo_set_line_cap( context, CAIRO_LINE_CAP_ROUND );
        cairo_set_line_join( context, CAIRO_LINE_JOIN_ROUND );
        cairo_set_line_width(context,width);
        cairo_set_source(context,ColorUtils::lightColor( base ));

        renderTabCloseIcon(context,r);

        // main icon painting
        cairo_translate(context,0,-1);
        cairo_set_source(context,color);

        renderTabCloseIcon(context,r);

        cairo_restore(context);
    }

    //_________________________________________________________
    void Style::setWindowBlur(GdkWindow* window,bool enable)
    {

        #ifdef GDK_WINDOWING_X11

        // Make whole window blurred
        // FIXME: should roundedness be taken into account?
        #if GTK_CHECK_VERSION(2,24,0)
        const int w( gdk_window_get_width( window ) );
        const int h( gdk_window_get_height( window ) );
        GdkDisplay* gdkDisplay = gdk_window_get_display( window );
        #else
        int w(0), h(0);
        gdk_drawable_get_size( window, &w, &h );
        GdkDisplay* gdkDisplay = gdk_drawable_get_display( window );
        #endif

        const unsigned long rects[4] = { 0, 0, (unsigned long)w, (unsigned long)h };
        const XID id( GDK_WINDOW_XID( window ) );
        Display* display( GDK_DISPLAY_XDISPLAY( gdkDisplay ) );

        if(enable)
        {

            XChangeProperty( display, id, _blurAtom, XA_CARDINAL, 32, PropModeReplace, reinterpret_cast<const unsigned char*>(rects), 4 );

        } else XDeleteProperty( display, id, _blurAtom );

        #endif

    }

}
