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
#include "oxygenargbhelper.h"
#include "oxygencairoutils.h"
#include "oxygengtkcellinfo.h"
#include "oxygengtkdetails.h"
#include "oxygengtktypenames.h"
#include "oxygengtkutils.h"
#include "oxygenstyle.h"
#include "oxygenwindowmanager.h"

#include <iostream>

namespace Oxygen
{

    //___________________________________________________________________________________________________________
    GtkStyleClass* StyleWrapper::_parentClass = 0L;
    GTypeInfo StyleWrapper::_typeInfo;
    GType StyleWrapper::_type = 0L;

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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        const Gtk::Detail d( detail );
        if( d.isBase() || d.isEventBox())
        {

            // special case for openoffice
            // we fill the background with flat color unless it is a toolbar
            if( Style::instance().settings().applicationName().isOpenOffice() )
            {
                if( !GTK_IS_TOOLBAR( widget ) )
                { Style::instance().fill( window, clipRect, x, y, w, h, Palette::Window ); }

                return;
            }

            // do nothing for mozilla, acrobat, gnome applets, and other hint-specific windows
            if(
                Style::instance().settings().applicationName().useFlatBackground( widget ) ||
                Gtk::gtk_widget_is_applet( widget ) ||
                Gtk::gdk_window_nobackground( window ) )
            { return; }

            // if background has been modified, simply fill with background color
            /*
            note: this is an inconsistent design choice. In principle we could just
            - register the widgets to the relevant engines as below
            - pass the modified color to renderWindowBackground
            */
            if( gtk_widget_get_modifier_style(widget)->color_flags[state]&GTK_RC_BG )
            {
                Style::instance().fill( window, clipRect, x, y, w, h, Gtk::gdk_get_color( style->bg[state] ) );
                return;
            }

            // register to relevant engines
            if( GTK_IS_WINDOW( widget ) )
            {

                Style::instance().animations().mainWindowEngine().registerWidget( widget );

            } else if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( widget ) ) {

                Style::instance().animations().scrollBarEngine().registerScrolledWindow( parent );

            }

            // register to window manager
            if( Gtk::gdk_window_is_base( window ) &&
                !( GTK_IS_EVENT_BOX( widget ) &&
                !gtk_event_box_get_above_child( GTK_EVENT_BOX( widget ) ) ) )
            { Style::instance().windowManager().registerWidget( widget ); }

            // change gtk dialog button order
            GtkWidget *toplevel = gtk_widget_get_toplevel( widget );
            if( GTK_IS_DIALOG( toplevel ) )
            { Style::instance().animations().dialogEngine().registerWidget( toplevel ); }

