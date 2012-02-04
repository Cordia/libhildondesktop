/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include <gdk/gdkx.h>

#include <X11/X.h>
#include <X11/Xatom.h>

#include <dbus/dbus-glib-lowlevel.h>

#include "hd-heartbeat.h"
#include "hd-plugin-module.h"

#include "hd-home-plugin-item.h"

/** 
 * SECTION:hd-home-plugin-item
 * @short_description: Base class for Home widgets.
 * @include: libhildondesktop/libhildondesktop.h
 * 
 * Base class for Home widgets. To create ur own Home widgets create a
 * subclass of #HDHomePluginItem.
 *
 * Eventhough #HdHomePluginItem is a GtkWindow it should mostly be used just as
 * a simple widget, where the content is drawn into it with cairo. Since there is
 * only tapping support for desktop widgets it is not possible to add complex
 * widgets like pannable widgets into a desktop widget. It is also not possible
 * to use clutter directly for implementing desktop widgets eventhough the window
 * manager (hildon-desktop) is clutter based.
 *
 * To create an transparent Home widget you have to set the colormap of the
 * widget to RGBA. See the following example:
 * 
 * <example>
 * <title>Create a transparent Home widget</title>
 * <programlisting>
 * static void
 * example_clock_applet_realize (GtkWidget *widget)
 * {
 *   GdkScreen *screen = gtk_widget_get_screen (widget);
 *   gtk_widget_set_colormap (widget, gdk_screen_get_rgba_colormap (screen));
 *   gtk_widget_set_app_paintable (widget, TRUE);
 *
 *   GTK_WIDGET_CLASS (example_clock_applet_parent_class)->realize (widget);
 * }
 *
 * static gboolean
 * example_clock_applet_expose_event (GtkWidget *widget,
 *                                    GdkExposeEvent *event)
 * {
 *   cairo_t *cr;
 *
 *   cr = gdk_cairo_create (GDK_DRAWABLE (widget->window));
 *   gdk_cairo_region (cr, event->region);
 *   cairo_clip (cr);
 *
 *   cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
 *   cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.0);
 *   cairo_paint (cr);
 *   
 *   cairo_destroy (cr);
 *
 *   return GTK_WIDGET_CLASS (example_clock_applet_parent_class)->expose_event (widget,
 *                                                                              event);
 * }
 *
 * static void
 * example_clock_applet_class_init (ExampleClockAppletClass *klass)
 * {
 *   GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
 *
 *   widget_class->realize = example_clock_applet_realize;
 *   widget_class->expose_event = example_clock_applet_expose_event;
 * }
 * </programlisting>
 * </example>
 *
 * To support a settings dialog in the layout mode in the Hildon desktop
 * connect to the #HDHomePluginItem::show-settings signal. And set the
 * property #HDHomePluginItem::settings to %TRUE.
 *
 * To start and stop animations of the widgets listen to the #GObject::notify signal of
 * the #HDHomePluginItem::is-on-current-desktop property. And show animations
 * only when #HDHomePluginItem::is-on-current-desktop is %TRUE.
 *
 * Plugin code is loaded and unloaded from memory using the Glib dynamic loader, #GModule.
 * In some cases (for example the plugin defines a #GType with pointers to code in the plugin)
 * the plugin code cannot be safely unloaded from memory even after the plugin object has
 * been finalized.  In that case, unloading can be prevented by including the following
 * code somewhere in the code for the plugin:
 * |[
 * const gchar *
 * g_module_check_init (GModule *module)
 * {
 *      g_module_make_resident(module);
 *      return NULL;
 * }
 * ]|
 * In this case, of course, the plugin code can only be updated by restarting hildon-home
 * so users wanting to update the widget will need to reboot.
 *
 **/

#define HD_HOME_PLUGIN_ITEM_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_HOME_PLUGIN_ITEM, HDHomePluginItemPrivate))

static void hd_home_plugin_item_init_plugin_item (HDPluginItemIface *iface);

