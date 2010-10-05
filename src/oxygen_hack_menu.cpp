#include <gtk/gtk.h>

/* All these functions are copied from redmond engine */

/***********************************************/
/* MenuShell/MenuBar Item Prelight Workaround  */
/***********************************************/

/***********************************************
 * oxygen_hack_menu_shell_cleanup_signals
 *  
 *   Cleanup/remove Menu Shell signals
 ***********************************************/
void
oxygen_hack_menu_shell_cleanup_signals(GtkWidget *widget)
{
  if (GTK_IS_MENU_BAR(widget))
    {
      gint id = 0;

      id = (gint)g_object_steal_data (G_OBJECT(widget), "OXYGEN_MENU_SHELL_MOTION_ID");
      g_signal_handler_disconnect(G_OBJECT(widget), id);

      id = (gint)g_object_steal_data (G_OBJECT(widget), "OXYGEN_MENU_SHELL_LEAVE_ID");
      g_signal_handler_disconnect(G_OBJECT(widget), id);

      id = (gint)g_object_steal_data (G_OBJECT(widget), "OXYGEN_MENU_SHELL_DESTROY_ID");
      g_signal_handler_disconnect(G_OBJECT(widget), id);

      id = (gint)g_object_steal_data (G_OBJECT(widget), "OXYGEN_MENU_SHELL_STYLE_SET_ID");
      g_signal_handler_disconnect(G_OBJECT(widget), id);

      g_object_steal_data (G_OBJECT(widget), "OXYGEN_MENU_SHELL_HACK_SET");
    }
}



/***********************************************
 * oxygen_hack_menu_shell_style_set -
 *  
 *   Style set signal to ensure menushell signals
 *   get cleaned up if the theme changes
 ***********************************************/
static gboolean
oxygen_hack_menu_shell_style_set(GtkWidget *widget,
                         GtkStyle *previous_style,
                         gpointer user_data)
{
  oxygen_hack_menu_shell_cleanup_signals(widget);

  return FALSE;
}

/***********************************************
 * oxygen_hack_menu_shell_destroy -
 *  
 *   Destroy signal to ensure menushell signals
 *   get cleaned if it is destroyed
 ***********************************************/
static gboolean
oxygen_hack_menu_shell_destroy(GtkWidget *widget,
                       GdkEvent *event,
                       gpointer user_data)
{
  oxygen_hack_menu_shell_cleanup_signals(widget);

  return FALSE;
}

/***********************************************
 * oxygen_hack_menu_shell_leave -
 *  
 *   Leave signal to ensure menushell items
 *   normal state on mouse leave.
 ***********************************************/
static gboolean
oxygen_hack_menu_shell_leave(GtkWidget *widget,
                      GdkEventCrossing *event,
                      gpointer user_data)
{
  if (GTK_IS_MENU_SHELL(widget))
    {
      GList *children = NULL, *child = NULL;

      if (GTK_IS_CONTAINER(widget))
        {
          children = gtk_container_get_children(GTK_CONTAINER(widget));

          for (child = g_list_first(children); child; child = g_list_next(child))
            {
              if ((child->data) && GTK_IS_MENU_ITEM(child->data) &&
                  (GTK_WIDGET_STATE(GTK_WIDGET(child->data)) != GTK_STATE_INSENSITIVE))
                {
                  if ((!GTK_IS_MENU(GTK_MENU_ITEM(child->data)->submenu)) ||
                      (!(GTK_WIDGET_REALIZED(GTK_MENU_ITEM(child->data)->submenu) &&
                         GTK_WIDGET_VISIBLE(GTK_MENU_ITEM(child->data)->submenu) &&
                         GTK_WIDGET_REALIZED(GTK_MENU(GTK_MENU_ITEM(child->data)->submenu)->toplevel) &&
                         GTK_WIDGET_VISIBLE(GTK_MENU(GTK_MENU_ITEM(child->data)->submenu)->toplevel))))
                  {
                    gtk_widget_set_state (GTK_WIDGET(child->data), GTK_STATE_NORMAL);
                  }
                }
            }

          if (children)
            g_list_free(children);
        }
    }

  return FALSE;
}

