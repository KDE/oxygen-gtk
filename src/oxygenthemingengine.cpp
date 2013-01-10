/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
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

#include "oxygencairoutils.h"
#include "oxygengtktypenames.h"
#include "oxygengtkutils.h"
#include "oxygenmetrics.h"
#include "oxygenstyle.h"
#include "config.h"

#include <cmath>

namespace Oxygen
{

    /*
    TODO:
    for now most of the switch are performed using the widget calls (from gtk_widget_path_is_type).
    try modify to use gtk_theming_engine_is_class() instead, as much as possible.
    Available classes are described at: http://library.gnome.org/devel/gtk/unstable/GtkStyleContext.html
    */

    //___________________________________________________________________________________________________________
    GtkThemingEngineClass* ThemingEngine::_parentClass = 0L;
    GTypeInfo ThemingEngine::_typeInfo;
    GType ThemingEngine::_type = 0L;

    //_____________________________________________________________________________________
    Cairo::Surface processTabCloseButton(GtkWidget* widget, GtkStateFlags state)
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::processTabCloseButton" << std::endl;
        #endif

        if( state & GTK_STATE_FLAG_PRELIGHT ) return Style::instance().tabCloseButton( Hover );
        if( state & GTK_STATE_FLAG_ACTIVE ) return Style::instance().tabCloseButton( Focus );
        else {

            // check if our button is on active page and if not, make it gray
            GtkNotebook* notebook=GTK_NOTEBOOK(Gtk::gtk_parent_notebook(widget));
            GtkWidget* page=gtk_notebook_get_nth_page(notebook,gtk_notebook_get_current_page(notebook));
            if( !page ) return 0L;

            GtkWidget* tabLabel=gtk_notebook_get_tab_label(notebook,page);
            if( !tabLabel ) return 0L;

            if( !Gtk::gtk_widget_is_parent( widget, tabLabel ) ) return Style::instance().tabCloseButton( Disabled );
            else return Style::instance().tabCloseButton( StyleOptions() );

        }