enum
{
  PROP_0,
  PROP_PLUGIN_ID,
  PROP_SETTINGS,
  PROP_IS_ON_CURRENT_DESKTOP
};

enum
{
  SHOW_SETTINGS,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0, };

static GdkAtom show_settings_atom = GDK_NONE;

struct _HDHomePluginItemPrivate
{
  gchar                      *plugin_id;

  gboolean                    settings;

  gboolean                    display_on_all_views; /* display on all views, loaded from .desktop */

  gboolean                    is_on_current_desktop;
};

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (HDHomePluginItem, hd_home_plugin_item, GTK_TYPE_WINDOW,
                                  G_IMPLEMENT_INTERFACE (HD_TYPE_PLUGIN_ITEM,
                                                         hd_home_plugin_item_init_plugin_item));

static void
hd_home_plugin_item_load_desktop_file (HDPluginItem *item,
                                       GKeyFile     *key_file)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (item)->priv;

  /* Display on all views */
  priv->display_on_all_views = g_key_file_get_boolean (key_file,
                                                       "Desktop Entry",
                                                       "X-Display-On-All-Views",
                                                       NULL);
}

static void
hd_home_plugin_item_init_plugin_item (HDPluginItemIface *iface)
{
  iface->load_desktop_file = hd_home_plugin_item_load_desktop_file;

  return;
}

static GdkFilterReturn
hd_home_plugin_item_event_filter (GdkXEvent *xevent,
                                  GdkEvent *event,
                                  gpointer data)
{
  if (((XClientMessageEvent*)xevent)->type == ClientMessage &&
      ((XClientMessageEvent*)xevent)->message_type == gdk_x11_atom_to_xatom (show_settings_atom))
    {
      g_signal_emit (GTK_WIDGET (data), signals[SHOW_SETTINGS], 0);

      return GDK_FILTER_REMOVE;
    }

  return GDK_FILTER_CONTINUE;
}

static gboolean
hd_home_plugin_item_property_notify_event (GtkWidget        *widget,
                                           GdkEventProperty *event)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (widget)->priv;
  static GdkAtom is_on_current_desktop_atom = GDK_NONE;

  if (G_UNLIKELY (is_on_current_desktop_atom == GDK_NONE))
    is_on_current_desktop_atom = gdk_atom_intern_static_string ("_HILDON_APPLET_ON_CURRENT_DESKTOP");

  if (event->atom == is_on_current_desktop_atom)
    {
      gboolean old_value = priv->is_on_current_desktop;

      if (event->state == GDK_PROPERTY_NEW_VALUE)
        priv->is_on_current_desktop = TRUE;
      else if (event->state == GDK_PROPERTY_DELETE)
        priv->is_on_current_desktop = FALSE;

      if (old_value != priv->is_on_current_desktop)
        g_object_notify (G_OBJECT (widget), "is-on-current-desktop");

      return TRUE;
    }

  if (GTK_WIDGET_CLASS (hd_home_plugin_item_parent_class)->property_notify_event)
    return GTK_WIDGET_CLASS (hd_home_plugin_item_parent_class)->property_notify_event (widget, event);

  return FALSE;
}

