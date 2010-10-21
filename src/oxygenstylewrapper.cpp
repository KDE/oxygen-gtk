/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
*
* Icon rendering code from Walmis
* <http://gnome-look.org/content/show.php?content=77783&forumpage=3>
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

#include "oxygenstylewrapper.h"

#include "config.h"

#include "oxygen.h"
#include "oxygenanimations.h"
#include "oxygencairoutils.h"
#include "oxygengtkdetails.h"
#include "oxygengtkutils.h"
#include "oxygenmaps.h"
#include "oxygenrcstyle.h"
#include "oxygenstyle.h"

#include <iostream>

//_______________________________________________________________________________________________________________
struct _OxygenStyle
{ GtkStyle parent; };

//_______________________________________________________________________________________________________________
struct _OxygenStyleClass
{  GtkStyleClass parent; };

//_______________________________________________________________________________________________________________
static GtkStyleClass* oxygen_style_parent_class = 0L;

namespace Oxygen
{

    //___________________________________________________________________________________________________________
    static void draw_flat_box(
        GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const char* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );
        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=flat_box, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        const Gtk::Detail d( detail );
        bool accepted( false );
        if( d.isBase() || d.isEventBox())
        {

            if( GTK_IS_WINDOW( widget ) )
            {

                Animations::instance().mainWindowEngine().registerWidget( widget );

            } else if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( widget ) ) {

                Animations::instance().scrollBarEngine().registerScrolledWindow( parent );

            }

            Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );
            return;

        } else if( d.isViewportBin() ) {

            GdkColor bg=style->bg[state];
            GdkColor defbg=gtk_widget_get_default_style()->bg[state];

            // This dirty hack with 10000 makes GIMP bottom element viewportbin have correct window color
            // while ccsm options list will remain white. Hope this will work as intended for other apps
            // FIXME: why default style bg isn't the same as unchanged(?) bg in GIMP? If it were, no hack would be needed
            if(
                bg.red/10000==defbg.red/10000 &&
                bg.green/10000==defbg.green/10000 &&
                bg.blue/10000==defbg.blue/10000)
            {

                // make sure that widget is registered to scrolledBarEngine,
                // so that background gets updated properly
                if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( widget ) )
                { Animations::instance().scrollBarEngine().registerScrolledWindow( parent ); }

                Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );
                accepted = true;
            }

        } else if( d.isTrough() ) {

            if( GTK_IS_PROGRESS_BAR( widget ) )
            { return; }

        } else if( d.isTooltip() && Style::instance().settings().tooltipDrawStyledFrames() ) {

            StyleOptions options;
            if( Gtk::gtk_widget_has_rgba( widget ) ) options |= Alpha;
            if( !( (options&Alpha) || Style::instance().settings().applicationName().isMozilla() )  )
            {

                // make tooltips appear rounded using XShape extension if screen isn't composited
                Animations::instance().widgetSizeEngine().registerWidget( widget );
                const GtkAllocation& allocation( widget->allocation );
                if( Animations::instance().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) )
                {
                    GdkPixmap* mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_mask( window, mask, 0, 0 );
                    gdk_pixmap_unref( mask );

                }

            }

            Style::instance().renderTooltipBackground( window, clipRect, x, y, w, h, options );
            return;

        } else if(
            d.isCheckButton() ||
            d.isRadioButton() ||
            d.isExpander() ) {

            return;

        } else if( d.isCell() ) {

            StyleOptions options( widget, state );

            // select palete colorgroup for cell background
            Palette::Group group( Palette::Active );
            if( options & Disabled ) group = Palette::Disabled;
            else if( !(options&Focus) ) group = Palette::Inactive;

            // render background
            ColorUtils::Rgba background;
            if( d.isCellEven() ) background = Style::instance().settings().palette().color( group, Palette::Base );
            else if( d.isCellOdd() ) background = Style::instance().settings().palette().color( group, Palette::BaseAlternate );

            if( background.isValid() ) Style::instance().fill( window, clipRect, x, y, w, h, background );

            if( Gtk::gtk_combobox_is_tree_view( widget ) )
            {

                // draw flat selection in combobox list
                if(state==GTK_STATE_SELECTED)
                {
                    ColorUtils::Rgba selection( Style::instance().settings().palette().color( Palette::Active, Palette::Selected ) );
                    Style::instance().fill( window, clipRect, x, y, w, h, selection );
                }

            } else {

                // draw rounded selection in normal list,
                // and detect hover
                if( GTK_IS_TREE_VIEW( widget ) )
                {

                    Animations::instance().treeViewEngine().registerWidget( widget, Style::instance().settings().viewDrawTreeBranchLines() );
                    if(  Animations::instance().treeViewEngine().isCellHovered( widget, x, y, w, h ) )
                    { options |= Hover; }

                }

                if( options & (Selected|Hover) )
                {

                    unsigned int tiles( TileSet::Center );
                    if( d.isCellStart() ) tiles |= TileSet::Left;
                    else if( d.isCellEnd() ) tiles |= TileSet::Right;
                    else if( !d.isCellMiddle() ) tiles = TileSet::Horizontal;

                    Style::instance().renderSelection( window, clipRect, x, y, w, h, tiles, options );

                }

            }


            return;

        } else if( d.isEntryBg() && !Style::instance().settings().applicationName().isMozilla() ) {

            StyleOptions options( NoFill );
            options |= StyleOptions( widget, state, shadow );

            // calculate proper offsets so that the glow/shadow match parent frame
            const int xOffset( style ? style->xthickness + 1 : 3);
            const int yOffset( style ? style->ythickness + 1 : 3);

            // there is no need to render anything if both offsets are larger than 4
            if( xOffset >= 5 && yOffset >= 5 ) return;

            // adjust rect using offsets above
            x -= xOffset;
            y -= yOffset;
            w += 2*xOffset;
            h += 2*yOffset;

            if( GTK_IS_SPIN_BUTTON( widget ) )
            {

                if(
                    Animations::instance().lineEditEngine().contains( widget ) &&
                    Animations::instance().lineEditEngine().hovered( widget ) )
                { options |= Hover; }

                // plain background
                if( style && !Style::instance().settings().applicationName().isMozilla() )
                {
                    ColorUtils::Rgba background( Gtk::gdk_get_color( style->base[gtk_widget_get_state(widget)] ) );
                    Style::instance().fill( window, clipRect, x, y, w, h, background );
                }

                // hole
                Style::instance().renderHole( window, clipRect, x, y, w+5, h, options, TileSet::Ring&( ~TileSet::Right ) );

            } else if( GtkWidget* parent = Gtk::gtk_parent_combobox_entry( widget ) ) {

                // check if parent is in style map
                Animations::instance().comboBoxEntryEngine().registerWidget( parent );
                Animations::instance().comboBoxEntryEngine().setEntry( parent, widget );
                Animations::instance().comboBoxEntryEngine().setEntryFocus( parent, options & Focus );

                if( state != GTK_STATE_INSENSITIVE )
                {
                    if( Animations::instance().comboBoxEntryEngine().hasFocus( parent ) ) options |= Focus;
                    else options &= ~Focus;

                    if(  Animations::instance().comboBoxEntryEngine().hovered( parent ) ) options |= Hover;
                    else options &= ~Hover;
                }

                // since combobox entry is drawn in the combobox full height, we'll have to adjust glow height
                y -= (parent->allocation.height-h)/2;
                h = parent->allocation.height;

                // partial highlight
                Style::instance().renderHole( window, clipRect, x, y, w+7, h, options, TileSet::Ring&( ~TileSet::Right ) );

            } else {

                if(
                    Animations::instance().lineEditEngine().contains( widget ) &&
                    Animations::instance().lineEditEngine().hovered( widget ) )
                { options |= Hover; }

                // compare painting rect to widget rect, to decide if some sides are to be masked
                TileSet::Tiles tiles = TileSet::Ring;
                if( widget && window != widget->window && GDK_IS_WINDOW( window ) && widget->window == gdk_window_get_parent( window )  )
                {

                    const int widgetWindowWidth( widget->allocation.width );
                    int localWindowX( 0 );
                    int localWindowWidth( 0 );
                    gdk_window_get_position( window, &localWindowX, 0L );
                    gdk_window_get_size( window, &localWindowWidth, 0L );

                    // remove left border if needed
                    if( localWindowX > 2 )
                    {
                        tiles &= ~TileSet::Left;
                        x -= 7;
                        w += 7;
                    }

                    // remove right border if needed
                    if( localWindowX + localWindowWidth < widgetWindowWidth - 2 )
                    {
                        tiles &= ~TileSet::Right;
                        w += 7;
                    }

                }

                // render hole
                Style::instance().renderHole( window, clipRect, x, y, w, h, options, tiles );

            }

            accepted = true;

        }

        if( !accepted )
        {

            oxygen_style_parent_class->draw_flat_box( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h );

        }

    }

    //_____________________________________________________________________________________
    GdkPixbuf* processTabCloseButton(GtkWidget* widget, GtkStateType state)
    {
        static GdkPixbuf* pbNormalColored(0L);
        static GdkPixbuf* pbNormalGray(0L);
        static GdkPixbuf* pbPrelight(0L);
        static GdkPixbuf* pbActive(0L);
        GdkPixbuf* toDraw( 0L );
        GError* err( 0L );
        switch (state)
        {
            case GTK_STATE_NORMAL:
            {

                if(!pbNormalColored)
                {
                    std::string buttonIconName = std::string(GTK_THEME_DIR) + "/special-icons/standardbutton-closetab-16.png";
                    pbNormalColored=gdk_pixbuf_new_from_file(buttonIconName.c_str(),&err);
                }

                // check if our button is on active page and if not, make it gray
                GtkNotebook* notebook=GTK_NOTEBOOK(Gtk::gtk_parent_notebook(widget));
                GtkWidget* page=gtk_notebook_get_nth_page(notebook,gtk_notebook_get_current_page(notebook));
                GtkWidget* tabLabel=gtk_notebook_get_tab_label(notebook,page);
                if( !Gtk::gtk_is_parent( widget, tabLabel ) )
                {
                    pbNormalGray = Gtk::gdk_pixbuf_set_alpha(pbNormalColored, 0.5);
                    gdk_pixbuf_saturate_and_pixelate( pbNormalGray, pbNormalGray , 0.1, false );
                    toDraw=pbNormalGray;
                }
                else toDraw = pbNormalColored;
            }

            break;

            case GTK_STATE_ACTIVE:
            {
                if(!pbActive) {

                    std::string buttonIconName = std::string(GTK_THEME_DIR)+ "/special-icons/standardbutton-closetab-down-16.png";
                    pbActive = gdk_pixbuf_new_from_file(buttonIconName.c_str(),&err);

                }

                if(pbActive) toDraw = pbActive;
            }

            break;

            case GTK_STATE_PRELIGHT:
            {
                if(!pbPrelight)
                {
                    std::string buttonIconName = std::string(GTK_THEME_DIR) + "/special-icons/standardbutton-closetab-hover-16.png";
                    pbPrelight = gdk_pixbuf_new_from_file(buttonIconName.c_str(),&err);
                }
                if(pbPrelight) toDraw = pbPrelight;
            }

            break;

            default:
            toDraw = 0L;
            break;

        }

        // TODO: should the error be deallocated ?
        if( err )
        { std::cerr << "Oxygen::processTabCloseButton - " << err->message << std::endl; }

        return toDraw;
    }
    //___________________________________________________________________________________________________________
    static void draw_box( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=box, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        if(GTK_IS_MENU_BAR( widget ) )
        { Animations::instance().menuShellEngine().registerWidget(widget); }

        Style::instance().sanitizeSize( window, w, h );
        const Gtk::Detail d( detail );

        if( d.isButton() || d.isOptionMenu() )
        {

            // check if it's PathBar toggle button
            if( Gtk::gtk_button_is_in_path_bar(widget) )
            {

                Animations::instance().flatButtonEngine().registerWidget( widget );

                // only two style options possible: hover or don't draw
                StyleOptions options(Hover);
                if(state!=GTK_STATE_NORMAL)
                {
                    if(state==GTK_STATE_ACTIVE && !Animations::instance().flatButtonEngine().hovered( widget ) )
                    { return; }

                    Style::instance().renderSelection(window,clipRect,x,y,w,h,TileSet::Full,options);

                }

            } else if( Gtk::gtk_parent_tree_view( widget ) ) {

                Style::instance().renderHeaderBackground( window, clipRect, x, y, w, h );

            } else if( GtkWidget* parent = Gtk::gtk_parent_combobox_entry( widget ) ) {

                /*
                editable combobox button get a hole (with left corner hidden), and a background
                that match the corresponding text entry background.
                */

                StyleOptions options( widget, state, shadow );
                options |= Blend|NoFill;

                // focus handling
                Animations::instance().comboBoxEntryEngine().registerWidget( parent );
                Animations::instance().comboBoxEntryEngine().setButton( parent, widget );
                GtkWidget* entry( Animations::instance().comboBoxEntryEngine().entry( parent ) );

                if( GTK_IS_COMBO( parent ) && entry ) { state = gtk_widget_get_state( entry ); }
                else state = gtk_widget_get_state(parent);

                if( style )
                {
                    ColorUtils::Rgba background( Gtk::gdk_get_color( style->bg[state] ) );
                    Style::instance().fill( window, clipRect, x, y, w, h, background );
                }

                // update option accordingly
                if( state == GTK_STATE_INSENSITIVE ) options &= ~(Hover|Focus);
                else {

                    Animations::instance().comboBoxEntryEngine().setButtonFocus( parent, options & Focus );
                    if( Animations::instance().comboBoxEntryEngine().hasFocus( parent ) ) options |= Focus;
                    else options &= ~Focus;

                    if(  Animations::instance().comboBoxEntryEngine().hovered( parent ) ) options |= Hover;
                    else options &= ~Hover;

                }

                // render
                Style::instance().renderHoleBackground(window,clipRect, x-5, y, w+6, h-1 );
                Style::instance().renderHole( window, clipRect, x-5, y, w+6, h, options, TileSet::Ring&(~TileSet::Left) );

                return;

            } else if( GTK_IS_TOOL_ITEM_GROUP( widget ) ) {

                return;

            } else {

                StyleOptions options( Blend );
                options |= StyleOptions( widget, state, shadow );

                if( widget && Gtk::gtk_button_is_flat( widget ) )
                {
                    options |= Flat;
                    Animations::instance().flatButtonEngine().registerWidget( widget );
                    if( Animations::instance().flatButtonEngine().hovered( widget ) )
                    { options |= Hover; }

                }

                if( Gtk::gtk_notebook_is_close_button(widget)) {

                    if( gtk_button_get_relief(GTK_BUTTON(widget))==GTK_RELIEF_NONE )
                    {

                        gtk_button_set_relief(GTK_BUTTON(widget),GTK_RELIEF_NORMAL);

                    }

                    if( GdkPixbuf* pixbuf = processTabCloseButton(widget,state) )
                    {

                        // hide previous image
                        // show ours instead
                        GtkWidget* image=Gtk::gtk_button_find_image(widget);
                        gtk_widget_hide(image);

                        // center the button image
                        const int height( gdk_pixbuf_get_height( pixbuf ) );
                        const int width( gdk_pixbuf_get_width( pixbuf ) );
                        x=x+(w-width)/2;
                        y=y+(h-height)/2;

                        // render the image
                        Cairo::Context context( window, clipRect );
                        gdk_cairo_set_source_pixbuf( context, pixbuf, x, y);
                        cairo_paint(context);

                    }

                } else {

                    Style::instance().renderButtonSlab( window, clipRect, x, y, w, h, options );

                }

            }

        } else if( d.isMenuBar() || d.isToolBar() ) {

            Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );
            return;

        } else if( d.isMenu() ) {

            if( GTK_IS_MENU( widget ) && GTK_MENU( widget )->torn_off )
            {

                Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );

            } else {

                StyleOptions options;
                if( Gtk::gtk_widget_has_rgba( widget ) ) options |= Alpha;
                if( !( (options&Alpha) || Style::instance().settings().applicationName().isMozilla() ) && GTK_IS_MENU(widget) )
                {

                    // make tooltips appear rounded using XShape extension if screen isn't composited
                    Animations::instance().widgetSizeEngine().registerWidget( widget );
                    const GtkAllocation& allocation( widget->allocation );
                    if( Animations::instance().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) )
                    {
                        GdkPixmap* mask( Style::instance().helper().roundMask( w, h - 2*Style::Menu_VerticalOffset ) );
                        gdk_window_shape_combine_mask( gtk_widget_get_parent_window(widget), mask, 0, Style::Menu_VerticalOffset );
                        gdk_pixmap_unref(mask);
                    }

                }

                Style::instance().renderMenuBackground( window, clipRect, x, y, w, h, options );
                Style::instance().drawFloatFrame( window, clipRect, x, y, w, h, options );

            }

        } else if( d.isMenuScrollArrow() ) {

            return;

        } else if( d.isDefaultButton() || d.isScrollBar() || d.isPaned() || d.isHandleBox()  ) {

            return;

        } else if( d.isMenuItem() ) {

            StyleOptions options = StyleOptions( widget, state, shadow );
            Style::instance().renderMenuItemRect( window, clipRect, widget, x, y, w, h, options );

        } else if( d.isTroughAny() && GTK_IS_VSCALE( widget ) ) {

            // TODO: calculate this value from the style "slider-width" property
            const int offset( 6 );
            if( d.isTrough() ) Style::instance().renderSliderGroove( window, clipRect, x, y + offset, w, h - 2*offset, Vertical );
            else if( d.isTroughLower() ) Style::instance().renderSliderGroove( window, clipRect, x, y + offset, w, h, Vertical );
            else if( d.isTroughUpper() ) Style::instance().renderSliderGroove( window, clipRect, x, y, w, h - offset, Vertical );

        } else if( d.isTroughAny() && GTK_IS_HSCALE( widget ) ) {

            // TODO: calculate this value from the style "slider-width" property
            const int offset( 6 );
            if( d.isTrough() ) Style::instance().renderSliderGroove( window, clipRect, x + offset, y, w - 2*offset, h, StyleOptions() );
            else if( d.isTroughLower() ) Style::instance().renderSliderGroove( window, clipRect, x + offset, y, w, h, StyleOptions() );
            else if( d.isTroughUpper() ) Style::instance().renderSliderGroove( window, clipRect, x, y, w - offset, h, StyleOptions() );

        } else if( d.isTrough() && shadow == GTK_SHADOW_IN ) {

            if( GTK_IS_PROGRESS_BAR( widget ) )
            {

                if( !Style::instance().settings().applicationName().isMozilla() )
                {
                    /*
                    need to call the parent style implementation here,
                    otherwise some uninitialized pixels are present.
                    Not sure why
                    */
                    oxygen_style_parent_class->draw_box( style, window, state, shadow, clipRect, widget, detail, x, y, w, h );
                    Style::instance().renderWindowBackground( window, widget, clipRect, x, y, w, h );
                }

                StyleOptions options;
                if( !Gtk::gtk_progress_bar_is_horizontal( widget ) ) options |= Vertical;
                Style::instance().renderProgressBarHole( window, clipRect, x, y, w, h, options );

            } else if( GTK_IS_VSCROLLBAR( widget ) ) {

                Style::instance().adjustScrollBarHole( x, y, w, h, Vertical );
                Style::instance().renderScrollBarHole( window, clipRect, x, y+1, w-1, h-1, Vertical );

            } else if( GTK_IS_HSCROLLBAR( widget ) ) {

                Style::instance().adjustScrollBarHole( x, y, w, h, StyleOptions() );
                Style::instance().renderScrollBarHole( window, clipRect, x+1, y, w-2, h-1, StyleOptions() );

            }

        } else if( d.isSpinButton()) {

            StyleOptions options( widget, state, shadow );
            options |= Blend|NoFill;

            if( style )
            {
                // get background color from widget state
                ColorUtils::Rgba background( Gtk::gdk_get_color( style->base[gtk_widget_get_state(widget)] ) );

                if( Style::instance().settings().applicationName().isMozilla() )
                {

                    /*
                    for firefox on has to mask out the corners manually,
                    because renderholebackground fails
                    */
                    Cairo::Context context( window, clipRect );
                    cairo_rounded_rectangle( context, x-1, y+2, w-1, h-4, 2, CornersRight );
                    cairo_set_source( context, background );
                    cairo_fill( context );

                } else Style::instance().fill( window, clipRect, x, y, w, h, background );

            }

            if(
                Animations::instance().lineEditEngine().contains( widget ) &&
                Animations::instance().lineEditEngine().hovered( widget ) )
            { options |= Hover; }

            Style::instance().renderHoleBackground(window,clipRect, x-5, y-1, w+6, h+1 );
            Style::instance().renderHole( window, clipRect, x-5, y-1, w+6, h+2, options, TileSet::Ring & (~TileSet::Left) );

        } else if( d.isSpinButtonArrow() ) {

            return;

        } else if( d.isBar() ) {

            StyleOptions options( widget, state, shadow );
            if(GTK_IS_PROGRESS_BAR(widget))
            {

                Style::instance().renderProgressBarHandle( window, clipRect, x, y, w, h, options );

            } else {

                // most likely it's progressbar in the list
                // FIXME: is it always the case ? Should we check on TREE_VIEW, CELL_VIEW, like done with scrollbar hole ?
                Style::instance().renderProgressBarHandle( window, clipRect, x, y+2, w, h-4, options );

            }

            return;

        } else {

            oxygen_style_parent_class->draw_box( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_shadow( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );
        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=shadow, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        GtkWidget* parent( 0L );
        const Gtk::Detail d( detail );

        // check if it's combobox list window
        if( Gtk::gtk_combobox_is_scrolled_window( widget ) && GTK_IS_WINDOW( parent = gtk_widget_get_parent( widget ) ) )
        {

            // now make it look like in Oxygen-Qt - with float frame and round edges
            GtkAllocation allocation(parent->allocation);
            StyleOptions options;
            if( Gtk::gtk_widget_has_rgba(parent) ) options|=Alpha;

            if( !(options&Alpha) ) // the same as with menus and tooltips (but changed a bit to take scrollbars into account)
            {
                // make background window rounded
                Animations::instance().widgetSizeEngine().registerWidget( parent );
                if( Animations::instance().widgetSizeEngine().updateSize( parent, allocation.width, allocation.height ) )
                {
                    GdkPixmap* mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_mask( parent->window, mask, 0, 0 );
                    gdk_pixmap_unref( mask );
                }

            }

            // TODO: when RGBA visual is present, do this via RGBA (maybe in list rendering code) and uncomment the following if statement
            // for now, this if statement is commented out so that we get rounded list anyway
            // if( !(options&Alpha) )
            {
                // and make the list rounded
                GList* children=gtk_container_get_children(GTK_CONTAINER( widget ));
                widget=GTK_WIDGET( children->data );
                Animations::instance().widgetSizeEngine().registerWidget( widget );
                if( Animations::instance().widgetSizeEngine().updateSize( widget, widget->allocation.width, widget->allocation.height) )
                {
                    // offset is needed to make combobox list border 3px wide instead of default 2
                    // additional pixel is for ugly shadow
                    const gint offset( options&Alpha ? 0:1 );
                    GdkPixmap* mask( Style::instance().helper().roundMask(
                        widget->allocation.width - 2*offset,
                        widget->allocation.height - 2*offset - ( options&Alpha ? 1:0 ),
                        3 ) );

                    gdk_window_shape_combine_mask( widget->window, mask, offset, offset );
                    gdk_pixmap_unref( mask );
                }

                if( children ) g_list_free( children );
            }

            // now draw float frame on background window
            allocation.y -= Style::Menu_VerticalOffset;
            allocation.height += 2*Style::Menu_VerticalOffset;
            Style::instance().renderMenuBackground( parent->window, clipRect, allocation.x, allocation.y, allocation.width, allocation.height, options );
            Style::instance().drawFloatFrame( parent->window, clipRect, allocation.x, allocation.y, allocation.width, allocation.height, options );
            return;

        } else if( Gtk::gtk_combobox_is_viewport( widget ) ) {

            return;

        } else if( Gtk::gtk_combobox_is_frame( widget ) ) {

            // make GtkCombo list look a bit better
            // retrieve proper parent and check
            GtkWidget* parent=gtk_widget_get_parent(widget);
            if( parent ) parent=gtk_widget_get_parent(parent);
            if( !( parent && GTK_IS_WINDOW(parent) ) ) return;

            const GtkAllocation& allocation(parent->allocation);
            StyleOptions options;
            if( Gtk::gtk_widget_has_rgba(parent) ) options|=Alpha;

            if( !(options&Alpha) )
            {
                // the same as with menus and tooltips (but changed a bit to take scrollbars into account)
                // make background window rounded
                Animations::instance().widgetSizeEngine().registerWidget(parent);
                if( Animations::instance().widgetSizeEngine().updateSize(parent,allocation.width,allocation.height))
                {
                    GdkPixmap* mask( Style::instance().helper().roundMask( allocation.width,allocation.height ) );
                    gdk_window_shape_combine_mask(parent->window,mask,0,0);
                    gdk_pixmap_unref(mask);
                }
            }
            // now draw float frame on background window
            Style::instance().renderMenuBackground(window,clipRect,x,y,w,h,options);
            Style::instance().drawFloatFrame(window,clipRect,x,y,w,h,options);

            return;

        } else if( d.isSlider() || d.isRuler() ) {

            return;

        } else if( ( d.isEntry() || d.isViewport() || d.isScrolledWindow() ) && shadow == GTK_SHADOW_IN ) {

            StyleOptions options( NoFill );
            options |= StyleOptions( widget, state, shadow );

            if( GtkWidget* parent = Gtk::gtk_parent_combobox_entry( widget ) )
            {

                // check if parent is in style map
                Animations::instance().comboBoxEntryEngine().registerWidget( parent );
                Animations::instance().comboBoxEntryEngine().setEntry( parent, widget );
                Animations::instance().comboBoxEntryEngine().setEntryFocus( parent, options & Focus );
                GtkWidget* entry( Animations::instance().comboBoxEntryEngine().entry( parent ) );
                if( !( GTK_IS_COMBO( parent ) && entry && gtk_widget_get_state( entry ) == GTK_STATE_INSENSITIVE ) )
                {

                    if( Animations::instance().comboBoxEntryEngine().hasFocus( parent ) ) options |= Focus;
                    else options &= ~Focus;

                    if(  Animations::instance().comboBoxEntryEngine().hovered( parent ) ) options |= Hover;
                    else options &= ~Hover;
                }

                // render
                Style::instance().renderHoleBackground( window, clipRect, x-1, y, w+7, h-1 );
                Style::instance().renderHole( window, clipRect, x-1, y, w+7, h, options, TileSet::Ring&(~TileSet::Right) );

            } else if( GTK_IS_SPIN_BUTTON( widget ) ) {

                // register to line edit engine
                Animations::instance().lineEditEngine().registerWidget( widget );
                if( Animations::instance().lineEditEngine().hovered( widget ) )
                { options |= Hover; }

                if( style && !Style::instance().settings().applicationName().isMozilla() )
                {
                    ColorUtils::Rgba background( Gtk::gdk_get_color( style->base[gtk_widget_get_state( widget )] ) );
                    Style::instance().fill( window, clipRect, x, y, w, h, background );
                }

                Style::instance().renderHoleBackground( window, clipRect, x-1, y-1, w+2, h+1 );
                Style::instance().renderHole( window, clipRect, x-1, y-1, w+5, h+2, options, TileSet::Ring & (~TileSet::Right) );

            } else {

                // register to line edit engine
                Animations::instance().lineEditEngine().registerWidget( widget );
                if( Animations::instance().lineEditEngine().hovered( widget ) )
                { options |= Hover; }

                Style::instance().renderHoleBackground( window, clipRect, x-1, y-1, w+2, h+1 );
                Style::instance().renderHole( window, clipRect, x-1, y-1, w+2, h+2, options );

            }

            return;

        } else if( ( d.isTrough() || d.isBar() ) && GTK_IS_PROGRESS_BAR( widget ) ) {

            return;

        } else if (GTK_IS_NOTEBOOK(widget)) {

            Style::instance().renderWindowBackground(window,clipRect,x-4,y-4,w+8,h+8);
            Style::instance().renderSlab(window,clipRect,x-1,y-1,w+2,h+2,NoFill);

        } else if( GTK_IS_CALENDAR( widget ) && shadow == GTK_SHADOW_OUT ) {

            // calendar header
            if( style )
            {
                Style::instance().fill( window,clipRect,x-2,y-2,w+4,h+4, Gtk::gdk_get_color( style->base[state] ) );
                Style::instance().renderWindowBackground( window,clipRect,x+2,y+2,w-4,h-6 );
            }

            StyleOptions options( Blend );
            options |= NoFill;
            Style::instance().renderSlab(window,clipRect,x-2,y-2,w+4,h+2, options );

        } else if( d.isNull() && (GTK_IS_TREE_VIEW(widget) || GTK_IS_CELL_VIEW(widget)) && shadow==GTK_SHADOW_IN ) {

            // it's likely progressbar hole
            // FIXME: is it enough to check for TreeView? is shadow_in the only possible case?
            StyleOptions options;
            Style::instance().renderProgressBarHole(window,clipRect,x-2,y,w+4,h,options);

        } else if( shadow == GTK_SHADOW_IN && !Gtk::gtk_parent_statusbar( widget ) ) {

            // default shadow_in frame
            Style::instance().renderHoleBackground( window, clipRect, x-1, y-1, w+2, h+1 );
            Style::instance().renderHole( window, clipRect, x-1, y-1, w+2, h+1, NoFill );

        } else if( (shadow == GTK_SHADOW_ETCHED_IN || shadow == GTK_SHADOW_ETCHED_OUT) && !Gtk::gtk_parent_button( widget )) {

            // default etched frame
            Style::instance().renderDockFrame( window, clipRect, x, y+1, w, h-2, Blend );

        } else if( shadow == GTK_SHADOW_OUT ) {

            // default shadow_out frame
            Style::instance().renderSlab(window,clipRect,x-1,y-1,w+2,h+2,Blend);

        }

        return;
    }

    //___________________________________________________________________________________________________________
    static void draw_check( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=check, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        const Gtk::Detail d( detail );
        if( d.isCheckButton() || d.isCellCheck() )
        {

            StyleOptions options( widget, state, shadow );
            if( !(d.isCellCheck() || Gtk::gtk_parent_tree_view( widget ) ) )
            {
                // enable blending
                options |= Blend;
            }

            if( d.isCellCheck() )
            {
                options &= ~Focus;
                if(
                    Animations::instance().treeViewEngine().contains( widget ) &&
                    Animations::instance().treeViewEngine().isCellHovered( widget, x, y, w, h, false ) )
                {
                    options |= Hover;
                } else options &= ~Hover;

            }

            Style::instance().renderCheckBox( window, clipRect, x, y, w, h, shadow, options );

        } else if( d.isCheck() && GTK_IS_CHECK_MENU_ITEM( widget ) ) {

            StyleOptions options( widget, state, shadow );
            options |= (Blend|Flat|NoFill );
            Style::instance().renderCheckBox( window, clipRect, x, y, w, h, shadow, options );

        } else {

            oxygen_style_parent_class->draw_check( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_option( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=option, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isRadioButton() )
        {

            StyleOptions options( widget, state, shadow );
            if( !Gtk::gtk_parent_tree_view( widget ) ) options |= Blend;
            Style::instance().renderRadioButton( window, clipRect, x, y, w, h, shadow, options );

        } else if( d.isOption() ) {

            // load options and disable hove
            StyleOptions options( widget, state, shadow );
            if( !Gtk::gtk_parent_tree_view( widget ) )
            {
                options |= Blend;
                if( Gtk::gtk_parent_menu( widget ) ) options|=Menu;
            }
            options &= ~Hover;
            Style::instance().renderRadioButton( window, clipRect, x, y, w, h, shadow, options );

        } else {

            oxygen_style_parent_class->draw_option( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h );

        }
    }

    //___________________________________________________________________________________________________________
    static void draw_hline(
        GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x1,
        gint x2,
        gint y )
    {
        g_return_if_fail( style && window );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=hline, state=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isVScale() )
        {

            return;

        } else if( d.isToolBar() && !Style::instance().settings().toolBarDrawItemSeparator() ) {

            return;

        } else if( d.isTearOffMenuItem() ) {

            if( widget && gtk_widget_get_state( widget ) != GTK_STATE_PRELIGHT )
            {
                // render background, this is needed to prevent a plain rect to be rendered (by gtk) where the item is
                // rectangle is adjusted manually so that it matches
                if( widget && GTK_IS_MENU( gtk_widget_get_parent( widget ) ) && GTK_MENU( gtk_widget_get_parent( widget ) )->torn_off )
                {

                    Style::instance().renderWindowBackground( window, clipRect, x1-4, y-7, x2-x1+10, 20 );

                } else {

                    Style::instance().renderMenuBackground( window, clipRect, x1-4, y-7, x2-x1+8, 20, StyleOptions() );
                }

            }

            // separators
            bool accepted( true );
            if( widget )
            {
                // do not draw side hlines because they conflict with selection rect
                const GtkAllocation& allocation( widget->allocation );
                if( x1 <= allocation.x + 5 || x2 >= allocation.x + allocation.width - 5 )
                { accepted = false; }
            }

            if( accepted )
            { Style::instance().drawSeparator( window, clipRect, x1, y+1, x2-x1, 0, StyleOptions() ); }

        } else {

            StyleOptions options;
            if( !Gtk::gtk_parent_tree_view( widget ) )
            {
                options |= Blend;
                if( Gtk::gtk_parent_menu( widget ) ) options |= Menu;
            }

            Style::instance().drawSeparator( window, clipRect, x1, y, x2-x1, 0, options );

        }

    }

    //___________________________________________________________________________________________________________
    static void draw_vline( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint y1,
        gint y2,
        gint x )
    {
        g_return_if_fail( style && window );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=vline, state=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            detail );
        #endif

        // disable vline in buttons (should correspond to comboboxes)
        Gtk::Detail d( detail );
        if( d.isHScale() || Gtk::gtk_parent_button( widget ) ) return;
        else if( d.isToolBar() && !Style::instance().settings().toolBarDrawItemSeparator() ) return;
        else {

            StyleOptions options( Vertical );
            if( !Gtk::gtk_parent_tree_view( widget ) )
            {
                options |= Blend;
                if( Gtk::gtk_parent_menu( widget ) ) options |= Menu;
            }
            Style::instance().drawSeparator( window, clipRect, x+1, y1, 0, y2-y1, options );

        }

    }

    //___________________________________________________________________________________________________________
    static void draw_arrow( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        GtkArrowType arrow,
        gboolean fill,
        gint x,
        gint y,
        gint w,
        gint h )
    {

        g_return_if_fail( style && window );
        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=arrow, state=%s, shadow=%s, detail=%s, arrow=%s, fill=%d",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail,
            Maps::getArrow( arrow ),
            fill );
        #endif

        const Gtk::Detail d( detail );

        QtSettings::ArrowSize arrowSize( QtSettings::ArrowNormal );
        if( d.isMenuItem() && Style::instance().settings().applicationName().isMozilla() )
        { arrowSize = QtSettings::ArrowTiny; }

        StyleOptions options( Contrast );
        options |= StyleOptions( widget, state );

        if( d.isTearOffMenuItem() )
        {
            if( widget && gtk_widget_get_state( widget ) != GTK_STATE_PRELIGHT && GTK_IS_MENU( gtk_widget_get_parent( widget ) ) && GTK_MENU( gtk_widget_get_parent( widget ) )->torn_off )
            {

                Style::instance().renderWindowBackground( window, clipRect, x-8, y-8, w+16, h+16);

            }

            // disable highlight in menus, for consistancy with oxygen qt style
            options &= ~( Focus|Hover );

        } else if( d.isMenuItem() && !Gtk::gtk_parent_tree_view( widget ) ) {

            // disable highlight in menus, for consistancy with oxygen qt style
            options &= ~( Focus|Hover );

        } else if( d.isSpinButton() ) {

            /*
            TODO: this should be made more robust. What one really want is an arrow that is
            one pixel away from the centerline, no matter what
            */
            if( arrow == GTK_ARROW_UP ) y += 1;
            if( arrow == GTK_ARROW_DOWN ) y -= 1;
            x-=1;

            // disable contrast
            options &= ~Contrast;

        } else if( d.isNotebook() ) {

            if( GTK_IS_NOTEBOOK( widget ) )
            {
                const int offset = 6;
                switch( gtk_notebook_get_tab_pos( GTK_NOTEBOOK( widget ) ) )
                {
                    default:
                    case GTK_POS_TOP: h += offset; break;
                    case GTK_POS_LEFT: w += offset; break;
                    case GTK_POS_BOTTOM: y-=offset; h+=offset; break;
                    case GTK_POS_RIGHT: x -= offset; w += offset; break;
                }
            }

        } else if( GtkWidget* parent = Gtk::gtk_parent_combobox_entry( widget ) ) {

            if( GTK_IS_COMBO( parent ) )
            {
                Animations::instance().comboBoxEntryEngine().registerWidget( parent );
                if( GtkWidget* entry = Animations::instance().comboBoxEntryEngine().entry( parent ) )
                { state = gtk_widget_get_state( entry ); }
            }

            if( state != GTK_STATE_INSENSITIVE ) options &= ~Contrast;

        } else if( Gtk::gtk_parent_button( widget ) && !Gtk::gtk_parent_tree_view( widget ) ) {

            options &= ~( Focus|Hover );

        } else if( GTK_IS_CALENDAR( widget ) ) {

            // need to render background behind arrows from calendar
            // offsets are empirical
            Style::instance().renderWindowBackground( window, clipRect, x-2, y-3, w+4, h+6 );

        }

        // render arrow
        Style::instance().renderArrow( window, clipRect, arrow, x, y, w, h, arrowSize, options );

    }


    //___________________________________________________________________________________________________________
    static void draw_expander(
        GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const char* detail,
        gint x,
        gint y,
        GtkExpanderStyle expander_style )
    {
        g_return_if_fail( style && window );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=expander, state=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            detail );
        #endif

        StyleOptions options( widget, state );
        if( Style::instance().settings().viewDrawTriangularExpander() )
        {

            const GtkArrowType arrow( ( expander_style == GTK_EXPANDER_COLLAPSED || expander_style == GTK_EXPANDER_SEMI_COLLAPSED ) ?
                GTK_ARROW_RIGHT : GTK_ARROW_DOWN );

            const Gtk::Detail d( detail );
            QtSettings::ArrowSize arrowSize = QtSettings::ArrowNormal;
            if( d.isTreeView() ) arrowSize = Style::instance().settings().viewTriangularExpanderSize();
            else options |= Contrast;

            Style::instance().renderArrow( window, clipRect, arrow, x-3, y-4, 10, 10, arrowSize, options );

        } else {

            Style::instance().renderTreeExpander( window, clipRect, x-3, y-4, 10, 10, expander_style, options );

        }

    }

    //___________________________________________________________________________________________________________
    static void draw_diamond( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const char* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );
        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=diamond, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        oxygen_style_parent_class->draw_diamond( style, window, state,
            shadow, clipRect, widget, detail,
            x, y, w, h );
    }

    //___________________________________________________________________________________________________________
    static void draw_tab( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const char* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=tab, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isOptionMenuTab() )
        {

            // render
            GtkArrowType arrow = GTK_ARROW_DOWN;
            QtSettings::ArrowSize arrowSize = QtSettings::ArrowNormal;
            StyleOptions options( Contrast );
            Style::instance().renderArrow( window, clipRect, arrow, x, y, w, h, arrowSize, options );
            return;

        } else {

            oxygen_style_parent_class->draw_tab( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_shadow_gap( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h,
        GtkPositionType position,
        gint gap_x,
        gint gap_w )
    {
        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=shadow_gap, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isFrame() ) {

            const Gtk::Gap gap( gap_x, gap_w, position );
            if( shadow == GTK_SHADOW_IN ) {

                Style::instance().renderHoleBackground( window, clipRect, x-1, y-1, w+2, h+1 );
                Style::instance().renderHole( window, clipRect, x-1, y-1, w+2, h+1, gap, NoFill );

            } else if( shadow == GTK_SHADOW_OUT ) {

                StyleOptions options( NoFill );
                options |= StyleOptions( widget, GTK_STATE_NORMAL, shadow );
                options &= ~(Hover|Focus);
                Style::instance().renderSlab( window, clipRect, x-1, y-4, w+2, h+4, gap, options );

            } else if( shadow == GTK_SHADOW_ETCHED_IN || shadow == GTK_SHADOW_ETCHED_OUT ) {

                Style::instance().renderDockFrame( window, clipRect, x, y-1, w, h+1, gap, Blend );

            }

            return;

        } else {

            oxygen_style_parent_class->draw_shadow_gap( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h,
                position, gap_x, gap_w );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_box_gap( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h,
        GtkPositionType position,
        gint gap_x,
        gint gap_w )
    {
        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=box_gap, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        const Gtk::Detail d( detail );
        if( d.isNotebook() )
        {

            // this might move to drawShadowGap
            StyleOptions options( NoFill );
            options |= StyleOptions( widget, GTK_STATE_NORMAL, shadow );
            options &= ~(Hover|Focus);

            Gtk::Gap gap( gap_x, gap_w, position );
            if( Style::instance().settings().applicationName().isMozilla() )
            {

                /*
                firefox calls box_gap twice. Once for the main frame, once for the actual gap
                the second call must be discarded somehow
                */
                if( h>12 )
                { Style::instance().renderSlab( window, clipRect, x-1, y-3, w+2, h-4, gap, options ); }

            } else {

                // need adjustment depending on gap side
                const int adjust = 2;
                switch( position )
                {

                    case GTK_POS_TOP:
                    y -= adjust;
                    h += adjust;
                    break;

                    case GTK_POS_BOTTOM:
                    h += adjust;
                    break;

                    case GTK_POS_LEFT:
                    x -= adjust;
                    w +=  adjust;
                    break;

                    case GTK_POS_RIGHT:
                    w += adjust;
                    break;

                    default: return;

                }

                gap.setHeight( 8 );
                Style::instance().renderSlab( window, clipRect, x-1, y-1, w+2, h+2, gap, options );

            }

        } else {

            oxygen_style_parent_class->draw_box_gap( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h,
                position, gap_x, gap_w );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_slider( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h,
        GtkOrientation orientation )
    {
        g_return_if_fail( style && window );
        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=slider, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isScale() )
        {

            StyleOptions options( Blend );
            options |= StyleOptions( widget, state, shadow );
            if( GTK_IS_VSCALE( widget ) ) options |= Vertical;
            Style::instance().renderSliderHandle( window, clipRect, x, y, w, h, options );
            return;

        } else if( GTK_IS_VSCROLLBAR( widget ) ) {

            StyleOptions options( Vertical );
            options |= StyleOptions( widget, state, shadow );
            Style::instance().renderScrollBarHandle( window, clipRect, x, y, w-1, h, options );

        } else if( GTK_IS_HSCROLLBAR( widget ) ) {

            StyleOptions options( widget, state, shadow );
            Style::instance().renderScrollBarHandle( window, clipRect, x, y, w, h-1, options );

        } else {

            oxygen_style_parent_class->draw_slider( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h,
                orientation );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_extension( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h,
        GtkPositionType position )
    {

        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=extension, state=%s, shadow=%s, detail=%s, position=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail,
            Maps::getPosition( position ) );
        #endif

        Gtk::Detail d( detail );
        if( d.isTab() )
        {

            StyleOptions options( widget, state, shadow );
            TabOptions tabOptions( widget, state, position, x, y, w, h );

            /*
            see if tab is hovered. This is only done if widget is notebook, and if not running a mozilla
            app, because the latter do not pass the actual tab rect as argument
            */
            if( GTK_IS_NOTEBOOK( widget ) && !Style::instance().settings().applicationName().isMozilla() )
            {

                // make sure widget is registered
                Animations::instance().tabWidgetEngine().registerWidget( widget );

                // get current tab, update tabRect and see if current tab is hovered
                const int tabIndex( Gtk::gtk_notebook_find_tab( widget, x+w/2, y+h/2 ) );
                Animations::instance().tabWidgetEngine().updateTabRect( widget, tabIndex, x, y, w, h );
                if( tabIndex == Animations::instance().tabWidgetEngine().hoveredTab( widget ) )
                { options |= Hover; }

                // check tab position and add relevant option flags
                GtkNotebook* notebook( GTK_NOTEBOOK( widget ) );
                if( tabIndex == 0 ) tabOptions |= FirstTab;
                if( tabIndex == gtk_notebook_get_n_pages( notebook ) - 1 ) tabOptions |= LastTab;

                const int current( gtk_notebook_get_current_page( notebook ) );
                if( tabIndex == current-1 ) tabOptions |= LeftOfSelected;
                else if( tabIndex == current+1 ) tabOptions |= RightOfSelected;
            }

            // render
            Style::instance().renderTab( window, clipRect, x, y, w, h, position, options, tabOptions );

            Gtk::gtk_notebook_update_close_buttons(GTK_NOTEBOOK(widget));
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_focus(
        GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const char* detail,
        gint x,
        gint y,
        gint w,
        gint h )
    {

        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=focus, state=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            detail );
        #endif

    }


    //___________________________________________________________________________________________________________
    static void draw_handle( GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GtkShadowType shadow,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x,
        gint y,
        gint w,
        gint h,
        GtkOrientation orientation )
    {
        g_return_if_fail( style && window );

        Style::instance().sanitizeSize( window, w, h );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=handle, state=%s, shadow=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            Maps::getShadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isPaned() )
        {

            StyleOptions options( widget, state, shadow );
            if( GTK_IS_VPANED( widget ) ) options |= Vertical;
            Style::instance().renderSplitter( window, clipRect, x, y, w, h, options );

        } else if( d.isHandleBox() ) {

            StyleOptions options( widget, state, shadow );
            if( orientation == GTK_ORIENTATION_VERTICAL ) options |= Vertical;
            Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );
            Style::instance().renderToolBarHandle( window, clipRect, x, y, w, h, options );

        } else {

            oxygen_style_parent_class->draw_handle( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h,
                orientation );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_resize_grip(
        GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const char* detail,
        GdkWindowEdge edge,
        gint x,
        gint y,
        gint w,
        gint h )
    {
        g_return_if_fail( style && window );

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=resize_grip, state=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            detail );
        #endif

        // no resize grip in oxygen no matter what
        return;
    }

    //___________________________________________________________
    static GdkPixbuf* render_icon(
        GtkStyle* style,
        const GtkIconSource* source,
        GtkTextDirection,
        GtkStateType state,
        GtkIconSize size,
        GtkWidget* widget,
        const char* )
    {

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=icon, state=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ));
        #endif

        GdkPixbuf* base_pixbuf( gtk_icon_source_get_pixbuf( source ) );
        g_return_val_if_fail( base_pixbuf != 0L, 0L );

        // retrieve screen and settings
        GdkScreen *screen( 0L );
        GtkSettings *settings( 0L );
        if( widget && gtk_widget_has_screen( widget ) )
        {

            screen = gtk_widget_get_screen( widget );
            settings = gtk_settings_get_for_screen( screen );

        } else if (style->colormap) {

            screen = gdk_colormap_get_screen( style->colormap );
            settings = gtk_settings_get_for_screen( screen );

        } else {

            settings = gtk_settings_get_default();

        }

        int width = 1;
        int height = 1;
        if( size != (GtkIconSize)-1 && !gtk_icon_size_lookup_for_settings( settings, size, &width, &height ) )
        {
            g_warning (G_STRLOC ": invalid icon size '%d'", size);
            return 0L;
        }

        /* If the size was wildcarded, and we're allowed to scale, then scale; otherwise,
        * leave it alone. */
        GdkPixbuf *scaled( 0L);
        if( size != (GtkIconSize)-1 && gtk_icon_source_get_size_wildcarded( source ) )
        {

            scaled = Gtk::gdk_pixbuf_resize( base_pixbuf, width, height );

        } else {

            scaled = static_cast<GdkPixbuf*>( g_object_ref( base_pixbuf ) );

        }

        /* If the state was wildcarded, then generate a state. */
        GdkPixbuf *stated( scaled );
        if( gtk_icon_source_get_state_wildcarded( source ) )
        {

            if( state == GTK_STATE_INSENSITIVE )
            {

                stated = Gtk::gdk_pixbuf_set_alpha( scaled, 0.3 );
                gdk_pixbuf_saturate_and_pixelate( stated, stated, 0.1, false );
                g_object_unref (scaled);

            } else if (state == GTK_STATE_PRELIGHT) {

                stated = gdk_pixbuf_copy( scaled );
                if(!Gtk::gdk_pixbuf_to_gamma( stated, 0.5 ) )
                {
                    // FIXME: correct the value to match KDE
                    gdk_pixbuf_saturate_and_pixelate( scaled, stated, 1.2, false );
                }
                g_object_unref( scaled );

            }
        }

        return stated;
    }

    //___________________________________________________________________________________________________________
    static void draw_layout(
        GtkStyle* style,
        GdkWindow* window,
        GtkStateType state,
        gboolean use_text,
        GdkRectangle* clipRect,
        GtkWidget* widget,
        const gchar* detail,
        gint x, gint y,
        PangoLayout* layout)
    {

        #if OXYGEN_DEBUG
        g_log( OXYGEN_LOG_DOMAIN, G_LOG_LEVEL_INFO,
            "widget=%s, primitive=layout, state=%s, detail=%s",
            G_OBJECT_TYPE_NAME( widget ),
            Maps::getState( state ),
            detail );
        #endif

        if( state == GTK_STATE_INSENSITIVE )
        {

            // for inactive text, we do the painting ourselves
            // to prevent 'emboss' inactive text rendering from gtk
            Cairo::Context context( window, clipRect );
            gdk_cairo_set_source_color( context, use_text ? &style->text[state] : &style->fg[state] );
            const PangoMatrix* matrix( pango_context_get_matrix( pango_layout_get_context( layout ) ) );
            if( matrix )
            {
                cairo_matrix_t cairo_matrix;
                PangoRectangle rect;

                cairo_matrix_init( &cairo_matrix, matrix->xx, matrix->yx, matrix->xy, matrix->yy, matrix->x0, matrix->y0 );
                pango_layout_get_extents( layout, 0L, &rect );
                pango_matrix_transform_rectangle( matrix, &rect );
                pango_extents_to_pixels( &rect, 0L );

                cairo_matrix.x0 += x - rect.x;
                cairo_matrix.y0 += y - rect.y;

                cairo_set_matrix( context, &cairo_matrix );

            } else cairo_translate( context, x, y );

            pango_cairo_show_layout( context, layout );

        } else {

            // in all other cases, fallback on default rendering, for now
            oxygen_style_parent_class->draw_layout(
                style, window, state, use_text,
                clipRect, widget, detail, x, y, layout );

        }
    }
}

//_______________________________________________________________________________________________________________
extern "C" void instance_init( OxygenStyle* self )
{ Oxygen::Style::instance(); }

//_______________________________________________________________________________________________________________
static void class_init( OxygenStyleClass* klass )
{
    GtkStyleClass* style_class;

    style_class = GTK_STYLE_CLASS( klass );

    oxygen_style_parent_class = static_cast<GtkStyleClass*>( g_type_class_peek_parent( klass ) );

    style_class->draw_hline = Oxygen::draw_hline;
    style_class->draw_vline = Oxygen::draw_vline;
    style_class->draw_shadow = Oxygen::draw_shadow;
    style_class->draw_arrow = Oxygen::draw_arrow;

    style_class->draw_diamond = Oxygen::draw_diamond;

    style_class->draw_box = Oxygen::draw_box;
    style_class->draw_flat_box = Oxygen::draw_flat_box;
    style_class->draw_check = Oxygen::draw_check;
    style_class->draw_option = Oxygen::draw_option;
    style_class->draw_tab = Oxygen::draw_tab;
    style_class->draw_shadow_gap = Oxygen::draw_shadow_gap;
    style_class->draw_box_gap = Oxygen::draw_box_gap;
    style_class->draw_extension = Oxygen::draw_extension;
    style_class->draw_focus = Oxygen::draw_focus;
    style_class->draw_slider = Oxygen::draw_slider;
    style_class->draw_handle = Oxygen::draw_handle;
    style_class->draw_resize_grip = Oxygen::draw_resize_grip;
    style_class->draw_expander = Oxygen::draw_expander;

    // icon rendering
    style_class->render_icon = Oxygen::render_icon;

    // text rendering
    style_class->draw_layout = Oxygen::draw_layout;


}

//_______________________________________________________________________________________________________________
static GType oxygen_style_type = 0;
void oxygen_style_register_type( GTypeModule* module )
{
    if( !oxygen_style_type )
    {

        static const GTypeInfo info =
        {
            sizeof( OxygenStyleClass ),
            0L,
            0L,
            ( GClassInitFunc ) class_init,
            0L,          /* class_finalize */
            0L,          /* class_data */
            sizeof( OxygenStyle ),
            0,           /* n_preallocs */
            ( GInstanceInitFunc ) instance_init,
        };

        oxygen_style_type = g_type_module_register_type( module, GTK_TYPE_STYLE, "OxygenStyle", &info, GTypeFlags(0 ) );
    }
}

//_______________________________________________________________________________________________________________
GType oxygen_style_get_type( void )
{ return oxygen_style_type; }
