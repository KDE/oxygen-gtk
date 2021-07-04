/*
    this file is part of the oxygen gtk engine
    SPDX-FileCopyrightText: 2010 Hugo Pereira Da Costa <hugo.pereira@free.fr>
    SPDX-FileCopyrightText: 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "oxygeninnershadowdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"
#include "../oxygencairocontext.h"
#include "../oxygencairoutils.h"
#include "oxygenanimations.h"
#include "../oxygenstyle.h"
#include "../oxygenmetrics.h"

#include <gtk/gtk.h>
#include <cstdlib>

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //_____________________________________________
    void InnerShadowData::connect( GtkWidget* widget )
    {

        assert( GTK_IS_SCROLLED_WINDOW( widget ) );
        assert( !_target );

        // store target
        _target = widget;

        if( gdk_display_supports_composite( gtk_widget_get_display( widget ) ) )
        { _exposeId.connect( G_OBJECT(_target), "expose-event", G_CALLBACK( targetExposeEvent ), this, true ); }

        // check child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        if( !child ) return;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::connect -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " child: " << child << " (" << G_OBJECT_TYPE_NAME( child ) << ")"
            << std::endl;
        #endif

        registerChild( child );

    }

    //_____________________________________________
    void InnerShadowData::disconnect( GtkWidget* )
    {
        _target = 0;
        for( ChildDataMap::reverse_iterator iter = _childrenData.rbegin(); iter != _childrenData.rend(); ++iter )
        { iter->second.disconnect( iter->first ); }

        // disconnect signals
        _exposeId.disconnect();

        // clear child data
        _childrenData.clear();
    }

    //_____________________________________________
    void InnerShadowData::registerChild( GtkWidget* widget )
    {

        #if ENABLE_INNER_SHADOWS_HACK

        // make sure widget is not already in map
        if( _childrenData.find( widget ) != _childrenData.end() ) return;

        if( gtk_scrolled_window_get_shadow_type( GTK_SCROLLED_WINDOW( _target ) ) != GTK_SHADOW_IN )
        { return; }

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::registerChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        GdkWindow* window(gtk_widget_get_window(widget));
        if(

            // check window
            window &&
            gdk_window_get_window_type( window ) == GDK_WINDOW_CHILD &&

            // check compositing
            gdk_display_supports_composite( gtk_widget_get_display( widget ) ) &&

            // make sure widget is scrollable
            GTK_WIDGET_GET_CLASS( widget )->set_scroll_adjustments_signal )
        {
            ChildData data;
            data._unrealizeId.connect( G_OBJECT(widget), "unrealize", G_CALLBACK( childUnrealizeNotifyEvent ), this );
            data._initiallyComposited = gdk_window_get_composited(window);
            gdk_window_set_composited(window,TRUE);
            _childrenData.insert( std::make_pair( widget, data ) );
        }

        #endif

    }

    //________________________________________________________________________________
    void InnerShadowData::unregisterChild( GtkWidget* widget )
    {
        #if ENABLE_INNER_SHADOWS_HACK

        ChildDataMap::iterator iter( _childrenData.find( widget ) );
        if( iter == _childrenData.end() ) return;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::unregisterChild -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        iter->second.disconnect( widget );
        _childrenData.erase( iter );

        #endif
    }

    //________________________________________________________________________________
    void InnerShadowData::ChildData::disconnect( GtkWidget* widget )
    {
        #if ENABLE_INNER_SHADOWS_HACK

        // disconnect signals
        _unrealizeId.disconnect();

        // remove compositing flag
        GdkWindow* window( gtk_widget_get_window( widget ) );

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::ChildData::disconnect -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << " window: " << window
            << std::endl;
        #endif

        // restore compositing if different from initial state
        if( GDK_IS_WINDOW( window ) && !gdk_window_is_destroyed(window) && gdk_window_get_composited( window ) != _initiallyComposited )
        { gdk_window_set_composited( window, _initiallyComposited ); }

        #endif
    }

    //____________________________________________________________________________________________
    gboolean InnerShadowData::childUnrealizeNotifyEvent( GtkWidget* widget, gpointer data )
    {
        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::childUnrealizeNotifyEvent -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif
        static_cast<InnerShadowData*>(data)->unregisterChild( widget );
        return FALSE;
    }

    //_________________________________________________________________________________________
    gboolean InnerShadowData::targetExposeEvent( GtkWidget* widget, GdkEventExpose* event, gpointer )
    {

        #if ENABLE_INNER_SHADOWS_HACK
        GtkWidget* child=gtk_bin_get_child(GTK_BIN(widget));
        GdkWindow* window=gtk_widget_get_window(child);

        #if OXYGEN_DEBUG
        std::cerr << "Oxygen::InnerShadowData::targetExposeEvent -"
            << " widget: " << widget << " (" << G_OBJECT_TYPE_NAME(widget) << ")"
            << " child: " << child << " (" << G_OBJECT_TYPE_NAME(child) << ")"
            << " path: " << Gtk::gtk_widget_path( child )
            << " area: " << event->area
            << std::endl;
        #endif

        if(!gdk_window_get_composited(window))
        {
            #if OXYGEN_DEBUG
            std::cerr << "Oxygen::InnerShadowData::targetExposeEvent - Window isn't composite. Doing nohing\n";
            #endif
            return FALSE;
        }

        // make sure the child window doesn't contain garbage
        gdk_window_process_updates(window,TRUE);

        // get window geometry
        GtkAllocation allocation( Gtk::gdk_rectangle() );
        gdk_window_get_geometry( window, &allocation.x, &allocation.y, &allocation.width, &allocation.height, 0L );

        // create context with clipping
        Cairo::Context context(gtk_widget_get_window(widget), &allocation );

        // add event region
        gdk_cairo_region(context,event->region);
        cairo_clip(context);

        // draw child
        gdk_cairo_set_source_window( context, window, allocation.x, allocation.y );
        cairo_paint(context);

        #if OXYGEN_DEBUG_INNERSHADOWS
        // Show updated parts in random color
        cairo_rectangle(context,allocation.x,allocation.y,allocation.width,allocation.height);
        double red=((double)rand())/RAND_MAX;
        double green=((double)rand())/RAND_MAX;
        double blue=((double)rand())/RAND_MAX;
        cairo_set_source_rgba(context,red,green,blue,0.5);
        cairo_fill(context);
        #endif

        // Render rounded combobox list child
        if(Gtk::gtk_combobox_is_tree_view( child ))
        {
            StyleOptions options(widget,gtk_widget_get_state(widget));
            Corners corners(CornersAll);
            if(gtk_widget_get_visible(gtk_scrolled_window_get_vscrollbar(GTK_SCROLLED_WINDOW(widget))))
            {
                if(Gtk::gtk_widget_layout_is_reversed( widget ))
                    corners &= ~CornersLeft;
                else
                    corners &= ~CornersRight;
            }
            if(gtk_widget_get_visible(gtk_scrolled_window_get_hscrollbar(GTK_SCROLLED_WINDOW(widget))))
                corners &= ~CornersBottom;

            int x(allocation.x),y(allocation.y),w(allocation.width),h(allocation.height);
            cairo_rectangle(context,x,y,w,h);
            if(!Gtk::gdk_default_screen_is_composited())
            {
                // Take ugly shadow into account
                x+=1;
                y+=1;
                w-=2;
                h-=2;
            }
            cairo_rounded_rectangle_negative(context,x,y,w,h,2,corners);
            cairo_clip(context);

            Style::instance().renderMenuBackground( gtk_widget_get_window(widget), context, allocation.x,allocation.y,allocation.width,allocation.height, options );

            // Event handling finished, now let the event propagate
            return FALSE;
        }

        // draw the shadow
        /*
        TODO: here child widget's allocation is used instead of window geometry.
        I think this is the correct thing to do (unlike above), but this is to be double check
        */
        allocation = Gtk::gtk_widget_get_allocation( child );
        int basicOffset=2;

        // we only draw SHADOW_IN here
        if( gtk_scrolled_window_get_shadow_type( GTK_SCROLLED_WINDOW( widget ) ) != GTK_SHADOW_IN )
        {
            if( GTK_IS_VIEWPORT(child) && gtk_viewport_get_shadow_type(GTK_VIEWPORT(child)) == GTK_SHADOW_IN )
            {

                basicOffset=0;

            } else {

                // FIXME: do we need this special case?
                // special_case {
                // we still want to draw shadow on GtkFrames with shadow containing GtkScrolledWindow without shadow
                GtkWidget* box=gtk_widget_get_parent(widget);
                GtkWidget* frame=0;
                if(GTK_IS_BOX(box) && GTK_IS_FRAME(frame=gtk_widget_get_parent(box)) &&
                       gtk_frame_get_shadow_type(GTK_FRAME(frame))==GTK_SHADOW_IN)
                {
                    #if OXYGEN_DEBUG
                    std::cerr << "Oxygen::InnerShadowData::targetExposeEvent: Box children: " << GTK_CONTAINER(box) << std::endl;
                    #endif
                    // make sure GtkScrolledWindow is the only visible child
                    GList* children=gtk_container_get_children(GTK_CONTAINER(box));
                    for(GList* child=g_list_first(children); child; child=g_list_next(child))
                    {
                        GtkWidget* childWidget(GTK_WIDGET(child->data));
                        if(gtk_widget_get_visible(childWidget) && !GTK_IS_SCROLLED_WINDOW(childWidget))
                        {
                            g_list_free(children);
                            return FALSE;
                        }
                    }
                    int frameX, frameY;
                    GtkAllocation frameAlloc;
                    if(gtk_widget_translate_coordinates(frame,widget,0,0,&frameX,&frameY))
                    {
                        #if OXYGEN_DEBUG
                        std::cerr << "coords translation: x=" << frameX << "; y=" << frameY << std::endl;
                        #endif
                        gtk_widget_get_allocation(frame,&frameAlloc);
                        allocation.x+=frameX;
                        allocation.y+=frameY;
                        allocation.width=frameAlloc.width;
                        allocation.height=frameAlloc.height;
                        basicOffset=0;
                    }

                } else {

                    #if OXYGEN_DEBUG
                    std::cerr << "Oxygen::InnerShadowData::targetExposeEvent - Shadow type isn't GTK_SHADOW_IN, so not drawing the shadow in expose-event handler\n";
                    #endif
                    return FALSE;

                }

            }
        }

        StyleOptions options(widget,gtk_widget_get_state(widget));
        options|=NoFill;
        options &= ~(Hover|Focus);
        if( Style::instance().animations().scrolledWindowEngine().contains( widget ) )
        {
            if( Style::instance().animations().scrolledWindowEngine().focused( widget ) ) options |= Focus;
            if( Style::instance().animations().scrolledWindowEngine().hovered( widget ) ) options |= Hover;
        }

        const AnimationData data( Style::instance().animations().widgetStateEngine().get( widget, options, AnimationHover|AnimationFocus, AnimationFocus ) );

        int offsetX=basicOffset+Entry_SideMargin;
        int offsetY=basicOffset;

        // clipRect
        GdkRectangle clipRect( allocation );

        // hole background
        Style::instance().renderHoleBackground(
            gtk_widget_get_window(widget), widget, &clipRect,
            allocation.x-offsetX, allocation.y-offsetY, allocation.width+offsetX*2, allocation.height+offsetY*2 );

        // adjust offset and render hole
        offsetX -= Entry_SideMargin;
        Style::instance().renderHole(
            gtk_widget_get_window(widget), &clipRect,
            allocation.x-offsetX, allocation.y-offsetY, allocation.width+offsetX*2, allocation.height+offsetY*2,
            options, data );

        #endif // enable inner shadows hack

        // let the event propagate
        return FALSE;
    }

}