static void
hd_home_plugin_item_realize (GtkWidget *widget)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (widget)->priv;
  GdkDisplay *display;
  Atom atom, wm_type;
  gchar *applet_id;
  GdkRGBA transparent = {0.0, 0.0, 0.0, 0.0};
  GdkWindow *window;

  GTK_WIDGET_CLASS (hd_home_plugin_item_parent_class)->realize (widget);

  window = gtk_widget_get_window (widget);

  /* No border as decoration */
  gdk_window_set_decorations (window, 0);

  /* Set the _NET_WM_WINDOW_TYPE property to _HILDON_WM_WINDOW_TYPE_HOME_APPLET */
  display = gdk_window_get_display (window);
  atom = gdk_x11_get_xatom_by_name_for_display (display,
                                                "_NET_WM_WINDOW_TYPE");
  wm_type = gdk_x11_get_xatom_by_name_for_display (display,
                                                   "_HILDON_WM_WINDOW_TYPE_HOME_APPLET");

  XChangeProperty (GDK_WINDOW_XDISPLAY (window),
                   GDK_WINDOW_XID (window),
                   atom, XA_ATOM, 32, PropModeReplace,
                   (unsigned char *)&wm_type, 1);

  applet_id = hd_home_plugin_item_get_applet_id (HD_HOME_PLUGIN_ITEM (widget));
  XChangeProperty (GDK_WINDOW_XDISPLAY (window),
                   GDK_WINDOW_XID (window),
                   gdk_x11_get_xatom_by_name_for_display (display,
                                                          "_HILDON_APPLET_ID"),
                   gdk_x11_get_xatom_by_name_for_display (display,
                                                          "UTF8_STRING"),
                   8, PropModeReplace,
                   (guchar *) applet_id, strlen (applet_id));
  g_free (applet_id);

  /* Set or remove settings property */
  if (priv->settings)
    XChangeProperty (GDK_WINDOW_XDISPLAY (window),
                     GDK_WINDOW_XID (window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_SETTINGS"),
                     XA_CARDINAL, 32, PropModeReplace,
                     (unsigned char *) &(priv->settings), 1);
  else
    XDeleteProperty (GDK_WINDOW_XDISPLAY (window),
                     GDK_WINDOW_XID (window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_SETTINGS"));

  /* Set display on all views property */
  if (priv->display_on_all_views)
    XChangeProperty (GDK_WINDOW_XDISPLAY (window),
                     GDK_WINDOW_XID (window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_DISPLAY_ON_ALL_VIEWS"),
                     XA_CARDINAL, 32, PropModeReplace,
                     (unsigned char *) &(priv->display_on_all_views), 1);
  else
    XDeleteProperty (GDK_WINDOW_XDISPLAY (window),
                     GDK_WINDOW_XID (window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_DISPLAY_ON_ALL_VIEWS"));


  /* Install client message filter */
  if (show_settings_atom == GDK_NONE)
    show_settings_atom = gdk_atom_intern_static_string ("_HILDON_APPLET_SHOW_SETTINGS");

  gdk_window_add_filter (window,
                         hd_home_plugin_item_event_filter, widget);

  /* Set background to transparent */
  gdk_window_set_background_rgba (window, &transparent);
}

static void
hd_home_plugin_item_unrealize (GtkWidget *widget)
{
  gdk_window_remove_filter (gtk_widget_get_window (widget),
                            hd_home_plugin_item_event_filter, widget);

  GTK_WIDGET_CLASS (hd_home_plugin_item_parent_class)->unrealize (widget);
}

static void
hd_home_plugin_item_constructed (GObject *object)
{
  G_OBJECT_CLASS (hd_home_plugin_item_parent_class)->constructed (object);

  gtk_window_set_accept_focus (GTK_WINDOW (object), FALSE);
}

static void
hd_home_plugin_item_dispose (GObject *object)
{
//HDHomePluginItemPrivate *priv;

//priv = HD_HOME_PLUGIN_ITEM (object)->priv;

  G_OBJECT_CLASS (hd_home_plugin_item_parent_class)->dispose (object);
}

static void
hd_home_plugin_item_finalize (GObject *object)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (object)->priv;

  g_free (priv->plugin_id);
  priv->plugin_id = NULL;

  G_OBJECT_CLASS (hd_home_plugin_item_parent_class)->finalize (object);
}

static void
hd_home_plugin_item_get_property (GObject      *object,
                                  guint         prop_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (object)->priv;

  switch (prop_id)
    {
    case PROP_PLUGIN_ID:
      g_value_set_string (value, priv->plugin_id);
      break;

    case PROP_IS_ON_CURRENT_DESKTOP:
      g_value_set_boolean (value, priv->is_on_current_desktop);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_home_plugin_item_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (object)->priv;

  switch (prop_id)
    {
    case PROP_PLUGIN_ID:
      g_free (priv->plugin_id);
      priv->plugin_id = g_value_dup_string (value);
      break;

    case PROP_SETTINGS:
      hd_home_plugin_item_set_settings (HD_HOME_PLUGIN_ITEM (object),
                                        g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static gchar *
hd_home_plugin_item_get_applet_id_real (HDHomePluginItem *item)
{
  gchar *plugin_id, *p;

  plugin_id = hd_plugin_item_get_plugin_id (HD_PLUGIN_ITEM (item));

  /* replace "/" and "#" by "_" */
  for (p = plugin_id; *p != '\0'; p++)
    {
      if (*p == '/')
        {
          g_warning ("Plugin id for applets should not contain '/'");
          *p = '_';
        }
      else if (*p == '#')
        {
          g_warning ("Plugin id for applets should not contain '#'");
          *p = '_';
        }
    }

  return plugin_id;
}

static void
hd_home_plugin_item_class_init (HDHomePluginItemClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  klass->get_applet_id = hd_home_plugin_item_get_applet_id_real;

  widget_class->property_notify_event = hd_home_plugin_item_property_notify_event;
  widget_class->realize = hd_home_plugin_item_realize;
  widget_class->unrealize = hd_home_plugin_item_unrealize;

  object_class->constructed = hd_home_plugin_item_constructed;
  object_class->dispose = hd_home_plugin_item_dispose;
  object_class->finalize = hd_home_plugin_item_finalize;
  object_class->get_property = hd_home_plugin_item_get_property;
  object_class->set_property = hd_home_plugin_item_set_property;

  g_object_class_override_property (object_class,
                                    PROP_PLUGIN_ID,
                                    "plugin-id");

  g_object_class_install_property (object_class,
                                   PROP_SETTINGS,
                                   g_param_spec_boolean ("settings",
                                                         "Settings",
                                                         "If the applet should show a settings button in the layout mode",
                                                         FALSE,
                                                         G_PARAM_WRITABLE));

  g_object_class_install_property (object_class,
                                   PROP_IS_ON_CURRENT_DESKTOP,
                                   g_param_spec_boolean ("is-on-current-desktop",
                                                         "Is On Current Desktop",
                                                         "If the applet is shown on current desktop",
                                                         FALSE,
                                                         G_PARAM_READABLE));

  /**
   * HDHomePluginItem::show-settings
   * @item: The #HDHomePluginItem which emitted the signal  
   *
   * The #HDHomePluginItem::show-settings signal is emmited when the settings button
   * is clicked on a desktop widget in the desktop layout mode. The settings button 
   * is only displayed when the #HDHomePluginItem::settings property is set to %TRUE.
   *
   * In repsonse to this signal a settings dialog should be shown.
   */
  signals[SHOW_SETTINGS] = g_signal_new ("show-settings",
                                         HD_TYPE_HOME_PLUGIN_ITEM,
                                         G_SIGNAL_RUN_LAST,
                                         0, /* No class method to not break ABI */
                                         NULL,
                                         NULL,
                                         g_cclosure_marshal_VOID__VOID,
                                         G_TYPE_NONE,
                                         0);

  g_type_class_add_private (klass, sizeof (HDHomePluginItemPrivate));
}

static void
hd_home_plugin_item_init (HDHomePluginItem *item)
{
  item->priv = HD_HOME_PLUGIN_ITEM_GET_PRIVATE (item);

  gtk_widget_add_events (GTK_WIDGET (item),
                         GDK_PROPERTY_CHANGE_MASK);
}

/**
 * hd_home_plugin_item_get_dl_filename:
 * @item: a #HDHomePluginItem
 *
 * Returns the filename of the dynamic library file from which this item was loaded.
 * Useful for debugging purposes.
 *
 * Returns: filename of the dynamic library file. The result must not be freed. 
 **/
const gchar *
hd_home_plugin_item_get_dl_filename (HDHomePluginItem *item)
{
  g_return_val_if_fail (HD_IS_HOME_PLUGIN_ITEM (item), NULL);

  return hd_plugin_item_get_dl_filename (HD_PLUGIN_ITEM (item));
}

/**
 * hd_home_plugin_item_get_dbus_connection:
 * @item: A #HDHomePluginItem
 * @type: The #DBusBusType %DBUS_BUS_SESSION or %DBUS_BUS_SYSTEM
 * @error: A #DBusError to return error messages
 *
 * Creates a new private #DBusConnection to the D-Bus session or system bus.
 *
 * It is similar to the dbus_bus_get_private() function but in contrast to the
 * dbus_bus_get_private() function the application will not exit if the connection
 * closes. Additionally this function is used to map the unique D-Bus name to the
 * plugin.
 *
 * So this function should be used by plugins to create D-Bus connections.
 *
 * Returns: A new private connection to bus %type. The connection must be unrefed with
 *   dbus_connection_unref() when it is not longer needed.
 **/
DBusConnection *
hd_home_plugin_item_get_dbus_connection (HDHomePluginItem *item,
                                         DBusBusType         type,
                                         DBusError          *error)
{
  DBusConnection *connection;

  g_return_val_if_fail (HD_IS_HOME_PLUGIN_ITEM (item), NULL);

  /* Create a private connection */
  connection = dbus_bus_get_private (type, error);

  if (!connection || (error != NULL && dbus_error_is_set (error)))
    return NULL;

  /* Do not exit on disconnect */
  dbus_connection_set_exit_on_disconnect (connection, FALSE);

  /* Log the connection name for debug purposes */
  g_debug ("Plugin '%s' opened D-Bus connection '%s'.",
           hd_home_plugin_item_get_dl_filename (item),
           dbus_bus_get_unique_name (connection));

  return connection;
}

/**
 * hd_home_plugin_item_get_dbus_g_connection:
 * @item: A #HDHomePluginItem
 * @type: The #DBusBusType %DBUS_BUS_SESSION or %DBUS_BUS_SYSTEM
 * @error: A #GError to return error messages
 *
 * Creates a new #DBusGConnection to the D-Bus session or system bus.
 *
 * Internally, calls dbus_g_bus_get(). See there for further informations.
 *
 * Returns: A shared connection.
 **/
DBusGConnection *
hd_home_plugin_item_get_dbus_g_connection (HDHomePluginItem  *item,
                                           DBusBusType          type,
                                           GError             **error)
{
  DBusGConnection *g_connection;
  DBusConnection *connection;
  GError *tmp_error = NULL;

  g_return_val_if_fail (HD_IS_HOME_PLUGIN_ITEM (item), NULL);

  /* Create a DBusGConnection (not private yet) */
  g_connection = dbus_g_bus_get (type, &tmp_error);

  if (tmp_error != NULL)
    {
      g_propagate_error (error, tmp_error);
      return NULL;
    }

  connection = dbus_g_connection_get_connection (g_connection);

  /* Log the connection name for debug purposes */
  g_debug ("Plugin '%s' opened D-Bus connection '%s'.",
           hd_home_plugin_item_get_dl_filename (item),
           dbus_bus_get_unique_name (connection));

  return g_connection;
}

/**
 * hd_home_plugin_item_heartbeat_signal_add:
 * @item: A #HDHomePluginItem.
 * @mintime: Time in seconds that must be waited before @source_func is called, or 0.
 * @maxtime: Time in seconds when the wait must end.
 * @source_func: Function to call.
 * @data: Data to pass to @function.
 * @destroy: Function to call when the signal is removed, or %NULL.
 *
 * Sets a function to be called at regular intervals. The @source_func is called repeatedly until 
 * it returns FALSE, at which point it is automatically destroyed and the function will not be
 * called again. The advantage of using this function over g_timeout_add_seconds_full() is
 * that all services using iphb on the device will wakeup synchronized, which will result in a
 * less power consumptioess  <note><para>Care must be taken when using this function not to crash hildon-home.  
 * If the plugin is removed from the desktop by the user, the timer 
 * <emphasis>must</emphasis> be cancelled in the dispose or finalize function to
 * avoid the timer firing after the plugin code has been unloaded from memory. 
 * The only way to cancel the timer is to destroy the event source. To do this,
 * save the event source ID returned by this function (assume it is called "timer_source")
 * and include something like
 * |[if (timer_source)
 *     {
 *       g_source_remove (timer_source);
 *       timer_source = 0;
 *     }]|
 * in the dispose or finalize function.</para></note>
 *
 * It is wise to have maxtime-mintime quite big so all users of this service get synced.
 *
 * If iphb is not avaiable g_timeout_add_seconds_full() is used with maxtime
 * as interval.
 * 
 * See iphb_wait() for more information.
 *
 * Returns: The ID (greater than 0) of the event source.
 **/
guint
hd_home_plugin_item_heartbeat_signal_add (HDHomePluginItem *item,
                                          guint               mintime,
                                          guint               maxtime,
                                          GSourceFunc         source_func,
                                          gpointer            data,
                                          GDestroyNotify      destroy)
{
  return hd_heartbeat_add_full (G_PRIORITY_DEFAULT,
                                mintime,
                                maxtime,
                                source_func,
                                data,
                                destroy);
}

/**
 * hd_home_plugin_item_get_applet_id:
 * @item: A #HDHomePluginItem
 *
 * Returns the applet id which is used to identify the applet in
 * the Hildon Desktop.
 *
 * Returns: The applet id. Free it when not longer used.
 *
 **/
gchar *
hd_home_plugin_item_get_applet_id (HDHomePluginItem *item)
{
  HDHomePluginItemClass *klass;

  g_return_val_if_fail (HD_IS_HOME_PLUGIN_ITEM (item), NULL);
  
  klass = HD_HOME_PLUGIN_ITEM_GET_CLASS (item);

  if (klass->get_applet_id)
    return klass->get_applet_id (item);

  g_warning ("No get_applet_id vfunction in %s", G_OBJECT_TYPE_NAME (item));

  return hd_home_plugin_item_get_applet_id_real (item);
}

/**
 * hd_home_plugin_item_set_settings:
 * @item: A #HDHomePluginItem
 * @settings: Whether the applet supports settings
 *
 * Sets whether the applet should show a settings button in layout mode.
 *
 * The applet should connect to the #HDHomePluginItem::show-settings signal
 * to get notified when it should show the settings dialog.
 *
 **/
void
hd_home_plugin_item_set_settings (HDHomePluginItem *item,
                                  gboolean          settings)
{
  g_return_if_fail (HD_IS_HOME_PLUGIN_ITEM (item));

  item->priv->settings = settings;

  if (gtk_widget_get_realized (GTK_WIDGET (item)))
    {
      GdkWindow *window;
      GdkDisplay *display;

      window = gtk_widget_get_window (GTK_WIDGET (item));
      display = gdk_window_get_display (window);

      /* Set or remove settings property from the window */
      if (item->priv->settings)
        XChangeProperty (GDK_WINDOW_XDISPLAY (window),
                         GDK_WINDOW_XID (window),
                         gdk_x11_get_xatom_by_name_for_display (display,
                                                                "_HILDON_APPLET_SETTINGS"),
                         XA_CARDINAL, 32, PropModeReplace,
                         (unsigned char *) &(item->priv->settings), 1);
      else
        XDeleteProperty (GDK_WINDOW_XDISPLAY (window),
                         GDK_WINDOW_XID (window),
                         gdk_x11_get_xatom_by_name_for_display (display,
                                                                "_HILDON_APPLET_SETTINGS"));
    }
}

/**
 * hd_home_plugin_item_set_resize_type:
 * @item: A #HDHomePluginItem
 * @resize_type: a resize type.
 *
 * Deprectaed. Applets are not resizeable.
 **/
void
hd_home_plugin_item_set_resize_type (HDHomePluginItem           *item,
                                     HDHomePluginItemResizeType  resize_type)
{
  /* Deprecated */
}
