#include <gtk/gtk.h>
#include <dlfcn.h>

typedef enum
{
    WinIsMaximized=1<<0,
    WinIsShaded=1<<2,
    WinIsResizable=1<<3,
    WinIsActive=1<<4,
    WinHasAlpha=1<<5,
}WinDecoOptions;

//! button status
typedef enum {
    Normal,
    Disabled, // this shouldn't be specified by WMs unless button is really insensitive
    Hovered,
    Pressed,
    ButtonStatusCount
}ButtonStatus;

//! buttons
typedef enum {
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
}ButtonType;

void (*drawWindowDecoration)(cairo_t*,WinDecoOptions,gint,gint,gint,gint, gchar**,gint,gint)=0;
void (*drawWindecoButton)(cairo_t*,ButtonType,ButtonStatus,WinDecoOptions,gint,gint,gint,gint)=0;
void (*drawWindecoShapeMask)(cairo_t*,WinDecoOptions,gint,gint,gint,gint)=0;
void (*drawWindowShadow)(cairo_t*,WinDecoOptions,gint,gint,gint,gint)=0;
gint (*getWindecoABIVersion)(void)=0;
static int ww=700,wh=200;
GtkWidget *mw0,*mw1;

const gboolean useAlpha=TRUE;

#define realShadowSize 29
#define shadowSize (realShadowSize-4)
// for now these metrics are hard-coded
const int dw=4+4+shadowSize*2; // left border + right one
const int dh=23+4+shadowSize*2; // title + bottom

gboolean initLib()
{
    void* library;
    char* error;
    library=dlopen("/usr/lib/gtk-2.0/2.10.0/engines/liboxygen-gtk.so",RTLD_LAZY);
    if(!library)
    {
        fprintf(stderr,"%s\n",dlerror());
        return FALSE;
    }
    else
    {
        getWindecoABIVersion=(gint(*)(void))dlsym(library,"getWindecoABIVersion");
        if((error=dlerror())!=NULL)
        {
            fprintf(stderr,"%s\n",error);
            return FALSE;
        }
        else
        {
#define EXPECTED_ABI_VERSION 3
            gint version=getWindecoABIVersion();
            if(version != EXPECTED_ABI_VERSION)
            {
                fprintf(stderr,"ABI version %d is not equal to expected version %d\n",version,EXPECTED_ABI_VERSION);
                return FALSE;
            }
        }
        drawWindowDecoration=(void(*)(cairo_t*,WinDecoOptions,gint,gint,gint,gint,gchar**,gint,gint))dlsym(library,"drawWindowDecoration");
        if((error=dlerror())!=NULL)
        {
            fprintf(stderr,"%s\n",error);
            return FALSE;
        }
        drawWindecoButton=(void (*)(cairo_t*,ButtonType,ButtonStatus,WinDecoOptions,gint,gint,gint,gint))dlsym(library,"drawWindecoButton");
        if((error=dlerror())!=NULL)
        {
                fprintf(stderr,"%s\n",error);
                return FALSE;
        }
        drawWindecoShapeMask=(void (*)(cairo_t*,WinDecoOptions,gint,gint,gint,gint))dlsym(library,"drawWindecoShapeMask");
        if((error=dlerror())!=NULL)
        {
                fprintf(stderr,"%s\n",error);
                return FALSE;
        }
        drawWindowShadow=(void (*)(cairo_t*,WinDecoOptions,gint,gint,gint,gint))dlsym(library,"drawWindowShadow");
        if((error=dlerror())!=NULL)
        {
                fprintf(stderr,"%s\n",error);
                return FALSE;
        }


    }
    return TRUE;
}

