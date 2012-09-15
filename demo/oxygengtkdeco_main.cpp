/*
* this file is part of the oxygen gtk engine
* Copyright (c) 2010 Hugo Pereira Da Costa <hugo@oxygen-icons.org>
* Copyright (c) 2010 Ruslan Kabatsayev <b7.10110111@gmail.com>
*
* based on the Null Theme Engine for Gtk+.
* Copyright (c) 2008 Robert Staudinger <robert.staudinger@gmail.com>
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

#include <gtk/gtk.h>
#include <dlfcn.h>
#include <string>
#include "oxygen_version.h"

enum WinDecoOptions
{
    WinIsMaximized=1<<0,
    WinIsShaded=1<<2,
    WinIsResizable=1<<3,
    WinIsActive=1<<4,
    WinHasAlpha=1<<5,
    DrawAlphaChannel=1<<6,
};

//! button status
enum ButtonStatus
{
    Normal,
    Disabled, // this shouldn't be specified by WMs unless button is really insensitive
    Hovered,
    Pressed,
    ButtonStatusCount
};

//! buttons
enum ButtonType
{
    ButtonHelp=0,
    ButtonMax,
    ButtonMin,
    ButtonClose,
    ButtonMenu,
    ButtonSticky,
    ButtonAbove,
    ButtonBelow,
    ButtonShade,
    ButtonUnmax,
    ButtonUnstick,
    ButtonUnshade,
    ButtonUndoAbove,
    ButtonUndoBelow,
    ButtonTypeCount
};

enum Metric
{
    BorderLeft=0,
    BorderRight,
    BorderBottom,
    // BorderTop includes title and resize handle heights
    BorderTop,
    ButtonSpacing,
    ButtonMarginTop,
    ButtonMarginBottom,
    ShadowLeft,
    ShadowTop,
    ShadowRight,
    ShadowBottom,
    MetricsCount
};

//___________________________________________________________________
// external pointers to functions
void (*drawWindowDecoration)(cairo_t*, unsigned long, gint, gint, gint, gint, const gchar**, gint, gint) = 0L;
void (*drawWindecoButton)(cairo_t*, unsigned long, unsigned long, unsigned long, gint, gint, gint, gint) = 0L;
void (*drawWindecoShapeMask)(cairo_t*, unsigned long, gint, gint, gint, gint) = 0L;
void (*drawWindowShadow)(cairo_t*, unsigned long, gint, gint, gint, gint) = 0L;
gint (*getWindecoABIVersion)(void) = 0L;
gint (*getWindecoMetric)(unsigned long) = 0L;
gint (*getWindecoButtonSize)(unsigned long) = 0L;

// window dimensions
int ww( 700 );
int wh( 200 );

// widgets
GtkWidget *mw0( 0L );
GtkWidget *mw1( 0L );

const gboolean useAlpha( TRUE );

// windeco metrics
int shadowLeft=0;
int shadowRight=0;
int shadowTop=0;
int shadowBottom=0;
int borderLeft=0;
int borderRight=0;
int borderTop=0;
int borderBottom=0;
int dw=0;
int dh=0;

//___________________________________________________________________
gboolean initLib()
{
    void* library;
    char* error=0;
    char* moduleDir=gtk_rc_get_module_dir();
    if(moduleDir)
    {
        std::string libpath(moduleDir+std::string("/liboxygen-gtk.so"));
        g_free(moduleDir);
        library=dlopen(libpath.c_str(),RTLD_LAZY);
        bool success=false;
        do{
            if(!library)
            {
                break;
            } else {

                getWindecoABIVersion=(gint(*)(void))dlsym(library, "getWindecoABIVersion");
                if((error=dlerror())!=0L)
                {
                    break;
                } else {

                    #define EXPECTED_ABI_VERSION 3
                    gint version=getWindecoABIVersion();
                    if(version != EXPECTED_ABI_VERSION)
                    {
                        fprintf(stderr, "ABI version %d is not equal to expected version %d\n", version, EXPECTED_ABI_VERSION);
                        return FALSE;
                    }

                }

                // store drawWindowDecoration symbol
                drawWindowDecoration = (void(*)(cairo_t*, unsigned long, gint, gint, gint, gint, const gchar**, gint, gint))dlsym(library, "drawWindowDecoration");
                if((error=dlerror())!=0L)
                    break;

                // store drawWindecoButton symbol
                drawWindecoButton = (void (*)(cairo_t*, unsigned long, unsigned long, unsigned long, gint, gint, gint, gint))dlsym(library, "drawWindecoButton");
                if((error=dlerror())!=0L)
                    break;

                // store drawWindecoShapeMask symbol
                drawWindecoShapeMask=(void (*)(cairo_t*, unsigned long, gint, gint, gint, gint))dlsym(library, "drawWindecoShapeMask");
                if((error=dlerror())!=0L)
                    break;

                // store drawWindowShadow symbol
                drawWindowShadow=(void (*)(cairo_t*, unsigned long, gint, gint, gint, gint))dlsym(library, "drawWindowShadow");
                if((error=dlerror())!=0L)
                    break;

                // store drawWindecoMetric symbol
                getWindecoMetric=(gint (*)(unsigned long))dlsym(library, "getWindecoMetric");
                if((error=dlerror())!=0L)
                    break;

                // store drawWindecoButtonSize symbol
                getWindecoButtonSize=(gint (*)(unsigned long))dlsym(library, "getWindecoButtonSize");
                if((error=dlerror())!=0L)
                    break;
              }
        }
        while(0);
        if(error)
        {
            fprintf(stderr, "%s\n", error);
            return FALSE;
        }
    }
    return TRUE;
}

void getMetrics()
{
    shadowLeft=getWindecoMetric(ShadowLeft);
    shadowRight=getWindecoMetric(ShadowRight);
    shadowTop=getWindecoMetric(ShadowTop);
    shadowBottom=getWindecoMetric(ShadowBottom);

    borderLeft=getWindecoMetric(BorderLeft);
    borderRight=getWindecoMetric(BorderRight);
    borderTop=getWindecoMetric(BorderTop);
    borderBottom=getWindecoMetric(BorderBottom);

    dw = borderLeft + borderRight + shadowLeft + shadowRight;
    dh = borderTop + borderBottom + shadowTop + shadowBottom;
}

//___________________________________________________________________
gboolean on_expose(GtkWidget* mw0, GdkEventExpose* event, gpointer user_data)
{
    cairo_t* cr=gdk_cairo_create( gtk_widget_get_window( mw0 ) );

    // define options
    int opt( WinIsResizable|DrawAlphaChannel );
    if( useAlpha ) opt |= WinHasAlpha;
    if( !gtk_window_is_active( GTK_WINDOW(mw1) ) ) opt|= WinIsActive;

    drawWindowShadow(cr, opt, 0, 0, mw0->allocation.width, mw0->allocation.height);

    const gchar* windowStrings[] = {
        "This is a caption",
        "WindowClass10110111",
        0 };

    drawWindowDecoration(cr, opt, 0+shadowLeft, 0+shadowTop, mw0->allocation.width-shadowLeft-shadowRight, mw0->allocation.height-shadowTop-shadowBottom, windowStrings, 0, 20*ButtonTypeCount);

    for( int status=0; status<ButtonStatusCount; status++)
    {
        for( int type=0; type<ButtonTypeCount; type++)
        {
            int buttonSize=getWindecoButtonSize(type);
            int buttonSpacing=getWindecoMetric(ButtonSpacing);
            int dbut=buttonSize+buttonSpacing;
            drawWindecoButton(cr, type, status, opt, 
                    mw0->allocation.width-shadowRight-borderRight-buttonSize-buttonSize*type,
                    status*dbut+shadowTop+(borderTop-buttonSize)/2,
                    buttonSize, buttonSize);
        }
    }

    cairo_destroy(cr);
    return TRUE;
}

//___________________________________________________________________
gboolean on_configure1(GtkWidget* mw1, GdkEventConfigure* event, gpointer user_data)
{
    static int w=0, h=0;
    gboolean redraw( event->width != w || event->height != h );
    if( redraw  )
    {
        w=event->width;
        h=event->height;

        int opt( WinIsActive|WinIsResizable );
        if( useAlpha ) opt|= WinHasAlpha;

        gtk_window_resize(GTK_WINDOW(mw0), event->width+dw, event->height+dh);

        if(!useAlpha)
        {
            GdkBitmap* mask=gdk_pixmap_new(0L, event->width+dw, event->height+dh, 1);
            cairo_t* cr=gdk_cairo_create(mask);
            drawWindecoShapeMask(cr, opt, 0+shadowLeft, 0+shadowRight, event->width+dw-shadowLeft-shadowRight, event->height+dh-shadowTop-shadowBottom);
            gdk_window_shape_combine_mask( gtk_widget_get_window( mw0 ), 0L, 0, 0 ); // remove old mask
            gdk_window_shape_combine_mask( gtk_widget_get_window( mw0 ), mask, 0, 0 ); // apply new mask
            gdk_pixmap_unref(mask);
        }
    }

    return FALSE;

}

//___________________________________________________________________
gboolean on_configure0(GtkWidget* mw0, GdkEventConfigure* event, gpointer user_data)
{
    static int w=0, h=0;
    static gboolean active=FALSE;

    gboolean redraw( event->width != w || event->height != h );
    if(!(gtk_window_is_active(GTK_WINDOW(mw1))!=active))
    {
        active ^= TRUE;
        redraw = TRUE;
    }

    if( redraw )
    {
        w=event->width;
        h=event->height;

        gdk_window_begin_paint_rect( gtk_widget_get_window( mw0 ), (GdkRectangle*)&mw0->allocation);
        on_expose(mw0, 0L, 0L );
        gdk_window_end_paint( gtk_widget_get_window( mw0 ) );
    }

    return FALSE;

}

//___________________________________________________________________
gboolean on_press0(GtkWindow* window, GdkEventButton* event, GdkWindowEdge edge)
{
    if(event->type == GDK_BUTTON_PRESS)
        if(event->button == 1)
            gtk_window_begin_move_drag(window,event->button,event->x_root,event->y_root,event->time);
    return FALSE;
}

//___________________________________________________________________
int main(int argc, char** argv)
{

    // load methods from style library
    if(!initLib()) return 1;

    // initialize gtk
    gtk_init(&argc, &argv);

    processCommandLine(argc,argv);

    // draw
    mw0=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    mw1=gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GdkColormap* cmap=gdk_screen_get_rgba_colormap(gdk_screen_get_default());
    gtk_widget_set_colormap(mw0, cmap);

    g_signal_connect(G_OBJECT(mw0), "destroy", G_CALLBACK(gtk_main_quit), 0L);
    g_signal_connect(G_OBJECT(mw1), "destroy", G_CALLBACK(gtk_main_quit), 0L);

    gtk_window_set_default_size( GTK_WINDOW(mw1), ww, wh );
    gtk_window_set_default_size( GTK_WINDOW(mw0), ww+dw, wh+dh );
    gtk_window_set_decorated(GTK_WINDOW(mw0), FALSE);
    gtk_widget_add_events(mw0,GDK_BUTTON_PRESS_MASK);

    gtk_window_set_title( GTK_WINDOW(mw1), "This is a caption");

    g_signal_connect( G_OBJECT(mw0), "expose-event", G_CALLBACK(on_expose), 0L);
    g_signal_connect( G_OBJECT(mw1), "configure-event", G_CALLBACK(on_configure1), 0L);
    g_signal_connect( G_OBJECT(mw0), "configure-event", G_CALLBACK(on_configure0), 0L);
    g_signal_connect( G_OBJECT(mw0), "button-press-event", G_CALLBACK(on_press0), NULL);

    gtk_widget_show( mw1 );
    gtk_widget_show( mw0 );

    gtk_window_move( GTK_WINDOW(mw0), 300, 320);
    gtk_window_move( GTK_WINDOW(mw1), 300, 320);

    // FIXME: this call crashes in Oxygen::StyleHelper::initializeRefSurface()
    // if done right after gtk_init() call, i.e. until any widget is created
    getMetrics();

    gtk_main();
    return 0;
}
