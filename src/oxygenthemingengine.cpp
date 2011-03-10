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
#include "oxygendbus.h"
#include "oxygengtktypenames.h"
#include "oxygengtkutils.h"
#include "oxygenstyle.h"
#include "config.h"

#include <cmath>

namespace Oxygen
{

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

        if( state & GTK_STATE_FLAG_PRELIGHT ) return Style::instance().tabCloseButton( Focus );
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

            StyleOptions options( Flat );
            options |= Hover;

            Style::instance().renderButtonSlab( context, rect.x, rect.y, rect.width, rect.height, options );

        } else if( engine.isLocked( widget ) && gtk_widget_get_state( engine.widget( widget, AnimationCurrent ) ) != GTK_STATE_ACTIVE ) {

            // TODO: see if offsetting is robust, and can be moved upstream
            GdkRectangle rect( engine.rectangle( widget, AnimationCurrent ) );
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            rect.x -= allocation.x;
            rect.y -= allocation.y;

            StyleOptions options( Flat );
            options |= Hover;

            Style::instance().renderButtonSlab( context, rect.x, rect.y, rect.width, rect.height, options );

        } else if( engine.isAnimated( widget, AnimationPrevious ) && gtk_widget_get_state( engine.widget( widget, AnimationPrevious ) ) != GTK_STATE_ACTIVE ) {

            // TODO: see if offsetting is robust, and can be moved upstream
            GdkRectangle rect( engine.rectangle( widget, AnimationPrevious ) );
            const GdkRectangle allocation( Gtk::gtk_widget_get_allocation( widget ) );
            rect.x -= allocation.x;
            rect.y -= allocation.y;

            const AnimationData data( engine.animationData( widget, AnimationPrevious ) );

            StyleOptions options( Flat );
            options |= Hover;

            Style::instance().renderButtonSlab( context, rect.x, rect.y, rect.width, rect.height, options, data );

        }

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

        if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TOOLBAR ) && !Style::instance().settings().toolBarDrawItemSeparator() )
        {
            // no separators in toolbars, if requested accordingly
            return;

        } else if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_BUTTON ) ) {

            // no separators in buttons
            return;

        } else {

            StyleOptions options;
            if( !Gtk::gtk_widget_path_has_type( path, GTK_TYPE_TREE_VIEW ) )
            {
                options |= Blend;
                if( Gtk::gtk_widget_path_has_type( path, GTK_TYPE_MENU ) )
                { options |= Menu; }
            }

            options |= Blend;

            // get orientation
            // TODO: is there a better way ?
            const bool vertical( abs( y1 - y0 ) > abs( x1 - x0 ) );
            if( vertical ) options |= Vertical;

            Style::instance().drawSeparator( context, x0, y0, x1-x0, y1-y0, options );


        }

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

        // lookup widget
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );

        if( widget && (
                   gtk_widget_path_is_type( path, GTK_TYPE_WINDOW ) ||
                   gtk_widget_path_is_type( path, GTK_TYPE_NOTEBOOK )
                   ) )
        {

            // register to engines
            Style::instance().animations().mainWindowEngine().registerWidget( widget );

            // render background gradient
            GdkWindow* window( gtk_widget_get_window( widget ) );
            Style::instance().renderWindowBackground( context, window, x, y, w, h );

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_BUTTON )||
                   gtk_widget_path_is_type( path, GTK_TYPE_SCROLLBAR ) )
        {

            // no flat background rendered for buttons
            // everything is dealt with in render_frame.

            // don't draw anything on scrollbars because their background is drawn on parent
            return;

        } else {

            ThemingEngine::parentClass()->render_background( engine, context, x, y, w, h );

        }

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

        // load state
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );

        // load border style
        GtkBorderStyle borderStyle;
        gtk_theming_engine_get( engine, state, "border-style", &borderStyle, NULL );

        // get path
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );

        // lookup widget
        GtkWidget* widget( Style::instance().widgetLookup().find( context, path ) );
        GtkWidget* parent( 0L );

        if( gtk_widget_path_is_type( path, GTK_TYPE_SCROLLBAR ) && GTK_IS_SCROLLBAR( widget ) )
        {
            StyleOptions options;
            if( Gtk::gtk_widget_is_vertical( widget ) ) options |= Vertical;

            Style::instance().adjustScrollBarHole( x, y, w, h, options );

            if( w>0 && h>0 )
            {
                if( options&Vertical ) Style::instance().renderScrollBarHole( context, x, y+1, w, h-1, options );
                else  Style::instance().renderScrollBarHole( context, x+1, y, w-2, h, options );
            }

            return;
        }

        // adjust shadow type for some known widgets
        if( gtk_widget_path_is_type( path, GTK_TYPE_SCROLLED_WINDOW ) &&
            GTK_IS_SCROLLED_WINDOW( widget ) &&
            borderStyle !=  GTK_BORDER_STYLE_INSET &&
            Gtk::gtk_scrolled_window_force_sunken( widget ) )
        {

            // make sure that scrolled windows containing a treeView have sunken frame
            borderStyle = GTK_BORDER_STYLE_INSET;
            gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( widget ), GTK_SHADOW_IN );

        } else if(
            gtk_widget_path_is_type( path, GTK_TYPE_FRAME ) &&
            GTK_IS_FRAME( widget ) &&
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

        if(
            gtk_widget_path_is_type( path, GTK_TYPE_MENU_BAR ) ||
            gtk_widget_path_is_type( path, GTK_TYPE_TOOLBAR ) )
        {

            // for menubars and toolbars, render background gradient
            Style::instance().renderWindowBackground( context, 0L, widget, x, y, w, h );

            /* TODO: fix issues with positionning, and re-enable */
            render_animated_button( context, widget );

        } else if( widget && gtk_widget_path_is_type( path, GTK_TYPE_BUTTON ) ) {

            // pathbar buttons
            if( Gtk::gtk_button_is_in_path_bar(widget) )
            {

                // https://bugzilla.gnome.org/show_bug.cgi?id=635511
                std::string name(G_OBJECT_TYPE_NAME( gtk_widget_get_parent( widget ) ) );
                Style::instance().animations().hoverEngine().registerWidget( widget );

                // only two style options possible: hover or don't draw
                StyleOptions options;
                const bool reversed( Gtk::gtk_widget_layout_is_reversed( widget ) );
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

            }

            // treeview headers
            if( ( parent = Gtk::gtk_parent_tree_view( widget ) ) ) {

                // register to scrolled window engine if any
                if(
                    GTK_IS_SCROLLED_WINDOW( parent = gtk_widget_get_parent( parent ) ) &&
                    Style::instance().animations().scrolledWindowEngine().contains( parent )
                    )
                { Style::instance().animations().scrolledWindowEngine().registerChild( parent, widget ); }

                // treevew header
                GdkWindow* window( gtk_widget_get_window( widget ) );
                Style::instance().renderHeaderBackground( context, window, x, y, w, h );
                return;

            }

            // combobox entry buttons
            if( ( parent = Gtk::gtk_parent_combobox_entry( widget ) ) ) {

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
                options |= NoFill;
                options |= Blend;

                // focus handling
                Style::instance().animations().comboBoxEntryEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEntryEngine().setButton( parent, widget );

                // FIXME: reimplement for Gtk
                // ColorUtils::Rgba background( Gtk::gdk_get_color( style->bg[state] ) );
                // Style::instance().fill( context, x, y, w, h, background );
                Style::instance().fill( context, x, y, w, h, Style::instance().settings().palette().color( Palette::Base ) );

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
                GdkWindow* window( gtk_widget_get_window( widget ) );
                TileSet::Tiles tiles( TileSet::Ring);
                const AnimationData data( Style::instance().animations().widgetStateEngine().get( parent, options, AnimationHover|AnimationFocus, AnimationFocus ) );
                if( Gtk::gtk_widget_layout_is_reversed( widget ) )
                {

                    // hide right and adjust width
                    tiles &= ~TileSet::Right;
                    Style::instance().renderHoleBackground( context, window, x-1, y, w+6, h, tiles );

                    x += Style::Entry_SideMargin;
                    w -= Style::Entry_SideMargin;
                    Style::instance().renderHole( context, x-1, y, w+6, h, options, data, tiles  );

                } else {

                    // hide left and adjust width
                    tiles &= ~TileSet::Left;
                    Style::instance().renderHoleBackground( context, window, x-5, y, w+6, h, tiles );

                    w -= Style::Entry_SideMargin;
                    Style::instance().renderHole( context, x-5, y, w+6, h, options, data, tiles  );

                }

                return;

            }

            // combobox buttons
            if( ( parent = Gtk::gtk_parent_combobox( widget ) ) && Gtk::gtk_combobox_appears_as_list( parent ) )
            {

                // combobox buttons
                const bool reversed( Gtk::gtk_widget_layout_is_reversed( widget ) );

                // StyleOptions options( widget, state, shadow );
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
                        Style::instance().renderButtonSlab( context, x, y, w+7, h, options, data, tiles );

                    } else {

                        tiles &= ~TileSet::Left;
                        Style::instance().renderButtonSlab( context, x-7, y, w+7, h, options, data, tiles );

                    }

                    return;

                } else {

                    options |= Flat;
                    if( Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;
                    if( reversed ) Style::instance().renderButtonSlab( context, x+1, y, w, h, options );
                    else Style::instance().renderButtonSlab( context, x-1, y, w, h, options );
                    return;

                }

            }

            // notebook close buttons
            if( Gtk::gtk_notebook_is_close_button(widget))
            {

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

            }

            #if GTK_CHECK_VERSION(2, 20, 0)
            // tool itemgroup buttons
            if( GTK_IS_TOOL_ITEM_GROUP( widget ) ) return;
            #endif

            StyleOptions options( Blend );
            options |= StyleOptions( widget, state );

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
                ToolBarStateEngine& engine( Style::instance().animations().toolBarStateEngine() );
                if( GtkWidget* parent = engine.findParent( widget ) )
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

        } else if( borderStyle == GTK_BORDER_STYLE_INSET && !Gtk::gtk_widget_path_has_type( path, GTK_TYPE_STATUSBAR ) ) {

            if( widget && GTK_IS_FRAME( widget ) )
            {

                /*
                check for scrolled windows embedded in frames, that contain a treeview.
                if found, change the shadowtypes for consistency with normal -sunken- scrolled windows.
                this should improve rendering of most mandriva drake tools
                */
                GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
                if(
                    GTK_IS_SCROLLED_WINDOW( child ) &&
                    GTK_IS_TREE_VIEW( gtk_bin_get_child( GTK_BIN( child ) ) ) )
                {
                    gtk_frame_set_shadow_type( GTK_FRAME( widget ), GTK_SHADOW_NONE );

                    // also change scrolled window shadow if needed
                    GtkScrolledWindow* scrolledWindow(GTK_SCROLLED_WINDOW( child ) );
                    if( gtk_scrolled_window_get_shadow_type( scrolledWindow ) != GTK_SHADOW_IN )
                    { gtk_scrolled_window_set_shadow_type( scrolledWindow, GTK_SHADOW_IN ); }

                    return;
                }

            }

            // default shadow_in frame
            // hole background is needed for some special cases
            if( GTK_IS_CALENDAR( widget ) )
            {

                GdkWindow* window( gtk_widget_get_window( parent ) );
                Style::instance().renderHoleBackground(
                    context, window,
                    x-1-Style::Entry_SideMargin, y-1, w+2+2*Style::Entry_SideMargin, h+2 );
            }

            // hole
            Style::instance().renderHole( context, x-1, y-1, w+2, h+1, NoFill );

        } else if( borderStyle == GTK_BORDER_STYLE_SOLID && !Gtk::gtk_widget_path_has_type( path, GTK_TYPE_BUTTON ) ) {

            // default etched frame
            Style::instance().renderDockFrame( context, x, y+1, w, h-2, Blend );

        } else if( borderStyle == GTK_BORDER_STYLE_OUTSET ) {

            // default shadow_out frame
            StyleOptions options( Blend );
            options |= NoFill;
            Style::instance().renderSlab( context, x-1, y-1, w+2, h+2, options );

        }

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

        // load state
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );

        // load border style
        GtkBorderStyle borderStyle;
        gtk_theming_engine_get( engine, state, "border-style", &borderStyle, NULL );

        // get path
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );

        // lookup widget
        GtkWidget* widget( Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) ) );

        if( gtk_widget_path_is_type( path, GTK_TYPE_NOTEBOOK ) )
        {

            // this might move to drawShadowGap
            StyleOptions options( NoFill );
            options |= StyleOptions( widget, state );
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

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_FRAME ) ) {

            const Gtk::Gap gap( std::min( xy0_gap, xy1_gap ), std::abs(xy1_gap-xy0_gap), position );

            if( borderStyle == GTK_BORDER_STYLE_INSET )
            {

                Style::instance().renderHoleBackground( context, 0L, widget, x - 1 - Style::Entry_SideMargin, y-1, w + 2 + 2*Style::Entry_SideMargin, h+2 );
                Style::instance().renderHole( context, x-1, y-1, w+2, h+1, gap, NoFill );

            } else if( borderStyle == GTK_BORDER_STYLE_OUTSET ) {

                Style::instance().renderSlab( context, x-1, y-4, w+2, h+4, gap, NoFill );

            } else if( borderStyle == GTK_BORDER_STYLE_SOLID ) {

                Style::instance().renderDockFrame( context, x, y-1, w, h+1, gap, Blend );

            }


        } else {

            ThemingEngine::parentClass()->render_frame_gap( engine, context, x, y, w, h, position, xy0_gap, xy1_gap );

        }

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

        // load state
        GtkStateFlags state( gtk_theming_engine_get_state( engine ) );

        // get path
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );

        // lookup widget
        GtkWidget* widget( Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) ) );
        if( gtk_widget_path_is_type( path, GTK_TYPE_NOTEBOOK ) )
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
                    drawTabBarBase = ((tabOptions & FirstTab) && !isCurrentTab) ||
                        ((tabOptions & LastTab) && gtk_notebook_get_current_page( notebook ) == 0 );
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

            Gtk::gtk_notebook_update_close_buttons( GTK_NOTEBOOK( widget ) );
        }

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

        // lookup widget
        GtkWidget* widget( Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) ) );