            // render background gradient
            Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );

            return;

        } else if( d.isViewportBin() ) {

            // do nothing for gnome applets
            if( Gtk::gtk_widget_is_applet( widget ) ) return;

            // for mozilla and openoffice
            // fill with flat color
            if( Style::instance().settings().applicationName().useFlatBackground( widget ) )
            {
                Style::instance().fill( window, clipRect, x, y, w, h, Palette::Window );
                return;
            }

            //register to window manager
            Style::instance().windowManager().registerWidget( widget );

            // for modified bg, fill with flat custom color
            if( gtk_widget_get_modifier_style(widget)->color_flags[state]&GTK_RC_BG )
            {
                Style::instance().fill( window, clipRect, x, y, w, h, Gtk::gdk_get_color( style->bg[state] ) );
                return;
            }

            // make sure that widget is registered to scrolledBarEngine,
            // so that background gets updated properly
            if( GtkWidget* parent = Gtk::gtk_parent_scrolled_window( widget ) )
            { Style::instance().animations().scrollBarEngine().registerScrolledWindow( parent ); }

            // render background gradient
            Style::instance().renderWindowBackground( window, widget, clipRect, x, y, w, h );
            return;

        } else if( d.isTrough() ) {

            if( GTK_IS_PROGRESS_BAR( widget ) )
            { return; }

        } else if( d.isTooltip() && Style::instance().settings().tooltipDrawStyledFrames() ) {

            // mozilla and openoffice get square non Argb tooltips no matter what
            if(
                Style::instance().settings().applicationName().isOpenOffice() ||
                Style::instance().settings().applicationName().isMozilla() )
            {
                Style::instance().renderTooltipBackground( window, clipRect, x, y, w, h, StyleOptions() );
                return;
            }

            StyleOptions options( Round );
            if( Gtk::gtk_widget_has_rgba( widget ) ) options |= Alpha;
            if( GDK_IS_WINDOW( window ) && !(options&Alpha) )
            {

                // make tooltips appear rounded using XShape extension if screen isn't composited
                Style::instance().animations().widgetSizeEngine().registerWidget( widget );
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                const bool sizeChanged( Style::instance().animations().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) );
                if( sizeChanged && ( gtk_widget_is_toplevel(widget) || GTK_IS_WINDOW(widget) ) )
                {
                    GdkPixmap* mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_mask( window, mask, x, y );
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
            if( d.isCellEven() || Gtk::gtk_combobox_is_tree_view( widget ) ) background = Style::instance().settings().palette().color( group, Palette::Base );
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

                const bool reversed( Gtk::gtk_widget_layout_is_reversed( widget ) );

                // draw rounded selection in normal list,
                // and detect hover
                bool forceCellStart( false );
                bool forceCellEnd( false );
                if( GTK_IS_TREE_VIEW( widget ) )
                {

                    GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                    Gtk::CellInfo cellInfo( treeView, x, y, w, h );

                    Style::instance().animations().treeViewEngine().registerWidget( widget );
                    if( Style::instance().animations().treeViewEngine().isDirty( widget ) )
                    { Style::instance().animations().treeViewEngine().updateHoveredCell( widget ); }

                    if( cellInfo.isValid() && Style::instance().animations().treeViewEngine().isCellHovered( widget, cellInfo ) )
                    { options |= Hover; }

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
                            StyleOptions options( widget, state, shadow );

                            // and render
                            Style::instance().renderTreeLines( window, clipRect, x, y, w, h, cellFlags, options );

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
                    if( (options&(Selected|Hover)) && cellInfo.isValid() && cellInfo.isLastVisibleColumn( treeView ) )
                    {
                        if( reversed ) forceCellStart = true;
                        else forceCellEnd = true;
                    }

                }

                if( options & (Selected|Hover) )
                {

                    TileSet::Tiles tiles( TileSet::Center );
                    if( d.isCellStart() ) tiles |= TileSet::Left;
                    else if( d.isCellEnd() ) tiles |= TileSet::Right;
                    else if( !d.isCellMiddle() ) tiles = TileSet::Horizontal;

                    if( forceCellStart ) tiles |= TileSet::Left;
                    if( forceCellEnd ) tiles |= TileSet::Right;

                    Style::instance().renderSelection( window, clipRect, x, y, w, h, tiles, options );

                }

            }

            return;

        } else if( d.isEntryBg() && !Style::instance().settings().applicationName().isMozilla( widget ) ) {

            StyleOptions options( widget, state, shadow );
            if(
                !Style::instance().settings().applicationName().isGoogleChrome() &&
                !Style::instance().settings().applicationName().isOpenOffice() )
            { options |= NoFill; }

            // calculate proper offsets so that the glow/shadow match parent frame
            const int xOffset( style->xthickness + 1 - Style::Entry_SideMargin );

            // adjust horizontal positioning and width
            x -= xOffset;
            w += 2*xOffset;

            if( GtkWidget* parent = Gtk::gtk_parent_combobox_entry( widget ) )
            {

                // check if parent is in style map
                Style::instance().animations().comboBoxEntryEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEntryEngine().setEntry( parent, widget );
                Style::instance().animations().comboBoxEntryEngine().setEntryFocus( parent, options & Focus );

                if( state != GTK_STATE_INSENSITIVE )
                {
                    if( Style::instance().animations().comboBoxEntryEngine().hasFocus( parent ) ) options |= Focus;
                    else options &= ~Focus;

                    if(  Style::instance().animations().comboBoxEntryEngine().hovered( parent ) ) options |= Hover;
                    else options &= ~Hover;
                }

                /*
                for some reason, adjusting y and h using ythickness does not work for combobox_entry
                one need to use parent allocation instead
                */
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( parent ) );
                y -= (allocation.height-h + 1)/2;
                h = allocation.height;

                // partial highlight
                TileSet::Tiles tiles( TileSet::Ring );

                if( Gtk::gtk_widget_layout_is_reversed( widget ) )
                {

                    // hide left part and increase width
                    tiles &= ~TileSet::Left;
                    Style::instance().renderHole( window, clipRect, x-7, y, w+7, h, options, tiles );

                } else {

                    // hide right part and increase width
                    tiles &= ~TileSet::Right;
                    Style::instance().renderHole( window, clipRect, x, y, w+7, h, options, tiles );

                }

            } else if( GTK_IS_SPIN_BUTTON( widget ) ) {

                const int yOffset( style->ythickness + 1 );

                // there is no need to render anything if both offsets are larger than 4
                if( xOffset > 4 && yOffset > 4 ) return;

                // adjust vertical positioning and height
                y -= yOffset;
                h += 2*yOffset;

                // for openoffice only draw solid window background
                // the rest of the spinbutton is painted on top, in draw_box and draw_shadow
                if( Style::instance().settings().applicationName().isOpenOffice() )
                {

                    Style::instance().fill( window, clipRect, x, y, w, h, Palette::Window );
                    return;
                }

                if(
                    Style::instance().animations().hoverEngine().contains( widget ) &&
                    Style::instance().animations().hoverEngine().hovered( widget ) )
                { options |= Hover; }

                // plain background
                ColorUtils::Rgba background( Gtk::gdk_get_color( style->base[gtk_widget_get_state(widget)] ) );
                Style::instance().fill( window, clipRect, x, y, w, h, background );

                // hole
                TileSet::Tiles tiles( TileSet::Ring );
                if( Gtk::gtk_widget_layout_is_reversed( widget ) )
                {

                    // hide right part and adjust width
                    tiles &= ~TileSet::Left;
                    Style::instance().renderHole( window, clipRect, x-5, y, w+5, h, options, tiles );

                } else {

                    // hide right part and adjust width
                    tiles &= ~TileSet::Right;
                    Style::instance().renderHole( window, clipRect, x, y, w+5, h, options, tiles );

                }

            } else  {

                const int yOffset( style->ythickness + 1 );

                // there is no need to render anything if both offsets are larger than 4
                if( xOffset > 4 && yOffset > 4 ) return;

                // adjust vertical positioning and height
                y -= yOffset;
                h += 2*yOffset;

                if(
                    Style::instance().animations().hoverEngine().contains( widget ) &&
                    Style::instance().animations().hoverEngine().hovered( widget ) )
                { options |= Hover; }

                // compare painting rect to widget rect, to decide if some sides are to be masked
                TileSet::Tiles tiles = TileSet::Ring;
                GdkWindow* widgetWindow( gtk_widget_get_window( widget ) );
                if( widget && window != widgetWindow && GDK_IS_WINDOW( window ) && widgetWindow == gdk_window_get_parent( window )  )
                {

                    const int widgetWindowWidth( Gtk::gtk_widget_get_allocation( widget ).width );
                    int localWindowX( 0 );
                    int localWindowWidth( 0 );
                    gdk_window_get_position( window, &localWindowX, 0L );
                    gdk_window_get_size( window, &localWindowWidth, 0L );

                    // remove left border if needed
                    if( localWindowX > 5 )
                    {
                        tiles &= ~TileSet::Left;
                        x -= 7;
                        w += 7;
                    }

                    // remove right border if needed
                    if( localWindowX + localWindowWidth < widgetWindowWidth - 5 )
                    {
                        tiles &= ~TileSet::Right;
                        w += 7;
                    }

                }

                // render hole
                Style::instance().renderHole( window, clipRect, x, y, w, h, options, tiles );

            }

            return;

        }

        // call parent method if reaching here
        StyleWrapper::parentClass()->draw_flat_box( style, window, state,
            shadow, clipRect, widget, detail,
            x, y, w, h );

    }

    //_____________________________________________________________________________________
    Cairo::Surface processTabCloseButton(GtkWidget* widget, GtkStateType state)
    {

        #if OXYGEN_DEBUG
        std::cout << "Oxygen::processTabCloseButton" << std::endl;
        #endif

        switch (state)
        {
            case GTK_STATE_NORMAL:
            {

                // check if our button is on active page and if not, make it gray
                GtkNotebook* notebook=GTK_NOTEBOOK(Gtk::gtk_parent_notebook(widget));
                GtkWidget* page=gtk_notebook_get_nth_page(notebook,gtk_notebook_get_current_page(notebook));
                if( !page ) break;

                GtkWidget* tabLabel=gtk_notebook_get_tab_label(notebook,page);
                if( !tabLabel ) break;

                if( !Gtk::gtk_widget_is_parent( widget, tabLabel ) ) return Style::instance().tabCloseButton( Disabled );
                else return Style::instance().tabCloseButton( StyleOptions() );

            }

            break;

            case GTK_STATE_ACTIVE: return Style::instance().tabCloseButton( Focus );
            case GTK_STATE_PRELIGHT: return Style::instance().tabCloseButton( Hover );
            default: break;

        }

        return 0L;

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
            widget?G_OBJECT_TYPE_NAME( widget ):"0x0",
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail?detail:"0x0" );
        #endif

        if(GTK_IS_MENU_BAR( widget ) )
        { Style::instance().animations().menuShellEngine().registerWidget(widget); }

        Style::instance().sanitizeSize( window, w, h );
        const Gtk::Detail d( detail );

        // OpenOffice doesn't call draw_box, so we have to draw it here to make steppers look not like slabs.
        if( d.isStepper() && Style::instance().settings().applicationName().isOpenOffice())
        {
            Style::instance().fill( window, clipRect, x, y, w-2, h-1, Palette::Window );
            return;
        }

        GtkWidget* parent(0L);
        if( d.isInfoBar() )
        {

            Style::instance().renderInfoBar( window, clipRect, x, y, w, h, Gtk::gdk_get_color( style->bg[state] ) );


        } else if( d.isButton() || d.isOptionMenu() || d.isToggleButton() ) {

            // check if it's PathBar toggle button
            if( Gtk::gtk_button_is_in_path_bar(widget) )
            {

                // https://bugzilla.gnome.org/show_bug.cgi?id=635511
                std::string name(G_OBJECT_TYPE_NAME( gtk_widget_get_parent( widget ) ) );

                // NautilusPathBar doesn't have any problem so only for GtkPathBar
                if( !Style::instance().settings().applicationName().isMozilla( widget ) &&
                    !Style::instance().settings().applicationName().isOpenOffice()
                    && name == "GtkPathBar" )
                { Style::instance().windowManager().registerWidget( widget ); }

                Style::instance().animations().hoverEngine().registerWidget( widget );

                // only two style options possible: hover or don't draw
                StyleOptions options;
                const bool reversed( Gtk::gtk_widget_layout_is_reversed( widget ) );
                const bool isLast( Gtk::gtk_path_bar_button_is_last( widget ) );
                if(state!=GTK_STATE_NORMAL && state!=GTK_STATE_INSENSITIVE)
                {
                    if( !(state==GTK_STATE_ACTIVE && !Style::instance().animations().hoverEngine().hovered( widget ) ) )
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

                        Style::instance().renderSelection(window,clipRect,x,y,w,h,TileSet::Full,options);
                    }
                }

                if( GTK_IS_TOGGLE_BUTTON(widget) && !isLast )
                {

                    options |= Contrast;

                    if( reversed ) Style::instance().renderArrow(window,NULL,GTK_ARROW_LEFT, x+3,y,5,h,QtSettings::ArrowNormal, options, Palette::WindowText);
                    else Style::instance().renderArrow(window,NULL,GTK_ARROW_RIGHT,x+w-8,y,5,h,QtSettings::ArrowNormal, options, Palette::WindowText);

                }

            } else if( ( parent = Gtk::gtk_parent_tree_view( widget ) ) ) {

                // register to scrolled window engine if any
                if(
                    GTK_IS_SCROLLED_WINDOW( parent = gtk_widget_get_parent( parent ) ) &&
                    Style::instance().animations().scrolledWindowEngine().contains( parent )
                    )
                { Style::instance().animations().scrolledWindowEngine().registerChild( parent, widget ); }

                // treevew header
                Style::instance().renderHeaderBackground( window, clipRect, x, y, w, h );

            } else if( ( parent = Gtk::gtk_parent_combobox_entry( widget ) ) ) {

                // keep track of whether button is active (pressed-down) or pre-lighted
                const bool buttonActive( state == GTK_STATE_ACTIVE || state == GTK_STATE_PRELIGHT );

                // get the state from the combobox
                /* this fixes rendering issues when the arrow is disabled, but not the entry */
                state = gtk_widget_get_state(parent);

                /*
                editable combobox button get a hole (with left corner hidden), and a background
                that match the corresponding text entry background.
                */

                StyleOptions options( widget, state, shadow );
                if(
                    !Style::instance().settings().applicationName().isOpenOffice() &&
                    !Style::instance().settings().applicationName().isGoogleChrome() )
                { options |= NoFill; }

                options |= Blend;

                // focus handling
                Style::instance().animations().comboBoxEntryEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEntryEngine().setButton( parent, widget );

                ColorUtils::Rgba background( Gtk::gdk_get_color( style->bg[state] ) );
                Style::instance().fill( window, clipRect, x, y, w, h, background );

                // update option accordingly
                if( state == GTK_STATE_INSENSITIVE ) options &= ~(Hover|Focus);
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
                TileSet::Tiles tiles( TileSet::Ring);
                if( Gtk::gtk_widget_layout_is_reversed( widget ) )
                {

                    // hide right and adjust width
                    tiles &= ~TileSet::Right;
                    Style::instance().renderHoleBackground(window,clipRect, x-1, y, w+6, h, tiles );

                    x += Style::Entry_SideMargin;
                    w -= Style::Entry_SideMargin;
                    Style::instance().renderHole( window, clipRect, x-1, y, w+6, h, options, tiles  );

                } else {

                    // hide left and adjust width
                    tiles &= ~TileSet::Left;
                    Style::instance().renderHoleBackground(window,clipRect, x-5, y, w+6, h, tiles );

                    w -= Style::Entry_SideMargin;
                    Style::instance().renderHole( window, clipRect, x-5, y, w+6, h, options, tiles  );

                }

                return;

            } else if(
                ( parent = Gtk::gtk_parent_combobox( widget ) ) &&
                !Style::instance().settings().applicationName().isMozilla( widget ) &&
                Gtk::gtk_combobox_appears_as_list( parent )
                )
            {

                const bool reversed( Gtk::gtk_widget_layout_is_reversed( widget ) );

                StyleOptions options( widget, state, shadow );
                options |= Blend;

                Style::instance().animations().comboBoxEngine().registerWidget( parent );
                Style::instance().animations().comboBoxEngine().setButton( parent, widget );
                Style::instance().animations().comboBoxEngine().setButtonFocus( parent, options & Focus );

                if( Gtk::gtk_combobox_has_frame( parent ) )
                {
                    if( Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;

                    TileSet::Tiles tiles( TileSet::Ring );
                    if( reversed )
                    {

                        tiles &= ~TileSet::Right;
                        Style::instance().renderButtonSlab( window, clipRect, x, y, w+7, h, options, tiles );

                    } else {

                        tiles &= ~TileSet::Left;
                        Style::instance().renderButtonSlab( window, clipRect, x-7, y, w+7, h, options, tiles );

                    }

                } else {

                    options |= Flat;
                    if( Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;
                    if( reversed ) Style::instance().renderButtonSlab( window, clipRect, x+1, y, w, h, options );
                    else Style::instance().renderButtonSlab( window, clipRect, x-1, y, w, h, options );

                }

            #if GTK_CHECK_VERSION(2, 20, 0)
            } else if( GTK_IS_TOOL_ITEM_GROUP( widget ) ) {

                return;

            #endif

            } else {

                // for google chrome, make GtkChromeButton appear as flat
                if(
                    Style::instance().settings().applicationName().isGoogleChrome() &&
                    !Gtk::gtk_button_is_flat( widget ) &&
                    Gtk::g_object_is_a( G_OBJECT( widget ), "GtkChromeButton" ) )
                { gtk_button_set_relief( GTK_BUTTON( widget ), GTK_RELIEF_NONE ); }

                StyleOptions options( Blend );
                options |= StyleOptions( widget, state, shadow );

                if( widget && Gtk::gtk_button_is_flat( widget ) )
                {

                    // set button as flat and disable focus
                    options |= Flat;
                    options &= ~Focus;

                    // register to Hover engine and check state
                    Style::instance().animations().hoverEngine().registerWidget( widget );
                    if( Style::instance().animations().hoverEngine().hovered( widget ) )
                    { options |= Hover; }

                } else if( Gtk::gtk_parent_combo( widget ) ) {

                    if( Style::instance().settings().applicationName().isOpenOffice() )
                    {

                        // Hover doesn't work correctly in OpenOffice, so disable it
                        options &= ~(Hover|Focus);
                        Style::instance().renderHole(window,clipRect,x-8,y-1,w+9,h+2,options,(TileSet::Tile)(TileSet::Full & (~TileSet::Left)));
                        return;

                    }

                    /*
                    make button flat; disable focus and hover
                    (this is handled when rendering the arrow)
                    */
                    options |= Flat;
                    options &= ~(Hover|Focus);
                }

                if( Gtk::gtk_notebook_is_close_button(widget)) {

                    if( gtk_button_get_relief(GTK_BUTTON(widget))==GTK_RELIEF_NONE )
                    { gtk_button_set_relief(GTK_BUTTON(widget),GTK_RELIEF_NORMAL); }

                    if( Cairo::Surface surface = processTabCloseButton(widget,state) )
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
                        Cairo::Context context( window, clipRect );
                        cairo_set_source_surface( context, surface, x, y);
                        cairo_paint(context);

                    }

                } else {

                    if( style )
                    { options._customColors.insert( options&Flat ? Palette::Window:Palette::Button, Gtk::gdk_get_color( style->bg[state] ) ); }

                    Style::instance().renderButtonSlab( window, clipRect, x, y, w, h, options );

                }

            }

        } else if( d.isMenuBar() || d.isToolBar() ) {

            if( Style::instance().settings().applicationName().useFlatBackground( widget ) ||
                Gtk::gtk_widget_is_applet( widget ) )
                { return; }

            Style::instance().windowManager().registerWidget( widget );
            Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );
            return;

        } else if( d.isMenu() ) {

            if( GTK_IS_MENU( widget ) && gtk_menu_get_tearoff_state( GTK_MENU( widget ) ) )
            {

                Style::instance().renderWindowBackground( window, widget, clipRect, x, y, w, h );

            } else {

                StyleOptions options( Menu );

                // set alpha flag. Special handling is needed for mozilla and openoffice.
                if( Style::instance().settings().applicationName().isMozilla( widget ) ||
                    Style::instance().settings().applicationName().isOpenOffice() )
                {

                    Style::instance().renderMenuBackground( window, clipRect, x, y, w, h, options );

                    // since menus are rendered square anyway, we can set the alpha channel
                    // based on the screen properties only, in order to prevent ugly shadow to be drawn
                    if( Gtk::gdk_default_screen_is_composited() ) options |= Alpha;
                    Style::instance().drawFloatFrame( window, clipRect, x, y, w, h, options );
                    return;
                }

                options |= Round;
                if( Gtk::gtk_widget_has_rgba( widget ) ) options |= Alpha;

                // add mask if needed
                if( GTK_IS_MENU(widget) )
                {
                    if( !(options&Alpha) )
                    {

                        // make menus appear rounded using XShape extension if screen isn't composited
                        Style::instance().animations().widgetSizeEngine().registerWidget( widget );
                        const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                        if( Style::instance().animations().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) )
                        {
                            GdkPixmap* mask( Style::instance().helper().roundMask( w, h - 2*Style::Menu_VerticalOffset ) );
                            gdk_window_shape_combine_mask( gtk_widget_get_parent_window(widget), mask, 0, Style::Menu_VerticalOffset );
                            gdk_pixmap_unref(mask);
                        }

                    }
                    else
                    {
                        // reset mask if compositing has appeared after we had set a mask
                        gdk_window_shape_combine_mask( gtk_widget_get_parent_window(widget), NULL, 0, 0);
                    }
                }

                // if render
                if( !Style::instance().renderMenuBackground( window, clipRect, x, y, w, h, options ) )
                { options &= ~Round; }

                Style::instance().drawFloatFrame( window, clipRect, x, y, w, h, options );

            }

        } else if( d.isMenuScrollArrow() ) {

            return;

        } else if( d.isDefaultButton() || d.isScrollBar() || d.isPaned() || d.isHandleBox() ) {

            return;

        } else if( d.isDockItem() ) {

            // force window background for dock-items. Fixes inkscape docks
            Style::instance().renderWindowBackground( window, clipRect, x, y, w, h );

        } else if( d.isMenuItem() ) {

            StyleOptions options( widget, state, shadow );
            if( !Style::instance().settings().applicationName().useFlatBackground( widget ) )
            { options |= Blend; }

            Style::instance().renderMenuItemRect( window, clipRect, widget, x, y, w, h, options );

        } else if( d.isTroughAny() && GTK_IS_SCALE( widget ) ) {

            const bool vertical( Gtk::gtk_widget_is_vertical( widget ) );
            const int offset( 6 );
            if( vertical ) {

                // TODO: calculate this value from the style "slider-width" property
                if( d.isTrough() ) Style::instance().renderSliderGroove( window, clipRect, x, y + offset, w, h - 2*offset, Vertical );
                else if( d.isTroughLower() ) Style::instance().renderSliderGroove( window, clipRect, x, y + offset, w, h, Vertical );
                else if( d.isTroughUpper() ) Style::instance().renderSliderGroove( window, clipRect, x, y, w, h - offset, Vertical );

            } else {

                // TODO: calculate this value from the style "slider-width" property
                const int offset( 6 );
                if( d.isTrough() ) Style::instance().renderSliderGroove( window, clipRect, x + offset, y, w - 2*offset, h, StyleOptions() );
                else if( d.isTroughLower() ) Style::instance().renderSliderGroove( window, clipRect, x + offset, y, w, h, StyleOptions() );
                else if( d.isTroughUpper() ) Style::instance().renderSliderGroove( window, clipRect, x, y, w - offset, h, StyleOptions() );

            }

        } else if( d.isTrough() && shadow == GTK_SHADOW_IN ) {

            if( GTK_IS_PROGRESS_BAR( widget ) )
            {

                if( !Style::instance().settings().applicationName().isMozilla( widget ) &&
                    !Style::instance().settings().applicationName().isOpenOffice() )
                {
                    /*
                    need to call the parent style implementation here,
                    otherwise some uninitialized pixels are present.
                    Not sure why
                    */
                    StyleWrapper::parentClass()->draw_box( style, window, state, shadow, clipRect, widget, detail, x, y, w, h );

                    if( !Gtk::gtk_widget_is_applet( widget ) )
                    { Style::instance().renderWindowBackground( window, widget, clipRect, x, y, w, h ); }
                }

                StyleOptions options;
                if( Gtk::gtk_widget_is_vertical( widget ) ) options |= Vertical;
                Style::instance().renderProgressBarHole( window, clipRect, x, y, w, h, options );

            } else if( GTK_IS_VSCROLLBAR( widget ) ) {

                if(Style::instance().settings().applicationName().isOpenOffice() )
                {

                    // OpenOffice doesn't call draw_box to draw background
                    Cairo::Context context(window,clipRect);
                    cairo_set_source(context,Gtk::gdk_get_color(style->bg[GTK_STATE_NORMAL]));
                    cairo_paint(context);

                    // adjust scrollbar hole since it has wrong geometry in OOo
                    y-=1; h+=1;
                }
                Style::instance().adjustScrollBarHole( x, y, w, h, Vertical );
                Style::instance().renderScrollBarHole( window, clipRect, x, y+1, w-1, h-1, Vertical );

            } else if( GTK_IS_HSCROLLBAR( widget ) ) {

                if(Style::instance().settings().applicationName().isOpenOffice() )
                {
                    // OpenOffice doesn't call draw_box to draw background
                    Cairo::Context context(window,clipRect);
                    cairo_set_source(context,Gtk::gdk_get_color(style->bg[GTK_STATE_NORMAL]));
                    cairo_paint(context);

                    // adjust scrollbar hole since it has wrong geometry in OOo
                    x-=2; w+=1;
                }
                Style::instance().adjustScrollBarHole( x, y, w, h, StyleOptions() );
                Style::instance().renderScrollBarHole( window, clipRect, x+1, y, w-2, h-1, StyleOptions() );

            }

        } else if( d.isSpinButton()) {

            StyleOptions options( widget, state, shadow );
            options |= Blend;

            if( Style::instance().settings().applicationName().isOpenOffice() )
            {

                // adjust rect
                y+=1;
                h-=2;
                x-=1;
                w+=1;

                // also first draw solid window background
                Style::instance().fill( window, clipRect, x, y, w, h, Palette::Window );

            } else {

                options |= NoFill;
                ColorUtils::Rgba background( Gtk::gdk_get_color( style->base[gtk_widget_get_state(widget)] ) );
                if( Style::instance().settings().applicationName().isMozilla( widget ) )
                {

                    /*
                    for firefox on has to mask out the corners manually,
                    because renderholebackground fails
                    */
                    Cairo::Context context( window, clipRect );
                    cairo_rounded_rectangle( context, x-4, y+2, w+3, h-4, 2, CornersRight );
                    cairo_set_source( context, background );
                    cairo_fill( context );

                    x += 1;
                    w += 2;

                } else Style::instance().fill( window, clipRect, x, y, w, h, background );

            }

            if(
                Style::instance().animations().hoverEngine().contains( widget ) &&
                Style::instance().animations().hoverEngine().hovered( widget ) )
            { options |= Hover; }

            TileSet::Tiles tiles( TileSet::Ring);

            if( Gtk::gtk_widget_layout_is_reversed( widget ) )
            {

                tiles &= ~TileSet::Right;

                if( !Style::instance().settings().applicationName().isOpenOffice() &&
                    !Style::instance().settings().applicationName().isMozilla( widget ) )
                { Style::instance().renderHoleBackground(window,clipRect, x-5, y-1, w+6, h+2, tiles ); }

                // shrink spinbox entry hole by 3px on right side
                x += Style::Entry_SideMargin;
                w -= Style::Entry_SideMargin;
                Style::instance().renderHole( window, clipRect, x-1, y-1, w+6, h+2, options, tiles );

            } else {

                tiles &= ~TileSet::Left;

                if( !Style::instance().settings().applicationName().isOpenOffice() &&
                    !Style::instance().settings().applicationName().isMozilla( widget ) )
                { Style::instance().renderHoleBackground(window,clipRect, x-5, y-1, w+6, h+2, tiles ); }

                // shrink spinbox entry hole by 3px on right side
                w -= Style::Entry_SideMargin;
                Style::instance().renderHole( window, clipRect, x-5, y-1, w+6, h+2, options, tiles );

            }

        } else if( d.isSpinButtonArrow() ) {

            return;

        } else if( d.isBar() ) {

            StyleOptions options( widget, state, shadow );
            if(GTK_IS_PROGRESS_BAR(widget))
            {

                // get orientation
                if( Gtk::gtk_widget_is_vertical( widget ) )
                { options |= Vertical; }

                Style::instance().renderProgressBarHandle( window, clipRect, x, y, w, h, options );

            } else {

                // most likely it's progressbar in the list
                // FIXME: is it always the case ? Should we check on TREE_VIEW, CELL_VIEW, like done with scrollbar hole ?
                Style::instance().renderProgressBarHandle( window, clipRect, x, y+2, w, h-4, options );

            }

            return;

        } else if( d.isEntryProgress() ) {

            StyleOptions options( widget, state, shadow );
            Style::instance().renderProgressBarHandle( window, clipRect, x-1, y+1, w+2, h-2, options );

        } else if( d.isTroughFillLevel () ) {

            return;

        } else if( d.isRuler() ) {

            Style::instance().renderWindowBackground(window,widget,clipRect,x,y,w,h);

        } else {

            StyleWrapper::parentClass()->draw_box( style, window, state,
                shadow, clipRect, widget, detail,
                x, y, w, h );
        }

    }

    //___________________________________________________________________________________________________________
    static void draw_shadow(
        GtkStyle* style,
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
            widget?G_OBJECT_TYPE_NAME( widget ):"0x0",
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail?detail:"0x0" );
        #endif

        GtkWidget* parent( 0L );
        const Gtk::Detail d( detail );

        // ugly Gnumeric header
        if( d.is("GnmItemBarCell") )
        { return; }

        // adjust shadow type for some known widgets
        if( d.isScrolledWindow() &&
            shadow != GTK_SHADOW_IN &&
            GTK_IS_SCROLLED_WINDOW( widget ) &&
            Gtk::gtk_scrolled_window_force_sunken( widget ) )
        {

            // make sure that scrolled windows containing a treeView have sunken frame
            shadow = GTK_SHADOW_IN;
            gtk_scrolled_window_set_shadow_type( GTK_SCROLLED_WINDOW( widget ), GTK_SHADOW_IN );

        } else if(
            d.isFrame() &&
            ( shadow == GTK_SHADOW_ETCHED_IN || shadow == GTK_SHADOW_ETCHED_OUT ) &&
            GTK_IS_FRAME( widget ) &&
            Gtk::gtk_scrolled_window_force_sunken( widget )
            )
        {

            // make sure that entry shadows are drawn
            shadow = GTK_SHADOW_IN;
            gtk_frame_set_shadow_type( GTK_FRAME( widget ), GTK_SHADOW_IN );

        } else if( d.isEntry() && shadow != GTK_SHADOW_IN ) {

            // make sure that entry shadows are drawn
            shadow = GTK_SHADOW_IN;

        }

        // check if it's combobox list window
        if( Gtk::gtk_combobox_is_scrolled_window( widget ) && GTK_IS_WINDOW( parent = gtk_widget_get_parent( widget ) ) )
        {

            // setup options
            StyleOptions options( Round );
            if( Gtk::gtk_widget_has_rgba(parent) ) options|=Alpha;

            // store parent allocation
            GtkAllocation allocation( Gtk::gtk_widget_get_allocation( parent ) );

            if( !(options&Alpha) )
            {
                // the same as with menus and tooltips (but changed a bit to take scrollbars into account)
                // make background window rounded
                Style::instance().animations().widgetSizeEngine().registerWidget( parent );
                if( Style::instance().animations().widgetSizeEngine().updateSize( parent, allocation.width, allocation.height ) )
                {
                    GdkPixmap* mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_mask( gtk_widget_get_window( parent ), mask, 0, 0 );
                    gdk_pixmap_unref( mask );
                }

            }

            if( GList* children=gtk_container_get_children(GTK_CONTAINER( widget )) )
            {
                widget=GTK_WIDGET( g_list_first(children)->data );
                Style::instance().animations().widgetSizeEngine().registerWidget( widget );
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
                if( Style::instance().animations().widgetSizeEngine().updateSize( widget, allocation.width, allocation.height ) )
                {

                    // offset is needed to make combobox list border 3px wide instead of default 2
                    // additional pixel is for ugly shadow
                    const gint offset( options&Alpha ? 0:1 );
                    GdkPixmap* mask( Style::instance().helper().roundMask(
                        allocation.width - 2*offset,
                        allocation.height - 2*offset,
                        3 ) );

                    gdk_window_shape_combine_mask( gtk_widget_get_window( widget ), mask, offset, offset );
                    gdk_pixmap_unref( mask );
                }


                if( children ) g_list_free( children );
            }

            // menu background and float frame
            GdkWindow* parentWindow( gtk_widget_get_window( parent ) );
            Style::instance().renderMenuBackground( parentWindow, clipRect, allocation.x, allocation.y, allocation.width, allocation.height, options );
            Style::instance().drawFloatFrame( parentWindow, clipRect, allocation.x, allocation.y, allocation.width, allocation.height, options );

            #if ENABLE_COMBOBOX_LIST_RESIZE

            // find matching pressed down combobox (or comboboxEntry)
            int sideMargin( 3 );
            GtkWidget* combobox = Style::instance().animations().comboBoxEngine().pressedComboBox();
            if( Style::Entry_SideMargin && !combobox )
            {
                combobox = Style::instance().animations().comboBoxEntryEngine().pressedComboBoxEntry();
                sideMargin = Style::Entry_SideMargin;
            }

            // resize the list to match combobox width (taking into account its lesser width because of button glow)
            if( combobox )
            {

                int w, h;
                GtkWindow* window( GTK_WINDOW( parent ) );
                gtk_window_get_size( window, &w, &h );
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( combobox ) );
                if( allocation.width-6 != w )
                {
                    gtk_widget_set_size_request( parent, allocation.width - 6, h );

                    gint targetX, dummy, y;
                    gtk_window_get_position( window, &dummy, &y );
                    gdk_window_get_origin( gtk_widget_get_window( combobox ), &targetX, &dummy);
                    gtk_window_move( window, targetX + allocation.x + 3, y );
                }

            }
            #endif

            return;

        } else if( d.isBase() && GTK_IS_MENU( widget ) ) {

                // this is to prevent crappy mozilla to
                // draw yet another frame around menus
                return;

        } else if( Gtk::gtk_combobox_is_viewport( widget ) ) {

            return;

        } else if( Gtk::gtk_combobox_is_frame( widget ) ) {

            // make GtkCombo list look a bit better
            // retrieve proper parent and check
            GtkWidget* parent=gtk_widget_get_parent(widget);
            if( parent ) parent=gtk_widget_get_parent(parent);
            if( !( parent && GTK_IS_WINDOW(parent) ) ) return;

            // setup options
            StyleOptions options( Round );
            if( Gtk::gtk_widget_has_rgba(parent) ) options|=Alpha;

            const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( parent ) );
            if( !(options&Alpha) )
            {
                // the same as with menus and tooltips (but changed a bit to take scrollbars into account)
                // make background window rounded
                Style::instance().animations().widgetSizeEngine().registerWidget(parent);
                if( Style::instance().animations().widgetSizeEngine().updateSize(parent,allocation.width,allocation.height))
                {
                    GdkPixmap* mask( Style::instance().helper().roundMask( allocation.width, allocation.height ) );
                    gdk_window_shape_combine_mask( gtk_widget_get_window( parent ), mask, 0, 0 );
                    gdk_pixmap_unref(mask);
                }
            }

            // menu background and float frame
            Style::instance().renderMenuBackground( window, clipRect, x, y, w, h, options );
            Style::instance().drawFloatFrame( window, clipRect, x, y, w, h, options );

            return;

        } else if( d.isSlider() || d.isRuler() || d.isDragAndDrop() ) {

            return;

        } else if( ( d.isEntry() || d.isViewport() || d.isScrolledWindow() ) && shadow == GTK_SHADOW_IN ) {

            StyleOptions options( widget, state, shadow );
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

                if( Gtk::gtk_widget_layout_is_reversed( widget ) )
                {

                    tiles &= ~TileSet::Left;
                    Style::instance().renderHoleBackground( window, clipRect, x-6, y, w+7, h, tiles );

                    w -= Style::Entry_SideMargin;
                    Style::instance().renderHole( window, clipRect, x-6, y, w+7, h, options, tiles );

                } else {

                    tiles &= ~TileSet::Right;
                    Style::instance().renderHoleBackground( window, clipRect, x-1, y, w+7, h, tiles );

                    x += Style::Entry_SideMargin;
                    w -= Style::Entry_SideMargin;
                    Style::instance().renderHole( window, clipRect, x-1, y, w+7, h, options, tiles );

                }

            } else if( GTK_IS_SPIN_BUTTON( widget ) ) {

                // register to hover engine
                Style::instance().animations().hoverEngine().registerWidget( widget, true );
                if( Style::instance().animations().hoverEngine().hovered( widget ) )
                { options |= Hover; }

                if( !Style::instance().settings().applicationName().isMozilla( widget ) )
                {

                    // fill the inside of the spinbox manually
                    ColorUtils::Rgba background( Gtk::gdk_get_color( style->base[gtk_widget_get_state( widget )] ) );
                    if( Style::instance().settings().applicationName().isOpenOffice() )
                    {

                        // for openoffice on has to properly round the corners
                        Cairo::Context context( window, clipRect );
                        cairo_rounded_rectangle( context, x+1, y+1, w-1, h-3, 2, CornersLeft );
                        cairo_set_source( context, background );
                        cairo_fill( context );

                    } else {

                        Style::instance().fill( window, clipRect, x, y, w, h, background );

                    }

                }

                // vertical alignment
                if( !Style::instance().settings().applicationName().isOpenOffice() )
                { y-=1; h+=2; }

                // basic adjustments
                x-=1; w+=2;

                TileSet::Tiles tiles( TileSet::Ring );

                if( Gtk::gtk_widget_layout_is_reversed( widget ) )
                {

                    tiles &= ~TileSet::Left;

                    if( !Style::instance().settings().applicationName().isOpenOffice() )
                    {
                        Style::instance().renderHoleBackground( window, clipRect, x, y, w, h, tiles );
                        w-= Style::Entry_SideMargin;
                    }

                    Style::instance().renderHole( window, clipRect, x-5, y, w+5, h, options, tiles );

                } else {

                    tiles &= ~TileSet::Right;

                    if( !Style::instance().settings().applicationName().isOpenOffice() )
                    {
                        Style::instance().renderHoleBackground( window, clipRect, x, y, w, h, tiles );
                        x += Style::Entry_SideMargin; w-= Style::Entry_SideMargin;
                    }

                    Style::instance().renderHole( window, clipRect, x, y, w+5, h, options, tiles );

                }

            } else {

                if( Style::instance().settings().applicationName().isGoogleChrome() && GTK_IS_HBOX( widget ) )
                { options &= ~NoFill; }

                // register to hover engine
                if( GTK_IS_ENTRY( widget ) && !Style::instance().settings().applicationName().isOpenOffice() )
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

                if( Style::instance().settings().applicationName().isOpenOffice() )
                {

                    if( d.isEntry() )
                    {

                        options &= ~NoFill;
                        Style::instance().renderHoleBackground( window, clipRect, x-1, y-1, w+2, h+2 );
                        Style::instance().renderHole( window, clipRect, x-1, y-1, w+2, h+2, options );

                    } else {

                        Style::instance().renderHole( window, clipRect, x, y, w, h, options );

                    }

                } else {

                    // basic adjustments
                    x-=1; y-=1;
                    w+=2; h+=2;

                    if( !Style::instance().settings().applicationName().isMozilla( widget ) )
                    { Style::instance().renderHoleBackground( window, clipRect, x, y, w, h ); }

                    // shrink entry by 3px at each side
                    if( d.isEntry() )
                    {
                        x += Style::Entry_SideMargin;
                        w -= 2*Style::Entry_SideMargin;
                    }
                    Style::instance().renderHole( window, clipRect, x, y, w, h, options );

                }

            }

            return;

        } else if( ( d.isTrough() || d.isBar() ) && GTK_IS_PROGRESS_BAR( widget ) ) {

            return;

        } else if (GTK_IS_NOTEBOOK(widget)) {

            if( !Style::instance().settings().applicationName().isOpenOffice() )
            { Style::instance().renderWindowBackground( window, clipRect, x-4, y-4, w+8, h+8 ); }

            Style::instance().renderSlab(window,clipRect,x-1,y-1,w+2,h+2,NoFill);

        } else if( GTK_IS_CALENDAR( widget ) && shadow == GTK_SHADOW_OUT ) {

            // calendar header
            if( style )
            {
                Style::instance().fill( window,clipRect,x-2,y-2,w+4,h+4, Gtk::gdk_get_color( style->base[state] ) );
                Style::instance().renderWindowBackground( window, widget, clipRect,x+2,y+2,w-4,h-6 );
            }

            StyleOptions options( Blend );
            options |= NoFill;
            Style::instance().renderSlab(window,clipRect,x-2,y-2,w+4,h+2, options );

        } else if(
            (parent = Gtk::gtk_parent_combobox( widget )) &&
            !GTK_IS_CELL_VIEW( widget ) &&
            !Style::instance().settings().applicationName().isMozilla( widget ) ) {

            Style::instance().animations().comboBoxEngine().registerWidget( parent );
            Style::instance().animations().comboBoxEngine().registerChild( parent, widget );
            GtkShadowType shadow( Style::instance().animations().comboBoxEngine().pressed( parent ) ? GTK_SHADOW_IN:GTK_SHADOW_OUT );
            StyleOptions options( widget, state, shadow );
            options |= Blend;

            if( Style::instance().animations().comboBoxEngine().hasFocus( parent ) ) options |= Focus;
            else options &= ~Focus;

            if(  Style::instance().animations().comboBoxEngine().hovered( parent ) ) options |= Hover;
            else options &= ~Hover;

            TileSet::Tiles tiles( TileSet::Ring );

            if( Gtk::gtk_widget_layout_is_reversed( widget ) )
            {

                tiles &= ~TileSet::Left;
                Style::instance().renderButtonSlab( window, clipRect, x-10, y, w+10, h, options, tiles );

            } else {

                tiles &= ~TileSet::Right;
                Style::instance().renderButtonSlab( window, clipRect, x, y, w+10, h, options, tiles );

            }

        } else if( d.isNull() && (GTK_IS_TREE_VIEW(widget) || GTK_IS_CELL_VIEW(widget)) && shadow==GTK_SHADOW_IN ) {

            // it's likely progressbar hole
            // FIXME: is it enough to check for TreeView? is shadow_in the only possible case?
            StyleOptions options;
            Style::instance().renderProgressBarHole(window,clipRect,x-2,y,w+4,h,options);

        } else if( shadow == GTK_SHADOW_IN && !Gtk::gtk_parent_statusbar( widget ) ) {

            if( GTK_IS_FRAME( widget ) )
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
                Style::instance().renderHoleBackground(
                    window, clipRect,
                    x-1-Style::Entry_SideMargin, y-1, w+2+2*Style::Entry_SideMargin, h+2 );
            }

            // hole
            Style::instance().renderHole( window, clipRect, x-1, y-1, w+2, h+1, NoFill );

        } else if( (shadow == GTK_SHADOW_ETCHED_IN || shadow == GTK_SHADOW_ETCHED_OUT) && !Gtk::gtk_parent_button( widget )) {

            // default etched frame
            Style::instance().renderDockFrame( window, clipRect, x, y+1, w, h-2, Blend );

        } else if( shadow == GTK_SHADOW_OUT ) {

            // default shadow_out frame
            StyleOptions options( Blend );
            options |= NoFill;
            Style::instance().renderSlab( window, clipRect, x-1, y-1, w+2, h+2, options );

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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
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
                options &= ~(Focus|Hover);
                if( GTK_IS_TREE_VIEW( widget ) )
                {
                    GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                    Gtk::CellInfo cellInfo( treeView, x, y, w, h );
                    if( cellInfo.isValid() &&
                        Style::instance().animations().treeViewEngine().contains( widget ) &&
                        Style::instance().animations().treeViewEngine().isCellHovered( widget, cellInfo, false ) )
                    { options |= Hover; }
                }

            }

            Style::instance().renderCheckBox( window, clipRect, x, y, w, h, shadow, options );

        } else if( d.isCheck() && GTK_IS_CHECK_MENU_ITEM( widget ) ) {

            StyleOptions options( widget, state, shadow );
            options |= (Blend|Flat|NoFill );
            Style::instance().renderCheckBox( window, clipRect, x, y, w, h, shadow, options );

        } else {

            StyleWrapper::parentClass()->draw_check( style, window, state,
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isRadioButton() )
        {

            StyleOptions options( widget, state, shadow );
            if( !Gtk::gtk_parent_tree_view( widget ) ) options |= Blend;
            Style::instance().renderRadioButton( window, clipRect, x, y, w, h, shadow, options );

        } else if( d.isOption() || d.isCellRadio() ) {

            // load options
            StyleOptions options( widget, state, shadow );
            if( !( d.isCellRadio() || Gtk::gtk_parent_tree_view( widget ) ) )
            {
                options |= Blend;
                if( Gtk::gtk_parent_menu( widget ) )
                {

                    // add menu flag and disable Hover/Focus
                    options|=Menu;
                    options &= ~(Hover|Focus);
                    x-=1;
                    y-=1;
                }

            }

            if( d.isCellRadio() )
            {
                options &= ~(Focus|Hover);
                if( GTK_IS_TREE_VIEW( widget ) )
                {
                    GtkTreeView* treeView( GTK_TREE_VIEW( widget ) );
                    Gtk::CellInfo cellInfo( treeView, x, y, w, h );
                    if( cellInfo.isValid() &&
                        Style::instance().animations().treeViewEngine().contains( widget ) &&
                        Style::instance().animations().treeViewEngine().isCellHovered( widget, cellInfo, false ) )
                    { options |= Hover; }
                }

            }

            Style::instance().renderRadioButton( window, clipRect, x, y, w, h, shadow, options );

        } else {

            StyleWrapper::parentClass()->draw_option( style, window, state,
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
            Gtk::TypeNames::state( state ),
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
                if(
                    widget &&
                    GTK_IS_MENU( gtk_widget_get_parent( widget ) ) &&
                    gtk_menu_get_tearoff_state( GTK_MENU( gtk_widget_get_parent( widget ) ) ) )
                {

                    Style::instance().renderWindowBackground( window, widget, clipRect, x1-4, y-7, x2-x1+10, 20 );

                } else {

                    Style::instance().renderMenuBackground( window, clipRect, x1-4, y-7, x2-x1+8, 20, Menu );
                }

            }

            // separators
            bool accepted( true );
            if( widget )
            {
                // do not draw side hlines because they conflict with selection rect
                const GtkAllocation allocation( Gtk::gtk_widget_get_allocation( widget ) );
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
            Gtk::TypeNames::state( state ),
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail,
            Gtk::TypeNames::arrow( arrow ),
            fill );
        #endif

        {
            // Deal with GimpScaleComboBox misplaced arrow because of entry shrinking
            GtkWidget* parent=gtk_widget_get_parent(widget);
            if(parent) parent=gtk_widget_get_parent(parent);
            if( parent && std::string(G_OBJECT_TYPE_NAME(parent)) == "GimpScaleComboBox")
            { x--; }

        }

        const Gtk::Detail d( detail );

        QtSettings::ArrowSize arrowSize( QtSettings::ArrowNormal );
        if( d.isMenuItem() && Style::instance().settings().applicationName().isMozilla( widget ) )
        { arrowSize = QtSettings::ArrowTiny; }

        // define default color role
        Palette::Role role( Palette::ButtonText );

        // define options
        StyleOptions options( Contrast );
        options |= StyleOptions( widget, state );

        // Arrows which are active are painted as hovered
        if( state == GTK_STATE_ACTIVE ) options |= Hover;

        GtkWidget* parent( 0L );
        if( d.isTearOffMenuItem() )
        {
            if( widget &&
                gtk_widget_get_state( widget ) != GTK_STATE_PRELIGHT &&
                GTK_IS_MENU( gtk_widget_get_parent( widget ) ) &&
                gtk_menu_get_tearoff_state( GTK_MENU( gtk_widget_get_parent( widget ) ) ) )
            { Style::instance().renderWindowBackground( window, widget, clipRect, x-8, y-8, w+16, h+16); }

            // disable highlight in menus, for consistancy with oxygen qt style
            options &= ~( Focus|Hover );

        } else if( d.isMenuItem() && !Gtk::gtk_parent_tree_view( widget ) ) {

            // disable highlight in menus, for consistancy with oxygen qt style
            options &= ~( Focus|Hover );
            role = Palette::WindowText;

        } else if( d.isSpinButton() ) {


            if( Gtk::gtk_widget_layout_is_reversed( widget ) ) x+=1;
            else x-=1;

            if( arrow == GTK_ARROW_UP && !Style::instance().settings().applicationName().isOpenOffice() )
            {

                y+= 1;

            } else if( arrow == GTK_ARROW_DOWN ) y -= 1;

            // disable contrast
            options &= ~Contrast;

            role = Palette::Text;

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

            role = Palette::WindowText;

        } else if( Gtk::gtk_parent_combobox_entry( widget ) ) {

            if( state != GTK_STATE_INSENSITIVE ) options &= ~Contrast;
            role = Palette::Text;

            if( Gtk::gtk_widget_layout_is_reversed( widget ) )
            { x+=2; }

        } else if( Gtk::gtk_parent_combo( widget ) ) {

            role = Palette::WindowText;

            if( Gtk::gtk_widget_layout_is_reversed( widget ) )
            { x+=2; }

        } else if( ( parent = Gtk::gtk_parent_combobox( widget ) ) ) {

            options &= ~( Focus|Hover );
            role = Palette::ButtonText;

            if( Gtk::gtk_widget_layout_is_reversed( widget ) )
            { x+=2; }

        } else if(
            Gtk::gtk_parent_button( widget ) &&
            !Gtk::gtk_parent_tree_view( widget ) &&
            !Gtk::gtk_parent_combo( widget ) )
        {

            options &= ~( Focus|Hover );
            if( d.isArrow() && GTK_IS_ARROW( widget ) )
            {

                //if( arrow == GTK_ARROW_DOWN || arrow == GTK_ARROW_UP )
                { x += 1; }

                role = Palette::WindowText;
            }

        } else if( GTK_IS_CALENDAR( widget ) && !Gtk::gtk_widget_is_applet( widget ) ) {

            // need to render background behind arrows from calendar
            // offsets are empirical
            Style::instance().renderWindowBackground( window, widget, clipRect, x-2, y-3, w+4, h+6 );
            role = Palette::WindowText;

        } else if( GTK_IS_SCROLLBAR( widget ) ) {

            GtkSensitivityType lowerOld = gtk_range_get_lower_stepper_sensitivity( GTK_RANGE(widget) );
            GtkSensitivityType upperOld=gtk_range_get_upper_stepper_sensitivity( GTK_RANGE(widget) );
            GtkStateType widgetState=gtk_widget_get_state(widget);
            role = Palette::WindowText;

            if( ( lowerOld==GTK_SENSITIVITY_AUTO || lowerOld==GTK_SENSITIVITY_ON ) && widgetState==GTK_STATE_INSENSITIVE)
            { gtk_range_set_lower_stepper_sensitivity(GTK_RANGE(widget),GTK_SENSITIVITY_OFF); }

            if( ( lowerOld==GTK_SENSITIVITY_AUTO || lowerOld==GTK_SENSITIVITY_OFF ) && widgetState!=GTK_STATE_INSENSITIVE)
            { gtk_range_set_lower_stepper_sensitivity(GTK_RANGE(widget),GTK_SENSITIVITY_ON); }

            if( ( upperOld==GTK_SENSITIVITY_AUTO || upperOld==GTK_SENSITIVITY_ON ) && widgetState==GTK_STATE_INSENSITIVE)
            { gtk_range_set_lower_stepper_sensitivity(GTK_RANGE(widget),GTK_SENSITIVITY_OFF); }

            if( ( upperOld==GTK_SENSITIVITY_AUTO || upperOld==GTK_SENSITIVITY_OFF ) && widgetState!=GTK_STATE_INSENSITIVE)
            { gtk_range_set_upper_stepper_sensitivity(GTK_RANGE(widget),GTK_SENSITIVITY_ON); }

        }

        // render arrow
        Style::instance().renderArrow( window, clipRect, arrow, x, y, w, h, arrowSize, options, role );

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
            Gtk::TypeNames::state( state ),
            detail );
        #endif

        StyleOptions options( widget, state );
        const Gtk::Detail d( detail );
        const Palette::Role role( d.isTreeView() ? Palette::Text : Palette::WindowText );
        if( Style::instance().settings().viewDrawTriangularExpander() )
        {

            GtkArrowType arrow;
            const bool isExpanded( expander_style !=GTK_EXPANDER_COLLAPSED && expander_style != GTK_EXPANDER_SEMI_COLLAPSED );
            if( isExpanded ) arrow = GTK_ARROW_DOWN;
            else if( Gtk::gtk_widget_layout_is_reversed( widget ) ) arrow = GTK_ARROW_LEFT;
            else arrow = GTK_ARROW_RIGHT;

            const Gtk::Detail d( detail );
            QtSettings::ArrowSize arrowSize = QtSettings::ArrowNormal;
            if( d.isTreeView() ) arrowSize = Style::instance().settings().viewTriangularExpanderSize();
            else options |= Contrast;

            Style::instance().renderArrow( window, clipRect, arrow, x-3, y-4, 10, 10, arrowSize, options, role );

        } else {

            Style::instance().renderTreeExpander( window, clipRect, x-3, y-4, 10, 10, expander_style, options, role );

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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        StyleWrapper::parentClass()->draw_diamond( style, window, state,
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isOptionMenuTab() )
        {

            // render
            GtkArrowType arrow = GTK_ARROW_DOWN;
            QtSettings::ArrowSize arrowSize = QtSettings::ArrowNormal;
            StyleOptions options( Contrast );
            options |= StyleOptions( widget, state, shadow );

            // disable hover and focus
            options &= ~(Hover|Focus);

            Style::instance().renderArrow( window, clipRect, arrow, x, y, w, h, arrowSize, options );
            return;

        } else {

            StyleWrapper::parentClass()->draw_tab( style, window, state,
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isFrame() )
        {

            const Gtk::Gap gap( gap_x, gap_w, position );
            if( shadow == GTK_SHADOW_IN ) {

                Style::instance().renderHoleBackground( window, clipRect, x-1-Style::Entry_SideMargin, y-1, w+2+2*Style::Entry_SideMargin, h+2 );
                Style::instance().renderHole( window, clipRect, x-1, y-1, w+2, h+1, gap, NoFill );

            } else if( shadow == GTK_SHADOW_OUT ) {

                Style::instance().renderSlab( window, clipRect, x-1, y-4, w+2, h+4, gap, NoFill );

            } else if( shadow == GTK_SHADOW_ETCHED_IN || shadow == GTK_SHADOW_ETCHED_OUT ) {

                Style::instance().renderDockFrame( window, clipRect, x, y-1, w, h+1, gap, Blend );

            }

            return;

        } else {

            StyleWrapper::parentClass()->draw_shadow_gap( style, window, state,
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        const Gtk::Detail d( detail );
        if( d.isNotebook() )
        {

            // https://bugzilla.gnome.org/show_bug.cgi?id=635511
            if( !Style::instance().settings().applicationName().isMozilla( widget ) &&
                !Style::instance().settings().applicationName().isAcrobat( widget ) &&
                !Style::instance().settings().applicationName().isOpenOffice() )
            { Style::instance().windowManager().registerWidget( widget ); }

            // this might move to drawShadowGap
            StyleOptions options( NoFill );
            options |= StyleOptions( widget, GTK_STATE_NORMAL, shadow );
            options &= ~(Hover|Focus);

            if( Style::instance().settings().applicationName().isMozilla( widget ) )
            {

                Gtk::Gap gap( gap_x, gap_w, position );

                /*
                firefox calls box_gap twice. Once for the main frame, once for the actual gap
                the second call must be discarded somehow
                */
                if( h>12 )
                { Style::instance().renderSlab( window, clipRect, x, y-3, w, h-4, gap, options ); }

            } else {

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
                Style::instance().renderTabBarFrame( window, clipRect, x-1, y-1, w+2, h+2, gap, options );

            }

        } else {

            StyleWrapper::parentClass()->draw_box_gap( style, window, state,
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isScale() )
        {

            StyleOptions options( Blend );
            options |= StyleOptions( widget, state, shadow );
            options &= ~Sunken;
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

            StyleWrapper::parentClass()->draw_slider( style, window, state,
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail,
            Gtk::TypeNames::position( position ) );
        #endif

        Gtk::Detail d( detail );
        if( d.isTab() )
        {

            StyleOptions options( widget, state, shadow );
            TabOptions tabOptions( widget, state, position, x, y, w, h );

            const bool isCurrentTab( tabOptions & CurrentTab );
            bool drawTabBarBase( isCurrentTab );
            bool dragInProgress( false );

            /*
            see if tab is hovered. This is only done if widget is notebook, and if not running a mozilla
            (or open office) app, because the latter do not pass the actual tab rect as argument
            */
            const bool isMozilla( Style::instance().settings().applicationName().isMozilla( widget ) );
            const bool isOpenOffice( Style::instance().settings().applicationName().isOpenOffice() );
            if( GTK_IS_NOTEBOOK( widget ) && !( isMozilla || isOpenOffice ) )
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
                    bool drag( widget && (window != gtk_widget_get_window( widget ) ) );
                    Style::instance().animations().tabWidgetEngine().setDragInProgress( widget, drag );
                }

                dragInProgress = Style::instance().animations().tabWidgetEngine().dragInProgress( widget );

                // this does not work when the first tab is being grabbed
                if( dragInProgress )
                {
                    drawTabBarBase = ((tabOptions & FirstTab) && !isCurrentTab) ||
                        ((tabOptions & LastTab) && gtk_notebook_get_current_page( notebook ) == 0 );
                }

            }

            if( isOpenOffice )
            {
                // draw background since OOo won't draw it as it should
                // in addition, it passes wrong rectangle to the theme
                Style::instance().fill(
                    window, clipRect, x-1, y, w+2, h+1,
                    Style::instance().settings().palette().color( Palette::Window ) );
            }

            // render
            if( isMozilla ) tabOptions |= Mozilla;
            Style::instance().renderTab( window, clipRect, x, y, w, h, position, options, tabOptions );

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

                Style::instance().renderTabBarBase( window, clipRect, xBase-1, yBase-1, wBase+2, hBase+2, position, gap, options, tabOptions );

            }

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
            Gtk::TypeNames::state( state ),
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
            Gtk::TypeNames::state( state ),
            Gtk::TypeNames::shadow( shadow ),
            detail );
        #endif

        Gtk::Detail d( detail );
        if( d.isPaned() )
        {

            StyleOptions options( widget, state, shadow );
            if( GTK_IS_VPANED( widget ) )
            {
                options |= Vertical;

            } else if( Gtk::g_object_is_a( G_OBJECT( widget ), "GtkPizza" ) ) {

                Style::instance().renderWindowBackground( window, widget, clipRect, x, y, w, h );
                if( w>h ) options |= Vertical;

            }

            Style::instance().renderSplitter( window, 0L, x, y, w, h, options );

        } else if( d.isHandleBox() ) {

            if( Style::instance().settings().applicationName().isOpenOffice() )
            {

                Style::instance().fill( window, clipRect, x, y, w, h, Gtk::gdk_get_color( style->bg[state] ) );

            } else if( !Gtk::gtk_widget_is_applet( widget ) ) {

                Style::instance().renderWindowBackground( window, widget, clipRect, x, y, w, h );

            }

            StyleOptions options( widget, state, shadow );
            if( orientation == GTK_ORIENTATION_VERTICAL ) options |= Vertical;
            Style::instance().renderToolBarHandle( window, clipRect, x, y, w, h, options );

        } else {

            StyleWrapper::parentClass()->draw_handle( style, window, state,
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
            Gtk::TypeNames::state( state ),
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
            Gtk::TypeNames::state( state ));
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

        // non-flat pushbuttons don't have any icon effect
        const bool useEffect( Style::instance().settings().useIconEffect() && Gtk::gtk_button_is_flat( Gtk::gtk_parent_button( widget ) ) );

        if( gtk_icon_source_get_state_wildcarded( source ) )
        {

            if( state == GTK_STATE_INSENSITIVE )
            {

                stated = Gtk::gdk_pixbuf_set_alpha( scaled, 0.3 );
                gdk_pixbuf_saturate_and_pixelate( stated, stated, 0.1, false );
                g_object_unref (scaled);

            } else if( useEffect && state == GTK_STATE_PRELIGHT ) {

                stated = gdk_pixbuf_copy( scaled );
                if(!Gtk::gdk_pixbuf_to_gamma( stated, 0.7 ) )
                {
                    // FIXME: correct the value to match KDE
                    /*
                    in fact KDE allows one to set many different effects on icon
                    not sure we want to copy this code all over the place, especially since nobody changes the default settings,
                    as far as I know */
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
            Gtk::TypeNames::state( state ),
            detail );
        #endif

        const Gtk::Detail d(detail);

        // draw progressbar text white if above indicator, black if not
        if( GTK_IS_PROGRESS( widget ) || GTK_IS_PROGRESS_BAR( widget ) || d.isProgressBar() )
        {
            Cairo::Context context( window,clipRect );
            if(state==GTK_STATE_PRELIGHT) gdk_cairo_set_source_color(context, &style->text[GTK_STATE_SELECTED]);
            else gdk_cairo_set_source_color(context, &style->text[state]);
            cairo_translate(context,x,y);
            pango_cairo_show_layout(context,layout);
            return;
        }

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
            StyleWrapper::parentClass()->draw_layout(
                style, window, state, use_text,
                clipRect, widget, detail, x, y, layout );

        }
    }

    //_______________________________________________________________________________________________________________
    void StyleWrapper::instanceInit( OxygenStyle* self )
    {

        // initialize window manager hook
        Style::instance().windowManager().initializeHooks();

        // initialize argb hooks
        if(
            Style::instance().settings().argbEnabled() &&
            !Style::instance().settings().applicationName().isMozilla() )
        { Style::instance().argbHelper().initializeHooks(); }

    }

    //_______________________________________________________________________________________________________________
    void StyleWrapper::classInit( OxygenStyleClass* klass )
    {

        GtkStyleClass* style_class( GTK_STYLE_CLASS( klass ) );

        _parentClass = static_cast<GtkStyleClass*>( g_type_class_peek_parent( klass ) );

        style_class->draw_hline = draw_hline;
        style_class->draw_vline = draw_vline;
        style_class->draw_shadow = draw_shadow;
        style_class->draw_arrow = draw_arrow;

        style_class->draw_diamond = draw_diamond;

        style_class->draw_box = draw_box;
        style_class->draw_flat_box = draw_flat_box;
        style_class->draw_check = draw_check;
        style_class->draw_option = draw_option;
        style_class->draw_tab = draw_tab;
        style_class->draw_shadow_gap = draw_shadow_gap;
        style_class->draw_box_gap = draw_box_gap;
        style_class->draw_extension = draw_extension;
        style_class->draw_focus = draw_focus;
        style_class->draw_slider = draw_slider;
        style_class->draw_handle = draw_handle;
        style_class->draw_resize_grip = draw_resize_grip;
        style_class->draw_expander = draw_expander;

        // icon rendering
        style_class->render_icon = render_icon;

        // text rendering
        style_class->draw_layout = draw_layout;

    }

    //_______________________________________________________________________________________________________________
    void StyleWrapper::registerType( GTypeModule* module )
    {
        if( !_type )
        {

            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::StyleWrapper::registerType" << std::endl;
            #endif

            const GTypeInfo info =
            {
                (guint16)sizeof( OxygenStyleClass ),
                (GBaseInitFunc) NULL,
                (GBaseFinalizeFunc) NULL,
                (GClassInitFunc) classInit,
                (GClassFinalizeFunc) NULL,
                NULL,
                (guint16)sizeof( OxygenStyle ),
                0,
                (GInstanceInitFunc) instanceInit,
                NULL
            };

            _typeInfo = info;
            _type = g_type_module_register_type( module, GTK_TYPE_STYLE, "OxygenStyle", &_typeInfo, GTypeFlags(0 ) );
        }
    }

    //_______________________________________________________________________________________________________________
    GType StyleWrapper::type( void )
    { return _type; }

}
