/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* This  library is free  software; you can  redistribute it and/or
* modify it  under  the terms  of the  GNU Lesser  General  Public
* License  as published  by the Free  Software  Foundation; either
* version 2 of the License, or(at your option ) any later version.
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

#include <gtk/gtk.h>
#include "oxygeninnershadowdata.h"
#include "../oxygengtkutils.h"
#include "../config.h"
#include "../oxygencairocontext.h"
#include "oxygenanimations.h"
#include "../oxygenstyle.h"

#include <cassert>
#include <iostream>

namespace Oxygen
{

    //_________________________________________________________________________________________
    gboolean InnerShadowData::targetExposeEvent( GtkWidget* widget, GdkEventExpose* event, gpointer )
    {

        GtkWidget* child=gtk_bin_get_child(GTK_BIN(widget));
        GdkWindow* window=gtk_widget_get_window(child);

        #if OXYGEN_DEBUG
        std::cerr << "InnerShadowData::targetExposeEvent( " << G_OBJECT_TYPE_NAME(widget) << " ); child: " ;
        char* path;
        gtk_widget_path(child,NULL,&path,NULL);
        std::cerr << path << std::endl;
        g_free(path);
        #endif

        // don't do anything if the window isn't composited
        if(!gdk_window_get_composited(window))
        {
            #if OXYGEN_DEBUG
            std::cerr << "Window isn't composited, so not doing anything\n";
            #endif
            return FALSE;
        }
        Cairo::Context context(gtk_widget_get_window(widget));

        // set up clipping independently of GTK version
        GtkAllocation alloc;
        gtk_widget_get_allocation(child,&alloc);
        cairo_rectangle(context,alloc.x,alloc.y,alloc.width,alloc.height);
        cairo_clip(context);
        gdk_cairo_region(context,event->region);
        cairo_clip(context);

        // first make sure the child window doesn't contain garbage
        gdk_window_process_updates(window,TRUE);

        // now draw the child
        gdk_cairo_set_source_window(context,window,alloc.x,alloc.y);
        cairo_paint(context);

        // draw the shadow

        int basicOffset=2;
        // we only draw SHADOW_IN here
        if(gtk_scrolled_window_get_shadow_type(GTK_SCROLLED_WINDOW(widget)) != GTK_SHADOW_IN )
        {
            if( GTK_IS_VIEWPORT(child) && gtk_viewport_get_shadow_type(GTK_VIEWPORT(child)) == GTK_SHADOW_IN )
            {
                basicOffset=0;
            }
            else
            {
                #if OXYGEN_DEBUG
                std::cerr << "Shadow type isn't GTK_SHADOW_IN, so not drawing the shadow in expose-event handler\n";
                #endif
                return FALSE;
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

        int offsetX=basicOffset+Style::Entry_SideMargin;
        int offsetY=basicOffset;

        GdkRectangle clipRect={alloc.x,alloc.y,alloc.width,alloc.height};

        Style::instance().renderHoleBackground( gtk_widget_get_window(widget), &clipRect, alloc.x-offsetX, alloc.y-offsetY, alloc.width+offsetX*2, alloc.height+offsetY*2 );
        offsetX-=Style::Entry_SideMargin;
        Style::instance().renderHole( gtk_widget_get_window(widget), &clipRect, alloc.x-offsetX, alloc.y-offsetY, alloc.width+offsetX*2, alloc.height+offsetY*2, options, data );

        // let the event propagate
        return FALSE;
    }

    //_____________________________________________
    void InnerShadowData::connect( GtkWidget* widget )
    {

        assert( GTK_IS_SCROLLED_WINDOW( widget ) );
        assert( !_target );

        // store target
        _target = widget;

        if(gdk_display_supports_composite(gdk_display_get_default()))
        {
            _compositeEnabled=true;
            _exposeId.connect( G_OBJECT(_target), "expose-event", G_CALLBACK( targetExposeEvent ), this, true );
        }

        // check child
        GtkWidget* child( gtk_bin_get_child( GTK_BIN( widget ) ) );
        if( !child ) return;

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::connect -"
            << " child: " << child << " (" << G_OBJECT_TYPE_NAME( child ) << ")"
            << std::endl;
        #endif

        registerChild( child );

    }

    //_____________________________________________
    void InnerShadowData::disconnect( GtkWidget* widget )
    {
        _target = 0;
        for( ChildDataMap::iterator iter = _childrenData.begin(); iter != _childrenData.end(); ++iter )
        { iter->second.disconnect( iter->first ); }

        if(_compositeEnabled)
        {
            _exposeId.disconnect();
        }

        _childrenData.clear();
    }

    //_____________________________________________
    void InnerShadowData::registerChild( GtkWidget* widget )
    {

        // make sure widget is not already in map
        if( _childrenData.find( widget ) == _childrenData.end() )
        {

            #if OXYGEN_DEBUG
            std::cerr
                << "Oxygen::InnerShadowData::registerChild -"
                << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
                << std::endl;
            #endif

            ChildData data;
            data._unrealizeId.connect( G_OBJECT(widget), "unrealize", G_CALLBACK( childUnrealizeNotifyEvent ), this );

            GdkWindow* window(gtk_widget_get_window(widget));
            if(window && gdk_display_supports_composite(gdk_display_get_default()))
            {
                data.initiallyComposited=gdk_window_get_composited(window);
                gdk_window_set_composited(window,TRUE);
            }

            _childrenData.insert( std::make_pair( widget, data ) );

        }

    }

    //________________________________________________________________________________
    void InnerShadowData::unregisterChild( GtkWidget* widget )
    {

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

    }

    //________________________________________________________________________________
    void InnerShadowData::ChildData::disconnect( GtkWidget* widget )
    {

        #if OXYGEN_DEBUG
        std::cerr
            << "Oxygen::InnerShadowData::ChildData::disconnect -"
            << " " << widget << " (" << G_OBJECT_TYPE_NAME( widget ) << ")"
            << std::endl;
        #endif

        _unrealizeId.disconnect();
        GdkWindow* window(gtk_widget_get_window(widget));
        if(GTK_IS_WINDOW(window))
            gdk_window_set_composited(window,initiallyComposited);
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

}