//        const GtkWidgetPath* path( gtk_theming_engine_get_path(engine) );
        GtkStateFlags stateFlags( gtk_theming_engine_get_state(engine) );

        // TODO: implement correct logic for checkboxes in cells

//        if( d.isCheckButton() || d.isCellCheck() )
        if(true)
        {

            StyleOptions options( widget, stateFlags );
//            if( !(d.isCellCheck() || Gtk::gtk_parent_tree_view( widget ) ) )
            {
                // enable blending
                options |= Blend;
            }

            AnimationData data;
//            if( d.isCellCheck() )
            if(false)
            {

                options &= ~(Focus|Hover);
                if( GTK_IS_TREE_VIEW( widget ) )
                {
                    GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                    const Gtk::CellInfo cellInfo( treeView, x, y, w, h );
                    if( cellInfo.isValid() &&
                        Style::instance().animations().treeViewEngine().contains( widget ) &&
                        Style::instance().animations().treeViewEngine().isCellHovered( widget, cellInfo, false ) )
                    { options |= Hover; }

                    // retrieve animation state
                    data = Style::instance().animations().treeViewStateEngine().get( widget, cellInfo, options );

                }

            } else {

                // retrieve animation state
                data = Style::instance().animations().widgetStateEngine().get( widget, options );

            }

            // shadow type defines checkmark presence and type
            GtkShadowType shadow( (stateFlags&GTK_STATE_FLAG_ACTIVE) ? GTK_SHADOW_IN : GTK_SHADOW_OUT );
            if(stateFlags&GTK_STATE_FLAG_INCONSISTENT)
                shadow=GTK_SHADOW_ETCHED_IN;

            Style::instance().renderCheckBox( context, x, y, w, h, shadow, options, data );

        } /*else if( d.isCheck() && GTK_IS_CHECK_MENU_ITEM( widget ) ) {

            StyleOptions options( widget, state, shadow );
            options |= (Blend|Flat|NoFill );
            Style::instance().renderCheckBox( context, x, y, w, h, shadow, options );

        }*/ else {
            ThemingEngine::parentClass()->render_check( engine, context, x, y, w, h );
        }
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

        // lookup
        Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) );

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // lookup
        Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) );

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // lookup
        Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) );

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // no focus whatsoever with oxygen. It is handled elsewhere
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

        // lookup
        Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) );

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // get path
        const GtkWidgetPath* path( gtk_theming_engine_get_path( engine ) );
        GtkStateFlags stateFlags( gtk_theming_engine_get_state(engine) );

        // lookup widget
        GtkWidget* widget( Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) ) );

        if( gtk_widget_path_is_type( path, GTK_TYPE_SCROLLBAR ) )
        {

            StyleOptions options(widget,stateFlags);
            if( Gtk::gtk_widget_is_vertical( widget ) ) options |= Vertical;

            const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover ) );
            Style::instance().renderScrollBarHandle( context, x, y, w, h, options, data );

        } else if( gtk_widget_path_is_type( path, GTK_TYPE_SCALE ) ) {

            StyleOptions options( Blend );
            options |= StyleOptions( widget, stateFlags );
            options &= ~Sunken;
            if( Gtk::gtk_widget_is_vertical( widget ) ) options |= Vertical;

            // retrieve animation state and render accordingly
            // TODO: re-introduce blending
            const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options ) );
            Style::instance().renderSliderHandle( context, x, y, w, h, options, data );

        } else {

            ThemingEngine::parentClass()->render_slider( engine, context, x, y, w, h, orientation );

        }

    }

    //________________________________________________________________________________________________
    void render_handle( GtkThemingEngine* engine, cairo_t* context, gdouble x, gdouble y, gdouble w, gdouble h )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_handle -"
            << " context: " << context
            << " rect: " << Gtk::gdk_rectangle( x, y, w, h )
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // lookup
        Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) );

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
            << " path: " << gtk_theming_engine_get_path(engine)
            << std::endl;
        #endif

        // lookup
        Style::instance().widgetLookup().find( context, gtk_theming_engine_get_path(engine) );

        ThemingEngine::parentClass()->render_activity( engine, context, x, y, w, h );

    }

    //________________________________________________________________________________________________
    GdkPixbuf* render_icon_pixbuf( GtkThemingEngine *engine, const GtkIconSource *source, GtkIconSize size)
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::render_handle -"
            << " source: " << source
            << " size: " << size
            << " path: " << gtk_theming_engine_get_path(engine)
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
        Style::instance().widgetLookup().initializeHooks();
        Style::instance().windowManager().initializeHooks();

        // also initialize dbus
        Oxygen::DBus::instance();

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