        return 0L;

    }

    //___________________________________________________________________________________________________________
    static void render_animated_button(
        cairo_t* context,
        GtkWidget* widget )
    {

        // check widget
        if( !widget ) return;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_animated_button -"
            << " context: " << context
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        ToolBarStateEngine& engine( Style::instance().animations().toolBarStateEngine() );
        engine.registerWidget(widget);

        if( engine.animatedRectangleIsValid( widget ) )
        {

            // TODO: see if offsetting is robust, and can be moved upstream
            GdkRectangle rect( engine.animatedRectangle( widget ) );
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            rect.x -= allocation.x;
            rect.y -= allocation.y;

            Style::instance().renderButtonSlab( context, rect.x, rect.y, rect.width, rect.height, Flat|Hover );

        } else if( engine.isLocked( widget ) && gtk_widget_get_state( engine.widget( widget, AnimationCurrent ) ) != GTK_STATE_ACTIVE ) {

            // TODO: see if offsetting is robust, and can be moved upstream
            GdkRectangle rect( engine.rectangle( widget, AnimationCurrent ) );
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            rect.x -= allocation.x;
            rect.y -= allocation.y;

            Style::instance().renderButtonSlab( context, rect.x, rect.y, rect.width, rect.height, Flat|Hover );

        } else if( engine.isAnimated( widget, AnimationPrevious ) && gtk_widget_get_state( engine.widget( widget, AnimationPrevious ) ) != GTK_STATE_ACTIVE ) {

            // TODO: see if offsetting is robust, and can be moved upstream
            GdkRectangle rect( engine.rectangle( widget, AnimationPrevious ) );
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            rect.x -= allocation.x;
            rect.y -= allocation.y;

            const AnimationData data( engine.animationData( widget, AnimationPrevious ) );
            Style::instance().renderButtonSlab( context, rect.x, rect.y, rect.width, rect.height, Flat|Hover, data );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_line( GtkThemingEngine* engine, cairo_t* context, gdouble x0, gdouble y0, gdouble x1, gdouble y1)
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_line -"
            << " context: " << context
            << " positions: (" << x0 << "," << y0 << ") (" << x1 << "," << y1 << ")"
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // get path
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TOOLBAR ) && !Style::instance().settings().toolBarDrawItemSeparator() )
        {
            // no separators in toolbars, if requested accordingly
            return;

        } else if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_BUTTON ) ) {

            // no separators in buttons
            /* note: can't use gkt_theming_engine_has_class, because it does not work for e.g. font buttons */
            return;

        } else if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TEAROFF_MENU_ITEM ) ) {

            // separators
            bool accepted( true );
            if( widget )
            {
                // do not draw side hlines because they conflict with selection rect
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                if( x0 <= allocation.x + 5 || x1 >= allocation.x + allocation.width - 5 )
                { accepted = false; }
            }

            if( accepted )
            { Style::instance().drawSeparator( widget, context, x0, y0, x1-x0, y1-y0, Blend|Menu ); }

        } else {

            StyleOptions options( Blend );
            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_MENUITEM ) &&
                !gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VIEW ) )
            { options |= Menu; }

            // get orientation
            // TODO: is there a better way ?
            const bool vertical( abs( y1 - y0 ) > abs( x1 - x0 ) );
            if( vertical ) options |= Vertical;

            Style::instance().drawSeparator( widget, context, x0, y0, x1-x0, y1-y0, options );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_background( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_background -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // get path
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        // if widget is invalid, use parent class
        if( !( widget && GTK_IS_WIDGET( widget ) ) )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_background - Calling parentClass()->render_background()\n";
            #endif

            ThemingEngine::parentClass()->render_background( engine, context, x, y, w, h );
            return;

        }

        GtkWidget* toplevel=gtk_widget_get_toplevel(widget);
        if(GTK_IS_DIALOG(toplevel))
        { Style::instance().animations().dialogEngine().registerWidget(toplevel); }

        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TOOLTIP ) )
        {

            StyleOptions options( Round );
            if( Gtk::gtk_widget_has_rgba( widget ) ) options |= Alpha;

            GdkWindow* window( gtk_widget_get_window( widget ) );
            if( GDK_IS_WINDOW( window ) && !gtk_window_get_decorated( GTK_WINDOW( widget ) ) && !(options&Alpha) )
            {

                // make tooltips appear rounded using XShape extension if screen isn't composited
                Style::instance().animations().widgetSizeEngine().registerWidget( widget );
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                const bool sizeChanged( Style::instance().animations().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) );
                if( sizeChanged && ( gtk_widget_is_toplevel(widget) || GTK_IS_WINDOW(widget) ) )
                {
                    Cairo::Region mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_region( window, mask, x, y );
                }

            }


            Style::instance().renderTooltipBackground( context, x, y, w, h, options );
            return;

        } else if( widget && (
            gtk_widget_path_is_type( path, GTK_TYPE_PANED ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_WINDOW ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_VIEWPORT ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_EVENT_BOX )
            ) )
        {

            // register to engines
            Style::instance().animations().mainWindowEngine().registerWidget( widget );

            // check if background image is present
            Cairo::Pattern pattern;
            GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
            gtk_theming_engine_get( engine, state, GTK_STYLE_PROPERTY_BACKGROUND_IMAGE, &pattern, NULL );
            if( pattern.isValid() )
            {
                // if valid background image is found, fallback to parent style
                #if OXYGEN_DEBUG
                std::cerr << "Oxygen::render_background - Calling parentClass()->render_background()\n";
                #endif
                ThemingEngine::parentClass()->render_background( engine, context, x, y, w, h );
                return;
            }

            if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( widget ) )
            { Style::instance().animations().scrollBarEngine().registerScrolledWindow( parent ); }

            // render background gradient
            GdkWindow* window( gtk_widget_get_window( widget ) );

            if( gtk_widget_path_is_type( path, GTK_TYPE_PANED ) && GTK_IS_PANED( widget ) )
            {

                // this is a hack, due to the fact that context is translated in latest gtk3 versions
                // and that there is no way that I could find to retrieve the amount of translation
                GtkWidget* local(0);
                GtkWidget* widget1( gtk_paned_get_child1( GTK_PANED( widget ) ) );
                GtkWidget* widget2( gtk_paned_get_child2( GTK_PANED( widget ) ) );

                if( widget1 && gtk_widget_get_allocated_width( widget1 ) == w && gtk_widget_get_allocated_height( widget1 ) == h )
                {

                    local = widget1;

                } else if( widget2 && gtk_widget_get_allocated_width( widget2 ) == w && gtk_widget_get_allocated_height( widget2 ) == h ) {

                    local = widget2;

                } else local = widget;

                /*
                do not pass the window when rendering background on paned,
                because it breaks positioning
                */
                Style::instance().renderWindowBackground( context, 0L, local, x, y, w, h );

            } else if( gtk_widget_path_is_type( path, GTK_TYPE_VIEWPORT ) ) {

                /*
                FIXME: the w and h adjustment are empirical and fix some rendering issues when
                viewports are embedded inside sunken frames
                */
                Style::instance().renderWindowBackground( context, window, x, y, w+2, h+2 );

            } else {

                // get background color
                GdkRGBA background_gtk;
                gtk_theming_engine_get_background_color( engine, state, &background_gtk );
                StyleOptions options;
                options._customColors.insert( Palette::Window, Gtk::gdk_get_color( background_gtk ) );

                // render background
                Style::instance().renderWindowBackground( context, window, x, y, w, h, options );

            }

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_NOTEBOOK ) ) {

            // no need to render anything for notebook gradient

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_CELL ) ) {

            GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
            StyleOptions options( widget, state );

            // select palete colorgroup for cell background
            Palette::Group group( Palette::Active );
            if( options & Disabled ) group = Palette::Disabled;
            else if( !(options&Focus) ) group = Palette::Inactive;

            if( Gtk::gtk_combobox_is_tree_view( widget ) )
            {
                // combobox tree view have simplified rendering
                // background
                Style::instance().fill( context, x, y, w, h, Style::instance().settings().palette().color( group, Palette::Base ) );

                // draw flat selection in combobox list
                if( state & GTK_STATE_FLAG_SELECTED)
                {
                    const ColorUtils::Rgba selection( Style::instance().settings().palette().color( Palette::Active, Palette::Selected ) );
                    Style::instance().fill( context, x, y, w, h, selection );
                }

                return;

            } else {

                GtkRegionFlags flags;
                const bool isRow( gtk_theming_engine_has_region( engine, GTK_STYLE_REGION_ROW, &flags ) );
                const bool isOdd( isRow && ( flags& GTK_REGION_ODD ) && !Gtk::gtk_combobox_is_tree_view( widget ) );

                // render background
                ColorUtils::Rgba background;
                if( isOdd ) background = Style::instance().settings().palette().color( group, Palette::BaseAlternate );
                else background = Style::instance().settings().palette().color( group, Palette::Base );
                if( background.isValid() ) Style::instance().fill( context, x, y, w, h, background );

                // cell selection and tree lines rendering
                const bool reversed( Gtk::gtk_theming_engine_layout_is_reversed( engine ) );

                // draw rounded selection in normal list,
                // and detect hover
                bool forceCellStart( false );
                bool forceCellEnd( false );

                if( GTK_IS_TREE_VIEW( widget ) )
                {

                    GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                    Gtk::CellInfo cellInfo( treeView, x, y, w, h );

                    Style::instance().animations().treeViewEngine().registerWidget( widget );
                    // if( options & Hover ) Style::instance().animations().treeViewEngine().setHoveredCell( widget, cellInfo );

                    const bool showExpanders( gtk_tree_view_get_show_expanders( treeView ) );
                    if( showExpanders && cellInfo.isValid() && cellInfo.isExpanderColumn( treeView ))
                    {

                        // tree lines
                        if( Style::instance().settings().viewDrawTreeBranchLines() && showExpanders )
                        {

                            // generate flags from cell info
                            Gtk::CellInfoFlags cellFlags( treeView, cellInfo );
                            if( reversed ) cellFlags._flags |= Gtk::CellInfoFlags::Reversed;

                            // set proper options
                            StyleOptions options( widget, state );

                            // and render
                            Style::instance().renderTreeLines( context, x, y, w, h, cellFlags, options );

                        }

                        // change selection rect so that it does not overlap with expander
                        if( reversed ) forceCellEnd = true;
                        else forceCellStart = true;

                        forceCellStart = true;
                        if( options&(Selected|Hover) )
                        {

                            // get expander size from widget
                            int depth( cellInfo.depth() );
                            int expanderSize(0);
                            gtk_widget_style_get( widget, "expander-size", &expanderSize, NULL );

                            int offset( 3 + expanderSize * depth + ( 4 + gtk_tree_view_get_level_indentation( treeView ) )*(depth-1) );

                            if( reversed ) w-= offset;
                            else {

                                x += offset;
                                w -= offset;

                            }

                        }

                    } else if( showExpanders && (options&(Selected|Hover)) && cellInfo.isValid() && cellInfo.isLeftOfExpanderColumn( treeView ) ) {

                        if( reversed ) forceCellStart = true;
                        else forceCellEnd = true;

                    }

                    // check if column is last
                    if( (options&(Selected|Hover)) && cellInfo.isValid() )
                    {
                        if(cellInfo.isLastVisibleColumn( treeView ))
                        {
                            if( reversed ) forceCellStart = true;
                            else forceCellEnd = true;
                        }
                        if(cellInfo.isFirstVisibleColumn( treeView ))
                        {
                            if( reversed ) forceCellEnd = true;
                            else forceCellStart = true;
                        }
                    }

                }

                if( options & (Selected|Hover) )
                {

                    GtkRegionFlags flags;
                    gtk_theming_engine_has_region( engine, GTK_STYLE_REGION_COLUMN, &flags );

                    TileSet::Tiles tiles( TileSet::Center );
                    if( gtk_widget_path_is_type( path, GTK_TYPE_ICON_VIEW ) ) tiles |= (TileSet::Left|TileSet::Right );
                    else {
                        if( flags&GTK_REGION_FIRST ) tiles |= TileSet::Left;
                        if( flags&GTK_REGION_LAST ) tiles |= TileSet::Right;
                    }

                    if( forceCellStart ) tiles |= TileSet::Left;
                    if( forceCellEnd ) tiles |= TileSet::Right;

                    Style::instance().renderSelection( context, x, y, w, h, tiles, options );

                }

            }

        } else if(
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SPINBUTTON ) &&
            !gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_ENTRY ) )
        {

            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TOOLBAR ) ) {

            // render background
            if( !Gtk::gtk_widget_is_applet( widget ) )
            { Style::instance().renderWindowBackground( context, 0L, widget, x, y, w, h ); }

            // possible groupbox background
            if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_FRAME ) )
            { Style::instance().renderGroupBoxBackground( context, widget, x, y, w, h, Blend ); }

            render_animated_button( context, widget );
            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCROLLBAR ) ) {

            // need to render background if there is a parent GtkPaned
            if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_PANED ) )
            { Style::instance().renderWindowBackground( context, 0L, widget, x, y, w, h ); }

            // do nothing otherwise
            return;

        } else if(
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_BUTTON ) ||
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_CHECK ) ||
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCALE ) ||
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_INFO ) ||
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TROUGH ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_LABEL ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_INFO_BAR ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_BUTTON ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_MENU_ITEM ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_IMAGE ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_EXPANDER ) )
        {

            /*
            Note: widget_path is used above instead of engine_class,
            because some widgets do not pass the correct "class"
            when rendering background
            */
            return;

        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_background - Calling parentClass()->render_background()\n";
            #endif
            ThemingEngine::parentClass()->render_background( engine, context, x, y, w, h );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_frame( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_frame -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // load state, path, and widget
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );
        GtkWidget* parent( 0L );

        // load border style
        GtkBorderStyle borderStyle;
        gtk_theming_engine_get( engine, state, GTK_STYLE_PROPERTY_BORDER_STYLE, &borderStyle, NULL );

        if( Gtk::gtk_combobox_is_scrolled_window( widget ) ) {

            // make GtkCombo list look a bit better
            // retrieve proper parent and check
            GtkWidget* parent = gtk_widget_get_parent(widget);
            if( !( parent && GTK_IS_WINDOW(parent) ) ) return;

            // setup options
            StyleOptions options( Round );
            if( Gtk::gtk_widget_has_rgba(parent) ) options|=Alpha;

            const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( parent ) );

            // always register to widget size engine
            Style::instance().animations().widgetSizeEngine().registerWidget( parent );
            const bool sizeChanged( Style::instance().animations().widgetSizeEngine().updateSize( parent, allocation.width, allocation.height ) );
            const bool wasAlpha(Style::instance().animations().widgetSizeEngine().wasAlpha(parent));

            if( sizeChanged || (!(options&Alpha) && wasAlpha) || (!wasAlpha && (options&Alpha)) )
            {
                if( !(options&Alpha) )
                {
                    // the same as with menus and tooltips (but changed a bit to take scrollbars into account)
                    // make background window rounded
                    Cairo::Region mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_region( gtk_widget_get_window( parent ), mask, 0, 0 );

                    Style::instance().animations().widgetSizeEngine().setAlpha(parent, false);
                }
                else if( !wasAlpha )
                {
                    // reset XShape mask on transition from non-composited to composited
                    gdk_window_shape_combine_region( gtk_widget_get_window( parent ), NULL, 0, 0 );

                    Style::instance().animations().widgetSizeEngine().setAlpha(parent, true);
                }
#if !ENABLE_INNER_SHADOWS_HACK
                // also sets inner list mask
                if( GtkWidget* child = gtk_bin_get_child( GTK_BIN( widget ) ) )
                {

                    const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( child ) );

                    // offset is needed to make combobox list border 3px wide instead of default 2
                    // additional pixel is for ugly shadow
                    const gint offset( options&Alpha ? 0:1 );
                    Cairo::Region mask( Style::instance().helper().innerRoundMask(
                        allocation.width - 2*offset,
                        allocation.height - 2*offset ) );

                    gdk_window_shape_combine_region( gtk_widget_get_window( child ), mask, offset, offset );

                }
#endif
            }

            // menu background and float frame
            {
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                Style::instance().renderMenuBackground(
                    context, allocation.x, allocation.y, allocation.width, allocation.height, options );

                Style::instance().drawFloatFrame( context,
                    allocation.x, allocation.y, allocation.width, allocation.height,
                    options );
            }

            return;

        }

        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TROUGH ) )
        {

            if( gtk_widget_path_is_type( path, GTK_TYPE_PROGRESS_BAR ) )
            {

                StyleOptions options(widget, state);
                if(GTK_IS_PROGRESS_BAR(widget) && gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) )
                { options|=Vertical; }

                Style::instance().renderProgressBarHole( context, x, y, w, h, options );
                return;

            } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_CELL ) ) {

                StyleOptions options(widget, state);
                Style::instance().renderProgressBarHole( context, x-1, y-1, w+2, h+2, options );
                return;


            } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCALE ) && GTK_IS_SCALE( widget ) ) {

                const bool vertical( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) );

                const int offset( 6 );
                TileSet::Tiles tiles( TileSet::Full );

                #if GTK_CHECK_VERSION( 3, 3, 0 )
                if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_LEFT ) ) { tiles &= ~TileSet::Right; x += offset; }
                else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_RIGHT ) ) { tiles &= ~TileSet::Left; w -= offset; }
                else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TOP ) ) { tiles &= ~TileSet::Bottom; y += offset; }
                else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_BOTTOM ) ) { tiles &= ~TileSet::Top; h -= offset; }
                else if( vertical ) { y += offset; h -= 2*offset; }
                else { x+= offset; w -= 2*offset; }
                #else
                if( vertical ) { y += offset; h -= 2*offset; }
                else { x+= offset; w -= 2*offset; }
                #endif

                // render
                Style::instance().renderSliderGroove( context, x, y, w, h, vertical ? Vertical:StyleOptions(), tiles );

                return;

            }


        }

        // adjust shadow type for some known widgets
        if( gtk_widget_path_is_type( path, GTK_TYPE_SCROLLED_WINDOW ) &&
            borderStyle !=  GTK_BORDER_STYLE_INSET &&
            Gtk::gtk_scrolled_window_force_sunken( widget ) )
        {

            // make sure that scrolled windows containing a treeView have sunken frame
            borderStyle = GTK_BORDER_STYLE_INSET;
            gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( widget ), GTK_SHADOW_IN );

        } else if(
            gtk_widget_path_is_type( path, GTK_TYPE_FRAME ) &&
            borderStyle == GTK_BORDER_STYLE_SOLID &&
            Gtk::gtk_scrolled_window_force_sunken( widget )
            )
        {

            // make sure that entry shadows are drawn
            borderStyle = GTK_BORDER_STYLE_INSET;
            gtk_frame_set_shadow_type( GTK_FRAME( widget ), GTK_SHADOW_IN );

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_ENTRY ) && borderStyle !=  GTK_BORDER_STYLE_INSET ) {

            // make sure that entry shadows are drawn
            borderStyle = GTK_BORDER_STYLE_INSET;

        }

        if( gtk_widget_path_is_type( path, GTK_TYPE_INFO_BAR ) )
        {

            // get background color
            GdkRGBA background;
            gtk_theming_engine_get_background_color( engine, state, &background );
            Style::instance().renderInfoBar( widget, context, x, y, w, h, Gtk::gdk_get_color( background ) );

        } else if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_STATUSBAR ) && borderStyle == GTK_BORDER_STYLE_INSET ) {

            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TOOLTIP ) ) {

            // do nothing for tooltips
            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_BUTTON ) ) {

            // no frame for scrollbar buttons, spin buttons, and menu buttons
            if(
                gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCROLLBAR ) ||
                gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SPINBUTTON ) ||
                gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_MENU ) )
            { return; }

            // pathbar buttons
            if( Gtk::gtk_button_is_in_path_bar(widget) )
            {

                // https://bugzilla.gnome.org/show_bug.cgi?id=635511
                std::string name(G_OBJECT_TYPE_NAME( gtk_widget_get_parent( widget ) ) );
                Style::instance().animations().hoverEngine().registerWidget( widget );

                // only two style options possible: hover or don't draw
                StyleOptions options;
                const bool reversed( Gtk::gtk_theming_engine_layout_is_reversed( engine ) );
                const bool isLast( Gtk::gtk_path_bar_button_is_last( widget ) );
                if( !( state & (GTK_STATE_FLAG_NORMAL|GTK_STATE_FLAG_INSENSITIVE ) ) )
                {
                    if( (state&GTK_STATE_FLAG_PRELIGHT) || Style::instance().animations().hoverEngine().hovered( widget ) )
                    {
                        options |= Hover;
                        if( isLast )
                        {
                            if( reversed )
                            {

                                x += 10;
                                w-=10;

                            } else w -= 10;
                        }

                        Style::instance().renderSelection( context, x, y, w, h, TileSet::Full, options );
                    }
                }

                if( GTK_IS_TOGGLE_BUTTON(widget) && !isLast )
                {

                    options |= Contrast;

                    if( reversed ) Style::instance().renderArrow( context, GTK_ARROW_LEFT, x+3, y, 5, h, QtSettings::ArrowNormal, options, Palette::WindowText);
                    else Style::instance().renderArrow( context, GTK_ARROW_RIGHT, x+w-8, y, 5, h, QtSettings::ArrowNormal, options, Palette::WindowText);

                }

                return;

            } else if( ( parent = Gtk::gtk_parent_combobox_entry( widget ) ) ) {

                // combobox entry buttons
                // keep track of whether button is active (pressed-down) or pre-lighted
                const bool buttonActive( state&(GTK_STATE_FLAG_ACTIVE|GTK_STATE_FLAG_PRELIGHT) );

                // get the state from the combobox
                /* this fixes rendering issues when the arrow is disabled, but not the entry */
                state = gtk_widget_get_state_flags(parent);

                /*
                editable combobox button get a hole (with left corner hidden), and a background
                that match the corresponding text entry background.
                */

                StyleOptions options( widget, state );
                options |= NoFill|Blend;

                // focus handling
                Style::instance().animations().comboBoxEntryEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEntryEngine().setButton( parent, widget );

                // fill background manually
                Palette::Group group( (options & Disabled) ? Palette::Disabled : Palette::Active );
                Style::instance().fill( context, x, y, w, h, Style::instance().settings().palette().color( group, Palette::Base ) );

                // update option accordingly
                if( state&GTK_STATE_FLAG_INSENSITIVE ) options &= ~(Hover|Focus);
                else {

                    Style::instance().animations().comboBoxEntryEngine().setButtonFocus( parent, options & Focus );
                    if( Style::instance().animations().comboBoxEntryEngine().hasFocus( parent ) ) options |= Focus;
                    else options &= ~Focus;

                    // properly set button hover state. Pressed-down buttons are marked hovered, consistently with Qt
                    Style::instance().animations().comboBoxEntryEngine().setButtonHovered( parent, buttonActive );
                    if( Style::instance().animations().comboBoxEntryEngine().hovered( parent ) ) options |= Hover;
                    else options &= ~Hover;

                }

                // render
                // GdkWindow* window( gtk_widget_get_window( widget ) );
                TileSet::Tiles tiles( TileSet::Ring);
                const AnimationData data( Style::instance().animations().widgetStateEngine().get( parent, options, AnimationHover|AnimationFocus, AnimationFocus ) );
                if( Gtk::gtk_theming_engine_layout_is_reversed( engine ) )
                {

                    // hide right and adjust width
                    tiles &= ~TileSet::Right;
                    Style::instance().renderHoleBackground( context, 0L, widget, x-1, y, w+6, h, tiles );

                    x += Oxygen::Entry_SideMargin;
                    w -= Oxygen::Entry_SideMargin;
                    Style::instance().renderHole( context, x-1, y, w+8, h, options, data, tiles  );

                } else {

                    // hide left and adjust width
                    tiles &= ~TileSet::Left;
                    Style::instance().renderHoleBackground( context, 0L, widget, x-5, y, w+6, h, tiles );

                    w -= Oxygen::Entry_SideMargin;
                    Style::instance().renderHole( context, x-7, y, w+8, h, options, data, tiles  );

                }

                return;

            } else if( ( parent = Gtk::gtk_parent_combobox( widget ) ) && Gtk::gtk_combobox_appears_as_list( parent ) ) {

                // combobox buttons
                const bool reversed( Gtk::gtk_theming_engine_layout_is_reversed( engine ) );
                StyleOptions options( widget, state );
                options |= Blend;

                Style::instance().animations().comboBoxEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEngine().setButton( parent, widget );
                Style::instance().animations().comboBoxEngine().setButtonFocus( parent, options & Focus );

                if( Gtk::gtk_combobox_has_frame( parent ) )
                {
                    if( Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;

                    // tiles
                    TileSet::Tiles tiles( TileSet::Ring );

                    // animation state
                    const AnimationData data( (options&Sunken) ?
                        AnimationData():
                        Style::instance().animations().widgetStateEngine().get( parent, options ) );

                    if( reversed )
                    {

                        tiles &= ~TileSet::Right;
                        Style::instance().renderButtonSlab( widget, context, x, y, w+7, h, options, data, tiles );

                    } else {

                        tiles &= ~TileSet::Left;
                        Style::instance().renderButtonSlab( widget, context, x-7, y, w+7, h, options, data, tiles );

                    }

                    return;

                } else {

                    options |= Flat;
                    if( Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;
                    if( reversed ) Style::instance().renderButtonSlab( widget, context, x+1, y, w, h, options );
                    else Style::instance().renderButtonSlab( widget, context, x-1, y, w, h, options );
                    return;

                }

            } else if( Gtk::gtk_button_is_header( widget )
                || gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_HEADER )
                || gtk_theming_engine_has_region( engine, GTK_STYLE_REGION_COLUMN_HEADER, 0L ) ) {

                // treeview headers
                // register to scrolled window engine if any
                if(
                    ( parent = Gtk::gtk_parent_scrolled_window( widget ) ) &&
                    Style::instance().animations().scrolledWindowEngine().contains( parent )
                    )
                { Style::instance().animations().scrolledWindowEngine().registerChild( parent, widget ); }

                // treevew header
                Style::instance().renderHeaderBackground( context, 0L, widget, x, y, w, h );
                return;

            } else if( Gtk::gtk_notebook_is_close_button(widget)) {

                // notebook close buttons
                if( gtk_button_get_relief(GTK_BUTTON(widget))==GTK_RELIEF_NONE )
                { gtk_button_set_relief(GTK_BUTTON(widget),GTK_RELIEF_NORMAL); }

                if( Cairo::Surface surface = processTabCloseButton( widget, state ) )
                {

                    // hide previous image
                    // show ours instead
                    if( GtkWidget* image = Gtk::gtk_button_find_image(widget) )
                    { gtk_widget_hide(image); }

                    // center the button image
                    const int height( cairo_surface_get_height( surface ) );
                    const int width( cairo_surface_get_width( surface ) );
                    x=x+(w-width)/2;
                    y=y+(h-height)/2;

                    // render the image
                    cairo_save( context );
                    cairo_set_source_surface( context, surface, x, y);
                    cairo_paint(context);
                    cairo_restore( context );

                }

                return;

            } else if( GTK_IS_TOOL_ITEM_GROUP( widget ) ) {

                // tool itemgroup buttons
                return;

            }

            StyleOptions options( widget, state );
            options |= Blend;

            // TODO: reimplement with Gtk3
            // if( style )
            // { options._customColors.insert( options&Flat ? Palette::Window:Palette::Button, Gtk::gdk_get_color( style->bg[state] ) ); }

            // flat buttons
            bool useWidgetState( true );
            AnimationData data;
            if( widget && Gtk::gtk_button_is_flat( widget ) )
            {

                // set button as flat and disable focus
                options |= Flat;
                options &= ~Focus;

                // register to Hover engine and check state
                Style::instance().animations().hoverEngine().registerWidget( widget );
                if( Style::instance().animations().hoverEngine().hovered( widget ) )
                { options |= Hover; }

                // register to ToolBarState engine
                GtkWidget* parent( 0L );
                ToolBarStateEngine& engine( Style::instance().animations().toolBarStateEngine() );
                if( !Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TOOL_PALETTE ) && (parent = engine.findParent( widget ) ) )
                {

                    // register child
                    engine.registerChild( parent, widget, options&Hover );
                    useWidgetState = false;

                    if( engine.animatedRectangleIsValid( parent ) && !(options&Sunken) ) {

                        return;

                    } if( engine.widget( parent, AnimationCurrent ) == widget ) {

                        data = engine.animationData( parent, AnimationCurrent );

                        if( engine.isLocked( parent ) ) options |= Hover;

                    } else if( (options & Sunken ) && engine.widget( parent, AnimationPrevious ) == widget ) {

                        data = engine.animationData( parent, AnimationPrevious );

                    }

                }

            }

            // retrieve animation
            if( useWidgetState )
            { data = Style::instance().animations().widgetStateEngine().get( widget, options ); }

            // render
            Style::instance().renderButtonSlab( widget, context, x, y, w, h, options, data );
            return;

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_MENU_BAR ) ) {

            // render background
            if( !Gtk::gtk_widget_is_applet( widget ) )
            { Style::instance().renderWindowBackground( context, 0L, widget, x, y, w, h ); }

            // possible groupbox background
            if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_FRAME ) )
            { Style::instance().renderGroupBoxBackground( context, widget, x, y, w, h, Blend ); }

            MenuBarStateEngine& engine( Style::instance().animations().menuBarStateEngine() );
            engine.registerWidget(widget);

            // draw animated or fade-out rect
            if( engine.animatedRectangleIsValid( widget ) )
            {

                const GdkRectangle& rect( engine.animatedRectangle( widget ) );
                Style::instance().renderMenuItemRect( context, 0L, engine.widget( widget, AnimationCurrent ), rect.x, rect.y, rect.width, rect.height, Hover|Blend );

            } else if( engine.isAnimated( widget, AnimationPrevious ) ) {

                const AnimationData data( engine.animationData( widget, AnimationPrevious ) );
                const GdkRectangle& rect( engine.rectangle( widget, AnimationPrevious ) );
                Style::instance().renderMenuItemRect( context, 0L, engine.widget( widget, AnimationPrevious ), rect.x, rect.y, rect.width, rect.height, Hover|Blend, data );

            }
            return;

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_MENU ) ) {

            if( GTK_IS_MENU( widget ) && gtk_menu_get_tearoff_state( GTK_MENU( widget ) ) )
            {

                GdkWindow* window( gtk_widget_get_window( widget ) );
                if( Gtk::gdk_window_is_base( window ) )
                { Style::instance().animations().backgroundHintEngine().registerWidget( widget ); }

                Style::instance().renderWindowBackground( context, window, widget, x, y, w, h );

            } else {

                StyleOptions options( Menu|Round );

                // this is not working.
                if( Gtk::gtk_widget_has_rgba( widget ) ) options |= Alpha;

                // add mask if needed
                if( GTK_IS_MENU(widget) )
                {

                    Style::instance().animations().menuItemEngine().registerMenu( widget );

                    GdkWindow* window( gtk_widget_get_parent_window(widget) );
                    if( !(options&Alpha) )
                    {

                        // make menus appear rounded using XShape extension if screen isn't composited
                        Style::instance().animations().widgetSizeEngine().registerWidget( widget );
                        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                        if( Style::instance().animations().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) )
                        {
                            Cairo::Region mask( Style::instance().helper().roundMask( w, h - 2*Oxygen::Menu_VerticalOffset ) );
                            gdk_window_shape_combine_region( window, mask, 0, Oxygen::Menu_VerticalOffset );
                        }

                    } else {

                        // reset mask if compositing has appeared after we had set a mask
                        gdk_window_shape_combine_region( window, 0L, 0, 0);

                    }
                }

                // if rendering of menu background fails, assume square window
                if( !Style::instance().renderMenuBackground( context, x, y, w, h, options ) )
                { options &= ~Round; }

                Style::instance().drawFloatFrame( context, x, y, w, h, options );

            }

            // TODO: this check is probably not necessary
            if( GTK_IS_MENU( widget ) )
            {

                // check animation state
                MenuStateEngine& engine( Style::instance().animations().menuStateEngine() );
                engine.registerWidget(widget);

                if( engine.animatedRectangleIsValid( widget ) )
                {

                    const GdkRectangle& rect( engine.animatedRectangle( widget ) );
                    Style::instance().renderMenuItemRect( context, 0L, engine.widget( widget, AnimationCurrent ), rect.x, rect.y, rect.width, rect.height, Hover );

                } else if( engine.isLocked( widget ) ) {

                    const GdkRectangle& rect( engine.rectangle( widget, AnimationCurrent ) );
                    Style::instance().renderMenuItemRect( context, 0L, engine.widget( widget, AnimationCurrent ), rect.x, rect.y, rect.width, rect.height, Hover );

               } else if( engine.isAnimated( widget, AnimationPrevious ) ) {

                    const AnimationData data( engine.animationData( widget, AnimationPrevious ) );
                    const GdkRectangle& rect( engine.rectangle( widget, AnimationPrevious ) );
                    Style::instance().renderMenuItemRect( context, 0L, engine.widget( widget, AnimationPrevious ), rect.x, rect.y, rect.width, rect.height, Hover, data );

                 }

            }
            return;

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_MENU_ITEM ) ) {

            #if GTK_CHECK_VERSION( 3, 7, 0 )
            // for gtk 3.7 and above, menu item rect is called even in not PRELIGHT mode
            // we need to draw nothing for it
            // bug: 312988
            GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
            if( !( state & GTK_STATE_FLAG_PRELIGHT ) ) return;
            #endif

            if( GTK_IS_MENU_ITEM( widget ) )
            {
                GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
                Style::instance().animations().menuItemEngine().registerWidget( child );
            }

            GtkWidget* parent( widget ? gtk_widget_get_parent( widget ):0L );
            AnimationData data;
            if( GTK_IS_MENU_BAR( parent ) )
            {

                MenuBarStateEngine& engine = Style::instance().animations().menuBarStateEngine();
                engine.registerWidget( parent );
                if( engine.animatedRectangleIsValid( parent ) )
                {
                    return;

                } else if( engine.widget( parent, AnimationCurrent ) == widget ) {

                    data = engine.animationData( parent, AnimationCurrent );

                }

            } else if( GTK_IS_MENU( parent ) ) {

                MenuStateEngine& engine = Style::instance().animations().menuStateEngine();
                engine.registerWidget( parent );

                if( engine.animatedRectangleIsValid( parent ) ) {

                    return;

                } else if( engine.widget( parent, AnimationCurrent ) == widget ) {

                    data = engine.animationData( parent, AnimationCurrent );

                }

            }

            StyleOptions options( widget, state );
            options |= Blend;

            Style::instance().renderMenuItemRect( context, 0L, widget, x, y, w, h, options, data );
            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_TROUGH ) ) {


            StyleOptions options;
            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) ) options |= Vertical;

            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCROLLBAR ) )
            { Style::instance().adjustScrollBarHole( x, y, w, h, options ); }

            if( w>0 && h>0 )
            {
                if( options&Vertical ) Style::instance().renderScrollBarHole( context, x, y+1, w, h-1, options );
                else  Style::instance().renderScrollBarHole( context, x+1, y, w-2, h, options );
            }

            return;

        } else if( (
            gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_ENTRY ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_VIEWPORT ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_SCROLLED_WINDOW )
            ) && borderStyle == GTK_BORDER_STYLE_INSET )
        {

            StyleOptions options( widget, state );
            options |= NoFill;

            if( GtkWidget* parent = Gtk::gtk_parent_combobox_entry( widget ) )
            {

                // check if parent is in style map
                Style::instance().animations().comboBoxEntryEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEntryEngine().setEntry( parent, widget );
                Style::instance().animations().comboBoxEntryEngine().setEntryFocus( parent, options & Focus );

                if( Style::instance().animations().comboBoxEntryEngine().hasFocus( parent ) ) options |= Focus;
                else options &= ~Focus;

                if(  Style::instance().animations().comboBoxEntryEngine().hovered( parent ) ) options |= Hover;
                else options &= ~Hover;

                // render
                TileSet::Tiles tiles( TileSet::Ring );
                const AnimationData data( Style::instance().animations().widgetStateEngine().get( parent, options, AnimationHover|AnimationFocus, AnimationFocus ) );

                // background color
                // TODO: use correct Palette::Group depending on state
                // fill background manually
                Palette::Group group( (options & Disabled) ? Palette::Disabled : Palette::Active );
                const ColorUtils::Rgba background( Style::instance().settings().palette().color( group, Palette::Base ) );

                if( Gtk::gtk_theming_engine_layout_is_reversed( engine ) )
                {

                    tiles &= ~TileSet::Left;
                    Style::instance().fill( context, x-6, y, w+7, h, background );
                    Style::instance().renderHoleBackground( context, 0L, widget, x-6, y, w+7, h, tiles );

                    w -= Oxygen::Entry_SideMargin;
                    Style::instance().renderHole( context, x-8, y, w+9, h, options, data, tiles );

                } else {

                    tiles &= ~TileSet::Right;

                    Style::instance().fill( context, x-1, y, w+7, h, background );
                    Style::instance().renderHoleBackground( context, 0L, widget, x-1, y, w+7, h, tiles );

                    x += Oxygen::Entry_SideMargin;
                    w -= Oxygen::Entry_SideMargin;
                    Style::instance().renderHole( context, x-1, y, w+9, h, options, data, tiles );

                }

            } else {

                // register to hover engine
                if( GTK_IS_ENTRY( widget ) )
                {

                    Style::instance().animations().hoverEngine().registerWidget( widget, true );
                    if( Style::instance().animations().hoverEngine().hovered( widget ) )
                    { options |= Hover; }

                } else if( GTK_IS_SCROLLED_WINDOW( widget ) ) {

                    Style::instance().animations().scrolledWindowEngine().registerWidget( widget );

                    options &= ~(Hover|Focus);
                    if( Style::instance().animations().scrolledWindowEngine().focused( widget ) ) options |= Focus;
                    if( Style::instance().animations().scrolledWindowEngine().hovered( widget ) ) options |= Hover;


                } else {

                    options &= ~(Hover|Focus);

                }


                // basic adjustments
                x-=1; y-=1;
                w+=2; h+=2;

                Style::instance().renderHoleBackground( context, 0L, widget, x, y, w, h );

                // shrink entry by 3px at each side
                if( GTK_IS_ENTRY( widget ) )
                {

                    x += Oxygen::Entry_SideMargin;
                    w -= 2*Oxygen::Entry_SideMargin;

                }

                // animation
                const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover|AnimationFocus, AnimationFocus ) );

                // FIXME: having anything other than shadow_in here looks like a bug, but we still do for GtkIconView case
                if(!Style::instance().animations().innerShadowEngine().contains(widget) ||
                       (GTK_IS_SCROLLED_WINDOW(widget) && gtk_scrolled_window_get_shadow_type(GTK_SCROLLED_WINDOW(widget))!=GTK_SHADOW_IN))
                {

                    Style::instance().renderHole( context, x, y, w, h, options, data );

                } else {

                    Style::instance().renderHole( context, x+1, y+1, w-2, h-2, options, data );

                }

            }

            return;

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_SPIN_BUTTON ) ) {

            return;

        } else if(
            (parent = Gtk::gtk_parent_combobox( widget )) &&
            !gtk_combo_box_get_has_entry( GTK_COMBO_BOX( parent ) ) &&
            !GTK_IS_CELL_VIEW( widget ) )
        {

            Style::instance().animations().comboBoxEngine().registerWidget( parent );
            Style::instance().animations().comboBoxEngine().registerChild( parent, widget );

            StyleOptions options( widget, state );
            options |= Blend;

            if( !Style::instance().animations().comboBoxEngine().isSensitive( parent ) ) options |= Disabled;

            if( Style::instance().animations().comboBoxEngine().pressed( parent ) ) options |= Sunken;
            else options &= ~Sunken;

            if( Style::instance().animations().comboBoxEngine().hasFocus( parent ) ) options |= Focus;
            else options &= ~Focus;

            if(  Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;
            else options &= ~Hover;

            // animation state
            const AnimationData data( (options&Sunken) ? AnimationData():Style::instance().animations().widgetStateEngine().get( parent, options ) );

            // tiles
            TileSet::Tiles tiles( TileSet::Ring );

            if( Gtk::gtk_theming_engine_layout_is_reversed( engine ) )
            {

                tiles &= ~TileSet::Left;
                Style::instance().renderButtonSlab( widget, context, x-10, y, w+10, h, options, data, tiles );

            } else {

                tiles &= ~TileSet::Right;
                Style::instance().renderButtonSlab( widget, context, x, y, w+10, h, options, data, tiles );

            }

            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_NOTEBOOK ) && GTK_IS_NOTEBOOK( widget ) && !gtk_notebook_get_show_tabs( GTK_NOTEBOOK( widget ) ) ) {

            /*
            disable border padding and paint nothing for notebooks
            for which tabs are hidden. This is consistent with Qt Version.
            */

            gtk_notebook_set_show_border( GTK_NOTEBOOK(widget), FALSE );

            return;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_FRAME ) && widget && GTK_IS_FRAME( widget ) ) {

            // check groupbox
            if( Gtk::gtk_widget_is_groupbox( widget ) )
            {

                Style::instance().renderGroupBoxFrame( context, widget, x-1, y-1, w+2, h+2, Blend );
                return;

            }

            // force sunken for sidebar frames
            if(
                gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SIDEBAR ) &&
                Gtk::gtk_widget_path_has_type( path, GTK_TYPE_ASSISTANT ) )
            {

                Style::instance().renderHoleBackground( context, 0L, widget, x-1-Oxygen::Entry_SideMargin, y-1, w+2+2*Oxygen::Entry_SideMargin, h+1 );
                Style::instance().renderHole( context, x-1, y-1, w+2, h+1, NoFill );
                return;

            }

            // standard case
            switch( gtk_frame_get_shadow_type( GTK_FRAME( widget ) ) )
            {
                case GTK_SHADOW_IN:
                Style::instance().renderHole( context, x-1, y-1, w+2, h+1, NoFill );
                break;

                case GTK_SHADOW_OUT:
                Style::instance().renderSlab( context, x-1, y-1, w+2, h+2, NoFill );
                break;

                default:
                case GTK_SHADOW_ETCHED_IN:
                case GTK_SHADOW_ETCHED_OUT:
                Style::instance().renderDockFrame( context, x, y+1, w, h-2, Blend );
                break;

            }

            return;

        } else if( borderStyle == GTK_BORDER_STYLE_INSET ) {

            // default shadow_in frame
            // hole background is needed for some special cases
            if( GTK_IS_CALENDAR( widget ) )
            {

                Style::instance().renderHoleBackground(
                    context, 0L, widget,
                    x-1-Oxygen::Entry_SideMargin, y-1, w+2+2*Oxygen::Entry_SideMargin, h+2 );
            }

            // hole
            Style::instance().renderHole( context, x-1, y-1, w+2, h+1, NoFill );
            return;

        } else if( borderStyle == GTK_BORDER_STYLE_SOLID && !Gtk::gtk_widget_path_has_type( path, GTK_TYPE_BUTTON ) ) {

            // default etched frame
            Style::instance().renderDockFrame( context, x, y+1, w, h-2, Blend );
            return;

        } else if( borderStyle == GTK_BORDER_STYLE_OUTSET ) {

            // default shadow_out frame
            Style::instance().renderSlab( context, x-1, y-1, w+2, h+2, Blend|NoFill );
            return;

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_frame_gap(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, gdouble w, gdouble h,
        GtkPositionType position,
        gdouble xy0_gap, gdouble xy1_gap)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_frame_gap -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " side: " << Gtk::TypeNames::position( position )
            << " gap: (" << xy0_gap << "," << xy1_gap << ")"
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // load state path and widget
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        // load border style
        GtkBorderStyle borderStyle;
        gtk_theming_engine_get( engine, state, GTK_STYLE_PROPERTY_BORDER_STYLE, &borderStyle, NULL );

        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_NOTEBOOK ) )
        {

            // this might move to drawShadowGap
            StyleOptions options( widget, state );
            options |= NoFill;
            options &= ~(Hover|Focus);

            if( GTK_IS_NOTEBOOK( widget ) && !Gtk::gdk_default_screen_is_composited() )
            {

                // this trick ensures that tabbar is always redrawn
                Style::instance().animations().tabWidgetEngine().registerWidget( widget );
                if( Style::instance().animations().tabWidgetEngine().isDirty( widget ) )
                {
                    Style::instance().animations().tabWidgetEngine().setDirty( widget, false );

                } else {

                    Style::instance().animations().tabWidgetEngine().setDirty( widget, true );

                }

            }

            Gtk::Gap gap;

            // need adjustment depending on gap side
            const int adjust = 2;
            switch( position )
            {

                case GTK_POS_TOP:
                gap = Gtk::Gap( 0, w+2, position );
                y -= adjust;
                h += adjust;
                break;

                case GTK_POS_BOTTOM:
                gap = Gtk::Gap( 0, w+2, position );
                h += adjust;
                break;

                case GTK_POS_LEFT:
                gap = Gtk::Gap( 0, h+2, position );
                x -= adjust;
                w +=  adjust;
                break;

                case GTK_POS_RIGHT:
                gap = Gtk::Gap( 0, h+2, position );
                w += adjust;
                break;

                default: return;

            }

            gap.setHeight( 8 );
            Style::instance().renderTabBarFrame( context, x-1, y-1, w+2, h+2, gap, options );

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_FRAME ) ) {

            const Gtk::Gap gap( std::min( xy0_gap, xy1_gap ), std::abs(xy1_gap-xy0_gap), position );

            GtkShadowType shadow( GTK_SHADOW_NONE );
            if( GTK_IS_FRAME( widget ) ) shadow = gtk_frame_get_shadow_type( GTK_FRAME( widget ) );
            else if( borderStyle == GTK_BORDER_STYLE_INSET ) shadow = GTK_SHADOW_IN;
            else if( borderStyle == GTK_BORDER_STYLE_OUTSET ) shadow = GTK_SHADOW_OUT;
            else shadow = GTK_SHADOW_ETCHED_IN;

            // draw frame depending on shadow type
            if( shadow == GTK_SHADOW_IN ) {

                Style::instance().renderHoleBackground( context, 0L, widget, x - 1 - Oxygen::Entry_SideMargin, y-1, w + 2 + 2*Oxygen::Entry_SideMargin, h+2 );
                Style::instance().renderHole( context, x-1, y-1, w+2, h+1, gap, NoFill );

            } else if( shadow == GTK_SHADOW_OUT ) {

                Style::instance().renderSlab( context, x-1, y-4, w+2, h+4, gap, NoFill );

            } else {

                Style::instance().renderDockFrame( widget, context, x, y-1, w, h+1, gap, Blend );

            }


        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_frame_gap - Calling parentClass()->render_frame_gap()\n";
            #endif
            ThemingEngine::parentClass()->render_frame_gap( engine, context, x, y, w, h, position, xy0_gap, xy1_gap );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_extension(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, gdouble w, gdouble h,
        GtkPositionType position )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_extension -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " side: " << Gtk::TypeNames::position( position )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // load state, path and widget
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        // check classs
        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_NOTEBOOK ) )
        {

            StyleOptions options( widget, state );
            TabOptions tabOptions( widget, state, position, x, y, w, h );

            const bool isCurrentTab( tabOptions & CurrentTab );
            bool drawTabBarBase( isCurrentTab );
            bool dragInProgress( false );

            /*
            see if tab is hovered. This is only done if widget is notebook, and if not running a mozilla
            (or open office) app, because the latter do not pass the actual tab rect as argument
            */
            AnimationData data;
            if( GTK_IS_NOTEBOOK( widget ) )
            {

                // make sure widget is registered
                Style::instance().animations().tabWidgetEngine().registerWidget( widget );

                // get current tab, update tabRect and see if current tab is hovered
                const int tabIndex( Gtk::gtk_notebook_find_tab( widget, x+w/2, y+h/2 ) );
                Style::instance().animations().tabWidgetEngine().updateTabRect( widget, tabIndex, x, y, w, h );
                if( tabIndex == Style::instance().animations().tabWidgetEngine().hoveredTab( widget ) )
                { options |= Hover; }

                // check tab position and add relevant option flags
                GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
                if( tabIndex == 0 ) tabOptions |= FirstTab;
                if( tabIndex == gtk_notebook_get_n_pages( notebook ) - 1 ) tabOptions |= LastTab;

                const int current( gtk_notebook_get_current_page( notebook ) );
                if( tabIndex == current-1 ) tabOptions |= LeftOfSelected;
                else if( tabIndex == current+1 ) tabOptions |= RightOfSelected;

                // update drag in progress flag
                if( isCurrentTab )
                {
                    // TODO: reimplement with gtk+3
                    // const bool drag( widget && (window != gtk_widget_get_window( widget ) ) );
                    const bool drag( false );
                    Style::instance().animations().tabWidgetEngine().setDragInProgress( widget, drag );
                }

                dragInProgress = Style::instance().animations().tabWidgetEngine().dragInProgress( widget );

                // this does not work when the first tab is being grabbed
                if( dragInProgress )
                {
                    int firstTabIndex( Gtk::gtk_notebook_find_first_tab( widget ) );
                    int focusTabIndex( gtk_notebook_get_current_page( notebook ) );
                    drawTabBarBase = (tabIndex == firstTabIndex && !isCurrentTab ) || (firstTabIndex == focusTabIndex && tabIndex == firstTabIndex+1 );
                }

                if( !isCurrentTab )
                { data = Style::instance().animations().tabWidgetStateEngine().get( widget, tabIndex, options ); }

            }

            Style::instance().renderTab( context, x, y, w, h, position, options, tabOptions, data );

            // render tabbar base if current tab
            if( drawTabBarBase )
            {

                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                int borderWidth( GTK_IS_CONTAINER( widget ) ? gtk_container_get_border_width( GTK_CONTAINER( widget ) ):0 );
                int xBase( allocation.x + borderWidth );
                int yBase( allocation.y + borderWidth );
                int wBase( allocation.width - 2*borderWidth );
                int hBase( allocation.height - 2*borderWidth );

                Gtk::Gap gap;
                switch( position )
                {
                    case GTK_POS_BOTTOM:
                    case GTK_POS_TOP:
                    if( !dragInProgress ) gap = Gtk::Gap( x - xBase + 5, w - 6, position );
                    yBase = y;
                    hBase = h;
                    break;

                    case GTK_POS_LEFT:
                    case GTK_POS_RIGHT:
                    if( !dragInProgress ) gap = Gtk::Gap( y - yBase + 5, h - 6, position );
                    xBase = x;
                    wBase = w;
                    break;

                    default: break;

                }

                gap.setHeight( 8 );

                Style::instance().renderTabBarBase( context, xBase-1, yBase-1, wBase+2, hBase+2, position, gap, options, tabOptions );

            }

            if( GTK_IS_NOTEBOOK( widget ) )
            { Gtk::gtk_notebook_update_close_buttons( GTK_NOTEBOOK( widget ) ); }

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_check( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_check -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        if( gtk_theming_engine_has_class( engine,  GTK_STYLE_CLASS_CHECK ) )
        {

            // lookup widget
            const GtkWidgetPath* path( gtk_theming_engine_get_path(engine) );
            const GtkStateFlags state( gtk_theming_engine_get_state(engine) );
            GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

            // style options
            StyleOptions options( widget, state );

            // this ensures that hover keeps precedence of focus for pressed down buttons
            if( options & Active ) options |= Hover;

            // animation data
            AnimationData data;

            // check widget type
            if( gtk_widget_path_is_type( path, GTK_TYPE_TREE_VIEW ) )
            {

                // TreeView checkboxes
                options &= ~(Focus|Hover);
                GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                const Gtk::CellInfo cellInfo( treeView, x, y, w, h );
                if( cellInfo.isValid() &&
                    Style::instance().animations().treeViewEngine().contains( widget ) &&
                    Style::instance().animations().treeViewEngine().isCellHovered( widget, cellInfo, false ) )
                { options |= Hover; }

                // disable active flag, which is not set properly for listviews
                options &= ~Active;

                // retrieve animation state
                data = Style::instance().animations().treeViewStateEngine().get( widget, cellInfo, options );

            } else if( gtk_widget_path_is_type( path, GTK_TYPE_CHECK_MENU_ITEM ) ) {

                // menu checkboxes
                options &= ~(Focus|Hover);
                options |= (Blend|Flat|NoFill );

            } else {

                // normal checkboxes
                // retrieve animation state
                options |= Blend;
                data = Style::instance().animations().widgetStateEngine().get( widget, options );

            }

            // shadow type defines checkmark presence and type
            GtkShadowType shadow( GTK_SHADOW_OUT );
            if( state&GTK_STATE_FLAG_INCONSISTENT ) shadow = GTK_SHADOW_ETCHED_IN;
            else if( state&GTK_STATE_FLAG_ACTIVE ) shadow = GTK_SHADOW_IN;

            Style::instance().renderCheckBox( widget, context, x, y, w, h, shadow, options, data );

        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_check - Calling parentClass()->render_check()\n";
            #endif
            ThemingEngine::parentClass()->render_check( engine, context, x, y, w, h );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_option( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_option -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        if( gtk_theming_engine_has_class( engine,  GTK_STYLE_CLASS_RADIO ) )
        {

            // lookup widget
            const GtkWidgetPath* path( gtk_theming_engine_get_path(engine) );
            const GtkStateFlags state( gtk_theming_engine_get_state(engine) );
            GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

            // style options
            StyleOptions options( widget, state );

            // this ensures that hover keeps precedence of focus for pressed down buttons
            if( options & Active ) options |= Hover;

            // animation data
            AnimationData data;

            // check widget type
            if( gtk_widget_path_is_type( path, GTK_TYPE_TREE_VIEW ) )
            {
                options &= ~(Focus|Hover);
                GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                Gtk::CellInfo cellInfo( treeView, x, y, w, h );
                if( cellInfo.isValid() &&
                    Style::instance().animations().treeViewEngine().contains( widget ) &&
                    Style::instance().animations().treeViewEngine().isCellHovered( widget, cellInfo, false ) )
                { options |= Hover; }

                // also add vertical offset
                x-=1;
                y-=1;

                // disable active flag, which is not set properly for listviews
                options &= ~Active;

                data = Style::instance().animations().treeViewStateEngine().get( widget, cellInfo, options );

            } else if( gtk_widget_path_is_type( path, GTK_TYPE_CHECK_MENU_ITEM ) ) {

                // menu checkboxes
                options &= ~(Focus|Hover);
                options |= Blend;

                // also add vertical offset
                x-=1;
                y-=1;

            } else {

                options |= Blend;
                data=Style::instance().animations().widgetStateEngine().get( widget, options );

            }

            // shadow type defines checkmark presence and type
            GtkShadowType shadow( GTK_SHADOW_OUT );
            if( state&GTK_STATE_FLAG_INCONSISTENT ) shadow = GTK_SHADOW_ETCHED_IN;
            else if( state&GTK_STATE_FLAG_ACTIVE ) shadow = GTK_SHADOW_IN;
            Style::instance().renderRadioButton( widget, context, x, y, w, h, shadow, options, data );

        } else {

            // parent
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_option - Calling parentClass()->render_option()\n";
            #endif
            ThemingEngine::parentClass()->render_option( engine, context, x, y, w, h );

        }

        return;

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        /* define rectangle dimensions */
        gint w( size );
        gint h( size );

        // lookup widget, path and state flags
        const GtkWidgetPath* path( gtk_theming_engine_get_path(engine) );
        const GtkStateFlags state( gtk_theming_engine_get_state(engine) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        // get arrow type
        /* TODO: is it robust */
        GtkArrowType arrow( Gtk::gtk_arrow_get_type( angle ) );

        // get arrow size (disregard the value passed as argument)
        QtSettings::ArrowSize arrowSize( QtSettings::ArrowNormal );

        // define default color role
        Palette::Role role( Palette::ButtonText );

        // define options
        StyleOptions options( widget, state );
        options |= Contrast;

        // Arrows which are active are painted as hovered
        if( state&GTK_STATE_FLAG_ACTIVE ) options |= Hover;

        // animation data
        AnimationData data;

        // if true, widgetStateEngine is used to decide on animation state
        // use either custom engine, or disable animation, otherwise
        bool useWidgetStateEngine( true );

        GtkWidget* parent( 0L );
        if( gtk_widget_path_is_type( path, GTK_TYPE_TEAROFF_MENU_ITEM ) )
        {
            if( widget &&
                gtk_widget_get_state( widget ) != GTK_STATE_PRELIGHT &&
                GTK_IS_MENU( gtk_widget_get_parent( widget ) ) &&
                gtk_menu_get_tearoff_state( GTK_MENU( gtk_widget_get_parent( widget ) ) ) )
            {
                GdkWindow* window( gtk_widget_get_window( widget ) );
                Style::instance().renderWindowBackground( context, window, widget, x-8, y-8, w+16, h+16);
            }

            // disable highlight in menus, for consistancy with oxygen qt style
            options &= ~( Focus|Hover );

            useWidgetStateEngine = false;

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_MENU_ITEM ) ) {

            /* note: can't use gtk_theming_engine_has_class here, cause MENUITEM is not passed */
            // disable highlight in menus, for consistancy with oxygen qt style
            options &= ~( Focus|Hover );
            role = Palette::WindowText;
            useWidgetStateEngine = false;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SPINBUTTON ) ) {

            // use dedicated engine to get animation state
            data = Style::instance().animations().arrowStateEngine().get( widget, arrow, options );
            useWidgetStateEngine = false;

            if( Gtk::gtk_theming_engine_layout_is_reversed( engine ) ) x+=3;
            else x-=1;

            // disable contrast
            options &= ~Contrast;

            role = Palette::Text;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_NOTEBOOK ) ) {

            // use dedicated engine to get animation state
            data = Style::instance().animations().arrowStateEngine().get( widget, arrow, options );
            useWidgetStateEngine = false;

            const int offset = 6;
            switch( gtk_notebook_get_tab_pos( GTK_NOTEBOOK( widget ) ) )
            {
                default:
                case GTK_POS_TOP: h += offset; break;
                case GTK_POS_LEFT: w += offset; break;
                case GTK_POS_BOTTOM: y-=offset; h+=offset; break;
                case GTK_POS_RIGHT: x -= offset; w += offset; break;
            }

            role = Palette::WindowText;

        } else if( ( parent = Gtk::gtk_parent_combobox( widget ) ) ) {

            if( gtk_combo_box_get_has_entry( GTK_COMBO_BOX( parent ) ) )
            {

                if( !( state&GTK_STATE_FLAG_INSENSITIVE ) ) options &= ~Contrast;
                role = Palette::Text;

            } else {

                useWidgetStateEngine = false;
                options &= ~( Focus|Hover );
                role = Palette::ButtonText;

            }

            if( Gtk::gtk_theming_engine_layout_is_reversed( engine ) ) x+=4;
            else x -= 2;

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCROLLBAR ) ) {

            x+= 1;

            // use dedicated engine to get animation state
            useWidgetStateEngine = false;
            data = Style::instance().animations().scrollBarStateEngine().get( widget, Gtk::gdk_rectangle( x, y, w, h ), arrow, options );
            role = Palette::WindowText;

        } else if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_BUTTON ) ) {


            if( !Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TREE_VIEW ) )
            {
                /* note: can't use gtk_theming_engine_has_class above, cause BUTTON is not passed */
                useWidgetStateEngine = false;
                options &= ~( Focus|Hover );

                if( gtk_widget_path_is_type( path, GTK_TYPE_ARROW ) )
                {

                    /* TODO: fixed margins for arrow buttons */
                    x += 1;
                    role = Palette::WindowText;

                }

            } else if( ( ( (parent = Gtk::gtk_parent_button( widget )) && Gtk::gtk_button_is_header( parent ) )
                || gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_HEADER )
                || gtk_theming_engine_has_region( engine, GTK_STYLE_REGION_COLUMN_HEADER, 0L ) ) &&
                Style::instance().settings().viewInvertSortIndicator() )
            { arrow = (arrow == GTK_ARROW_UP ) ? GTK_ARROW_DOWN:GTK_ARROW_UP; }

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_CALENDAR ) ) {

            // FIXME: needs dedicated engine to
            // handle smooth animations
            useWidgetStateEngine = false;

            if( !Gtk::gtk_widget_is_applet( widget ) )
            {

                // need to render background behind arrows from calendar
                // offsets are empirical
                GdkWindow* window( gtk_widget_get_window( widget ) );
                Style::instance().renderWindowBackground( context, window, widget, x-2, y-3, w+4, h+6 );
                role = Palette::WindowText;

            }

        }

        // render arrow
        if( useWidgetStateEngine ) data = Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover );
        Style::instance().renderArrow( context, arrow, x, y, w, h, arrowSize, options, data, role );
        return;

    }

    //________________________________________________________________________________________________
    void render_expander( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_expander -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // lookup
        const GtkWidgetPath* path( gtk_theming_engine_get_path(engine) );
        const GtkStateFlags state( gtk_theming_engine_get_state(engine) );
        const GtkExpanderStyle expander_style( (state&GTK_STATE_FLAG_ACTIVE) ? GTK_EXPANDER_EXPANDED:GTK_EXPANDER_COLLAPSED );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        StyleOptions options( widget, state );
        const bool isTreeView( gtk_widget_path_is_type( path, GTK_TYPE_TREE_VIEW ) );
        const Palette::Role role( isTreeView ? Palette::Text : Palette::WindowText );

        //

        /*
        TODO: For TreeViews, use dedicated engine to handle animations.
        It should use Widget and CellInfo for tagging, and work like
        TabWidgetState engine.
        */
        AnimationData data;
        if( isTreeView )
        {
            GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
            const Gtk::CellInfo cellInfo( treeView, x, y, w, h );
            data = Style::instance().animations().treeViewStateEngine().get( widget, cellInfo, options );
        }

        if( Style::instance().settings().viewDrawTriangularExpander() )
        {

            GtkArrowType arrow;
            if( expander_style == GTK_EXPANDER_EXPANDED ) arrow = GTK_ARROW_DOWN;
            else if( Gtk::gtk_theming_engine_layout_is_reversed( engine ) ) arrow = GTK_ARROW_LEFT;
            else arrow = GTK_ARROW_RIGHT;

            if( isTreeView )
            {

                const QtSettings::ArrowSize arrowSize = Style::instance().settings().viewTriangularExpanderSize();
                Style::instance().renderArrow( context, arrow, x+1, y, w, h, arrowSize, options, data, role );

            } else {

                options |= Contrast;
                const QtSettings::ArrowSize arrowSize = QtSettings::ArrowNormal;
                const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover ) );
                Style::instance().renderArrow( context, arrow, x, y-2, w, h, arrowSize, options, data, role );

            }

        } else if( isTreeView ) {

            Style::instance().renderTreeExpander( context, x+2, y+1, w, h, expander_style, options, data, role );

        } else {

            const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover ) );
            Style::instance().renderTreeExpander( context, x, y-2, w, h, expander_style, options, data, role );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_focus( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_focus -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // no focus whatsoever with oxygen. It is handled elsewhere
        return;

    }

    //________________________________________________________________________________________________
    void render_layout_internal(
        GtkThemingEngine* engine, cairo_t* context,
        gdouble x, gdouble y, PangoLayout *layout )
    {

        const GtkStateFlags state( gtk_theming_engine_get_state(engine) );
        if( state&GTK_STATE_FLAG_INSENSITIVE )
        {

            // for inactive text, we do the painting ourselves
            // to prevent 'emboss' inactive text rendering from gtk

            cairo_save( context );
            if( const PangoMatrix* matrix = pango_context_get_matrix( pango_layout_get_context( layout ) ) )
            {

                cairo_matrix_t cairo_matrix;
                cairo_matrix_init(
                    &cairo_matrix,
                    matrix->xx, matrix->yx,
                    matrix->xy, matrix->yy,
                    matrix->x0, matrix->y0 );

                PangoRectangle rect;
                pango_layout_get_extents( layout, 0L, &rect );
                pango_matrix_transform_rectangle( matrix, &rect );
                pango_extents_to_pixels (&rect, 0L );

                cairo_matrix.x0 += x - rect.x;
                cairo_matrix.y0 += y - rect.y;

                cairo_set_matrix( context, &cairo_matrix );

            } else cairo_move_to( context, x, y);

            GdkRGBA fg_color;
            gtk_theming_engine_get_color( engine, state, &fg_color );

            gdk_cairo_set_source_rgba( context, &fg_color );
            pango_cairo_show_layout( context, layout );
            cairo_restore( context );

        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_layout - Calling parentClass()->render_layout()\n";
            #endif
            ThemingEngine::parentClass()->render_layout( engine, context, x, y, layout );

        }

        return;

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // draw progressbar text white if above indicator, black if not
        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_PROGRESSBAR ) )
        {

            cairo_save( context );
            const ColorUtils::Rgba selection( Style::instance().settings().palette().color( Palette::Active, Palette::SelectedText ) );
            cairo_set_source( context, selection );
            cairo_translate(context,x,y);
            pango_cairo_show_layout(context,layout);
            cairo_restore( context );
            return;

        }

        const GtkWidgetPath* path( gtk_theming_engine_get_path(engine) );
        if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_LABEL ) )
        {

            GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );
            if( widget && GTK_IS_NOTEBOOK( gtk_widget_get_parent( widget ) ) )
            {

                // identify gtk notebook labels, and translate context vertically if found
                cairo_save( context );

                switch( gtk_notebook_get_tab_pos( GTK_NOTEBOOK( gtk_widget_get_parent( widget ) ) ) )
                {
                    case GTK_POS_TOP:
                    case GTK_POS_BOTTOM:
                    cairo_translate( context, 0, 1 );
                    break;

                    default: break;
                }

                /*
                TODO: should also réimplement the "insensitive hack" here
                rather than resorting to parent engine rendering
                */
                render_layout_internal( engine, context, x, y, layout );
                cairo_restore( context );
                return;

            }

        }

        // default rendering
        render_layout_internal( engine, context, x, y, layout );
        return;

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // get flags, path and widget
        GtkStateFlags stateFlags( gtk_theming_engine_get_state(engine) );
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCALE ) ) {

            StyleOptions options( widget, stateFlags );
            options |= Blend;
            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) ) options |= Vertical;

            // retrieve animation state and render accordingly
            // TODO: re-introduce blending
            const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options ) );
            Style::instance().renderSliderHandle( context, x, y, w, h, options, data );

        } else if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SCROLLBAR ) || gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_SLIDER ) ) {

            StyleOptions options( widget, stateFlags );
            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) ) options |= Vertical;

            if( GTK_IS_SWITCH( widget ) )
            {
                // for GtkSwitch, need to manually register to hover engine
                Style::instance().animations().hoverEngine().registerWidget( widget, true );
                if( Style::instance().animations().hoverEngine().hovered( widget ) )
                { options |= Hover; }
            }

            // retrieve animation state
            const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover ) );

            Style::instance().renderScrollBarHandle( context, x, y, w, h, options, data );

        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_slider - Calling parentClass()->render_slider()\n";
            #endif
            ThemingEngine::parentClass()->render_slider( engine, context, x, y, w, h, orientation );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_handle( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        const GtkWidgetPath* path(gtk_theming_engine_get_path(engine));
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_handle -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << path
            << std::endl;
        #endif

        // lookup widget and state
        GtkWidget* widget(Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) ));
        GtkStateFlags state(gtk_theming_engine_get_state(engine));

        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_PANE_SEPARATOR) )
        {

            Style::instance().animations().panedEngine().registerWidget( widget );

            StyleOptions options( widget, state );
            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) )  options |= Vertical;

            /*
            need to add allocation position as an offset for the animation 'dirty rect'
            because of how gtkpaned are rendered internally
            */
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            const AnimationData data( Style::instance().animations().widgetStateEngine().get(
                widget,
                Gtk::gdk_rectangle( x + allocation.x, y + allocation.y, w, h ),
                options, AnimationHover ) );
            Style::instance().renderSplitter( context, x, y, w, h, options, data );

        } else if( gtk_widget_path_is_type(path,GTK_TYPE_HANDLE_BOX) ) {

            // render background
            if( !Gtk::gtk_widget_is_applet( widget ) )
            { Style::instance().renderWindowBackground( context, 0L, widget, x, y, w, h ); }

            // handles
            StyleOptions options( widget, state );
            if(h>w) options|=Vertical;
            Style::instance().renderToolBarHandle( context, x, y, w, h, options );

        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_handle - Calling parentClass()->render_handle()\n";
            #endif
            ThemingEngine::parentClass()->render_handle( engine, context, x, y, w, h );

        }

        return;

    }

    //________________________________________________________________________________________________
    void render_activity(  GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_activity-"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // lookup widget and state
        GtkWidget* widget( Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path( engine ) ) );
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );

        if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_PROGRESSBAR ) )
        {
            StyleOptions options( widget, state);
            if( gtk_theming_engine_has_class( engine, GTK_STYLE_CLASS_VERTICAL ) )
            { options |= Vertical; }
            if( GTK_IS_PROGRESS_BAR(widget) )
            {

                y+=1; h-=2;
                x+=1; w-=2;

            } else if( GTK_IS_ENTRY( widget ) ) {

                #if GTK_CHECK_VERSION( 3, 3, 0 )
                y+=1; h-=2;
                x+=3; w-=6;
                #else
                y-=1; h+=2;
                x-=2; w+=4;
                #endif

            }

            Style::instance().renderProgressBarHandle( context, x, y, w, h, options );

        } else {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_activity - Calling parentClass()->render_activity()\n";
            #endif
            ThemingEngine::parentClass()->render_activity( engine, context, x, y, w, h );

        }

        return;

    }

    //___________________________________________________________
    static GdkPixbuf* render_stated_pixbuf( GdkPixbuf* source, GtkStateFlags state, bool useEffect )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_stated_pixbuf -"
            << " state: " << state
            << " useEffect: " << useEffect
            << std::endl;
        #endif

        // first make a copy
        GdkPixbuf* stated( source );
        if( state & GTK_STATE_FLAG_INSENSITIVE )
        {

            stated = Gtk::gdk_pixbuf_set_alpha( source, 0.3 );
            gdk_pixbuf_saturate_and_pixelate( stated, stated, 0.1, false );

        } else if( useEffect && (state&GTK_STATE_FLAG_PRELIGHT) ) {

            stated = gdk_pixbuf_copy( source );
            if(!Gtk::gdk_pixbuf_to_gamma( stated, 0.7 ) )
            {
                // FIXME: correct the value to match KDE
                /*
                in fact KDE allows one to set many different effects on icon
                not sure we want to copy this code all over the place, especially since nobody changes the default settings,
                as far as I know */
                gdk_pixbuf_saturate_and_pixelate( source, stated, 1.2, false );
            }

        }

        return stated;
    }

    //________________________________________________________________________________________________
    GdkPixbuf* render_icon_pixbuf( GtkThemingEngine *engine, const GtkIconSource *source, GtkIconSize size)
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_icon_pixbuf -"
            << " source: " << source
            << " size: " << size
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // copied from gtkthemingengine.c
        GdkPixbuf* base_pixbuf( gtk_icon_source_get_pixbuf( source ) );
        g_return_val_if_fail( base_pixbuf != 0L, 0L );

        GdkScreen *screen( gtk_theming_engine_get_screen( engine ) );
        GtkSettings *settings( gtk_settings_get_for_screen( screen ) );

        int width = 1;
        int height = 1;
        if( size != (GtkIconSize)-1 && !gtk_icon_size_lookup_for_settings( settings, size, &width, &height ) )
        {
            g_warning (G_STRLOC ": invalid icon size '%d'", size);
            return 0L;
        }

        /*
        If the size was wildcarded, and we're allowed to scale, then scale;
        otherwise, leave it alone.
        */
        GdkPixbuf *scaled( 0L);
        if( size != (GtkIconSize)-1 && gtk_icon_source_get_size_wildcarded( source ) )
        {

            scaled = Gtk::gdk_pixbuf_resize( base_pixbuf, width, height );

        } else {

            scaled = static_cast<GdkPixbuf*>( g_object_ref( base_pixbuf ) );

        }

        // retrieve state and path
        GtkStateFlags state(gtk_theming_engine_get_state( engine ) );
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );

        /* If the state was wildcarded, then generate a state. */
        if( !gtk_icon_source_get_state_wildcarded( source ) ) return scaled;
        else {

            // non-flat pushbuttons don't have any icon effect
            /* since we can't access the button directly, we enable effect only for toolbutton widgets */
            const bool useEffect(
                Style::instance().settings().useIconEffect() &&
                Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TOOL_BUTTON ) );

            /* If the state was wildcarded, then generate a state. */
            GdkPixbuf *stated( render_stated_pixbuf( scaled, state, useEffect ) );

            // clean-up
            if( stated != scaled )
            { g_object_unref( scaled ); }

            // return
            return stated;
        }

    }

    //_______________________________________________________________________________________________________________
    void render_icon( GtkThemingEngine* engine, cairo_t *context, GdkPixbuf* pixbuf, gdouble x, gdouble y )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_icon -"
            << " context: " << context
            << " pixbuf: " << pixbuf
            << " position: (" << x << "," << y << ")"
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // get state and path
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        if( gtk_widget_path_is_type( path, GTK_TYPE_SPIN_BUTTON ) )
        {

            // need to apply state effect on pixbuf because it is not done by Gtk
            // nor is render_icon_pixbuf called
            // TODO: see if one can implement some sort of cache.

            /* since we can't access the button directly, we enable effect only for toolbutton widgets */
            const bool useEffect( Style::instance().settings().useIconEffect() );
            GdkPixbuf* stated( render_stated_pixbuf( pixbuf, state, useEffect ) );

            // call parent method with stated pixbuf
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_icon - calling parentClass()->render_icon()\n";
            #endif
            ThemingEngine::parentClass()->render_icon( engine, context, stated, x, y );

            // and cleanup
            if( stated != pixbuf ) g_object_unref( stated );
            return;

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_ENTRY ) ) {

            // call parent method with extra vertical offset due to gtk3 bug
            ThemingEngine::parentClass()->render_icon( engine, context, pixbuf, x, y-2 );
            return;

        } else {

            // call parent method
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::render_icon - calling parentClass()->render_icon()\n";
            #endif
            ThemingEngine::parentClass()->render_icon( engine, context, pixbuf, x, y );
            return;

        }

    }

    //_______________________________________________________________________________________________________________
    void ThemingEngine::instanceInit( OxygenThemingEngine* self )
    {

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::ThemingEngine::instanceInit" << std::endl;
        #endif

        // hooks
        Style::instance().animations().initializeHooks();
        Style::instance().widgetLookup().initializeHooks();
        Style::instance().windowManager().initializeHooks();
        Style::instance().shadowHelper().initializeHooks();

        // initialize argb hooks
        if( Style::instance().settings().argbEnabled() )
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
        theming_engine_class->render_icon = render_icon;
        return;

    }

    //_______________________________________________________________________________________________________________
    void ThemingEngine::registerType( GTypeModule* module )
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
        return;

    }

    //_______________________________________________________________________________________________________________
    GType ThemingEngine::type( void )
    { return _type; }

}