/***********************************************
 * oxygen_hack_menu_shell_motion -
 *  
 *   Motion signal to ensure menushell items
 *   prelight state changes on mouse move.
 ***********************************************/
static gboolean 
oxygen_hack_menu_shell_motion(GtkWidget *widget, 
                      GdkEventMotion *event, 
                      gpointer user_data)
{
  if (GTK_IS_MENU_SHELL(widget))
    {
      gint pointer_x, pointer_y;
      GdkModifierType pointer_mask;
      GList *children = NULL, *child = NULL;

      gdk_window_get_pointer(widget->window, &pointer_x, &pointer_y, &pointer_mask);

      if (GTK_IS_CONTAINER(widget))
        {
          children = gtk_container_get_children(GTK_CONTAINER(widget));

          for (child = g_list_first(children); child; child = g_list_next(child))
            {
              if ((child->data) && GTK_IS_WIDGET(child->data) &&
                  (GTK_WIDGET_STATE(GTK_WIDGET(child->data)) != GTK_STATE_INSENSITIVE))
                {
                  if ((pointer_x >= GTK_WIDGET(child->data)->allocation.x) &&
                      (pointer_y >= GTK_WIDGET(child->data)->allocation.y) &&
                      (pointer_x < (GTK_WIDGET(child->data)->allocation.x +
                                      GTK_WIDGET(child->data)->allocation.width)) &&
                      (pointer_y < (GTK_WIDGET(child->data)->allocation.y +
                                      GTK_WIDGET(child->data)->allocation.height)))
                    {
                      gtk_widget_set_state (GTK_WIDGET(child->data), GTK_STATE_PRELIGHT);
                    }
                  else
                    {
                      gtk_widget_set_state (GTK_WIDGET(child->data), GTK_STATE_NORMAL);
                    }
                 }
             }

           if (children)
             g_list_free(children);
        }
    }

  return FALSE;
}


void oxygen_hack_menu_shell_setup_signals(GtkWidget* widget)
{
  if (GTK_IS_MENU_BAR(widget))
    {
      gint id = 0;

      if (!g_object_get_data(G_OBJECT(widget), "OXYGEN_MENU_SHELL_HACK_SET"))
      {
        id = g_signal_connect(G_OBJECT(widget), "motion-notify-event",
                                             (GCallback)oxygen_hack_menu_shell_motion,
                                             NULL);
                                  
        g_object_set_data(G_OBJECT(widget), "OXYGEN_MENU_SHELL_MOTION_ID", (gpointer)id);
        
        id = g_signal_connect(G_OBJECT(widget), "leave-notify-event",
                                             (GCallback)oxygen_hack_menu_shell_leave,
                                             NULL);
        g_object_set_data(G_OBJECT(widget), "OXYGEN_MENU_SHELL_LEAVE_ID", (gpointer)id);
                                             
        id = g_signal_connect(G_OBJECT(widget), "destroy-event",
                                             (GCallback)oxygen_hack_menu_shell_destroy,
                                             NULL);
        g_object_set_data(G_OBJECT(widget), "OXYGEN_MENU_SHELL_DESTROY_ID", (gpointer)id);
  
        g_object_set_data(G_OBJECT(widget), "OXYGEN_MENU_SHELL_HACK_SET", (gpointer)1);
     
        id = g_signal_connect(G_OBJECT(widget), "style-set",
                                             (GCallback)oxygen_hack_menu_shell_style_set,
                                             NULL);
        g_object_set_data(G_OBJECT(widget), "OXYGEN_MENU_SHELL_STYLE_SET_ID", (gpointer)id);
      }
    }
}