gboolean on_expose(GtkWidget* mw0,GdkEventExpose* event,gpointer user_data)
{
    cairo_t* cr=gdk_cairo_create(mw0->window);
    WinDecoOptions opt=0;
    opt|= useAlpha ? WinHasAlpha : 0;
    opt|= WinIsResizable;
    if(!gtk_window_is_active(GTK_WINDOW(mw1)))
        opt|= WinIsActive;

    drawWindowShadow(cr,opt,0,0,mw0->allocation.width,mw0->allocation.height);

    gchar* windowStrings[]={"This is a caption","WindowClass10110111",0};

    drawWindowDecoration(cr,opt,0+shadowSize,0+shadowSize,mw0->allocation.width-shadowSize*2,mw0->allocation.height-shadowSize*2,windowStrings,0,20*ButtonTypeCount);
    ButtonType type;
    int status;
    for(status=0; status<ButtonStatusCount; status++)
    {
        for(type=0;type<ButtonTypeCount;type++)
        {
            drawWindecoButton(cr,type,status,opt,mw0->allocation.width-shadowSize-25-20*type,status*25+shadowSize,25,25);
        }
    }

    cairo_destroy(cr);
    return TRUE;
}

gboolean on_configure1(GtkWidget* mw1,GdkEventConfigure* event,gpointer user_data)
{
    static int w=0,h=0;
    static gboolean redraw=TRUE;
    if( event->width != w || event->height != h )
        redraw=TRUE;
    else
        redraw=FALSE;
    if( redraw  )
    {
        w=event->width;
        h=event->height;

        WinDecoOptions opt=0;
        opt|= useAlpha ? WinHasAlpha : 0;
        opt|= WinIsActive|WinIsResizable;

        gtk_window_resize(GTK_WINDOW(mw0),event->width+dw,event->height+dh);

        if(!useAlpha)
        {
            GdkBitmap* mask=gdk_pixmap_new(NULL,event->width+dw,event->height+dh,1);
            cairo_t* cr=gdk_cairo_create(mask);
            drawWindecoShapeMask(cr,opt,0+shadowSize,0+shadowSize,event->width+dw-shadowSize*2,event->height+dh-shadowSize*2);
            gdk_window_shape_combine_mask(mw0->window,NULL,0,0); // remove old mask
            gdk_window_shape_combine_mask(mw0->window,mask,0,0); // apply new mask
            gdk_pixmap_unref(mask);
        }
    }
    return FALSE;
}

gboolean on_configure0(GtkWidget* mw0,GdkEventConfigure* event,gpointer user_data)
{
    static int w=0,h=0;
    static gboolean active=FALSE;
    static gboolean redraw=TRUE;
    if( event->width != w || event->height != h )
        redraw=TRUE;
    else
        redraw=FALSE;
    if(!(gtk_window_is_active(GTK_WINDOW(mw1))!=active))
    {
        active ^= TRUE;
        redraw = TRUE;
    }
    if( redraw )
    {
        w=event->width;
        h=event->height;

        gdk_window_begin_paint_rect(mw0->window,(GdkRectangle*)&mw0->allocation);
        on_expose(mw0,NULL,NULL);
        gdk_window_end_paint(mw0->window);
    }
}

int main(int argc, char** argv)
{
    if(!initLib())
        return 1;
    gtk_init(&argc,&argv);

    mw0=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    mw1=gtk_window_new(GTK_WINDOW_TOPLEVEL);

    GdkColormap* cmap=gdk_screen_get_rgba_colormap(gdk_screen_get_default());
    gtk_widget_set_colormap(mw0,cmap);

    g_signal_connect(G_OBJECT(mw0),"destroy",G_CALLBACK(gtk_main_quit),NULL);
    g_signal_connect(G_OBJECT(mw1),"destroy",G_CALLBACK(gtk_main_quit),NULL);

    gtk_window_set_default_size(GTK_WINDOW(mw1),ww,wh);
    gtk_window_set_default_size(GTK_WINDOW(mw0),ww+dw,wh+dh);
    gtk_window_set_decorated(GTK_WINDOW(mw0),FALSE);

    gtk_window_set_title(GTK_WINDOW(mw1),"");

    g_signal_connect(G_OBJECT(mw0),"expose-event",G_CALLBACK(on_expose),NULL);
    g_signal_connect(G_OBJECT(mw1),"configure-event",G_CALLBACK(on_configure1),NULL);
    g_signal_connect(G_OBJECT(mw0),"configure-event",G_CALLBACK(on_configure0),NULL);

    gtk_widget_show(mw1);
    gtk_widget_show(mw0);

    gtk_window_move(GTK_WINDOW(mw0),300,320);
    gtk_window_move(GTK_WINDOW(mw1),300,320);

    gtk_main();
    return 0;
}
