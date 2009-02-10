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
 * @short_description: Base class for Home applets.
 *
 * Base class for Home applets.
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
};

enum
{
  SHOW_SETTINGS,
  LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0, };

struct _HDHomePluginItemPrivate
{
  gchar                      *plugin_id;

  gboolean                    settings;

  gboolean                    display_on_all_views; /* display on all views, loaded from .desktop */
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

static gboolean
hd_home_plugin_item_client_event (GtkWidget      *widget,
                                  GdkEventClient *event)
{
  static GdkAtom show_settings_atom = GDK_NONE;
  
  if (show_settings_atom == GDK_NONE)
    show_settings_atom = gdk_atom_intern_static_string ("_HILDON_APPLET_SHOW_SETTINGS");

  if (event->message_type == show_settings_atom)
    {
      g_signal_emit (widget, signals[SHOW_SETTINGS], 0);

      return TRUE;
    }

  return FALSE;
}

static void
hd_home_plugin_item_realize (GtkWidget *widget)
{
  HDHomePluginItemPrivate *priv = HD_HOME_PLUGIN_ITEM (widget)->priv;
  GdkDisplay *display;
  Atom atom, wm_type;
  gchar *applet_id;

  GTK_WIDGET_CLASS (hd_home_plugin_item_parent_class)->realize (widget);

  /* No border as decoration */
  gdk_window_set_decorations (widget->window, 0);

  /* Set the _NET_WM_WINDOW_TYPE property to _HILDON_WM_WINDOW_TYPE_HOME_APPLET */
  display = gdk_drawable_get_display (widget->window);
  atom = gdk_x11_get_xatom_by_name_for_display (display,
                                                "_NET_WM_WINDOW_TYPE");
  wm_type = gdk_x11_get_xatom_by_name_for_display (display,
                                                   "_HILDON_WM_WINDOW_TYPE_HOME_APPLET");

  XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window),
                   GDK_WINDOW_XID (widget->window),
                   atom, XA_ATOM, 32, PropModeReplace,
                   (unsigned char *)&wm_type, 1);

  applet_id = hd_home_plugin_item_get_applet_id (HD_HOME_PLUGIN_ITEM (widget));
  XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window),
                   GDK_WINDOW_XID (widget->window),
                   gdk_x11_get_xatom_by_name_for_display (display,
                                                          "_HILDON_APPLET_ID"),
                   gdk_x11_get_xatom_by_name_for_display (display,
                                                          "UTF8_STRING"),
                   8, PropModeReplace,
                   (guchar *) applet_id, strlen (applet_id));
  g_free (applet_id);

  /* Set or remove settings property */
  if (priv->settings)
    XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window),
                     GDK_WINDOW_XID (widget->window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_SETTINGS"),
                     XA_CARDINAL, 32, PropModeReplace,
                     (unsigned char *) &(priv->display_on_all_views), 1);
  else
    XDeleteProperty (GDK_WINDOW_XDISPLAY (widget->window),
                     GDK_WINDOW_XID (widget->window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_SETTINGS"));

  /* Set display on all views property */
  if (priv->display_on_all_views)
    XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window),
                     GDK_WINDOW_XID (widget->window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_DISPLAY_ON_ALL_VIEWS"),
                     XA_CARDINAL, 32, PropModeReplace,
                     (unsigned char *) &(priv->display_on_all_views), 1);
  else
    XDeleteProperty (GDK_WINDOW_XDISPLAY (widget->window),
                     GDK_WINDOW_XID (widget->window),
                     gdk_x11_get_xatom_by_name_for_display (display,
                                                            "_HILDON_APPLET_DISPLAY_ON_ALL_VIEWS"));
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
  HDHomePluginItemPrivate *priv;

  priv = HD_HOME_PLUGIN_ITEM (object)->priv;

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

  widget_class->client_event = hd_home_plugin_item_client_event;
  widget_class->realize = hd_home_plugin_item_realize;

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
  HDHomePluginItemPrivate *priv;
  DBusConnection *connection;

  g_return_val_if_fail (HD_IS_HOME_PLUGIN_ITEM (item), NULL);

  priv = item->priv;

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
  HDHomePluginItemPrivate *priv;
  DBusGConnection *g_connection;
  DBusConnection *connection;
  GError *tmp_error = NULL;

  g_return_val_if_fail (HD_IS_HOME_PLUGIN_ITEM (item), NULL);

  priv = item->priv;

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
 * called again.
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
  HDHomePluginItemPrivate *priv;

  g_return_if_fail (HD_IS_HOME_PLUGIN_ITEM (item));

  priv = item->priv;

  priv->settings = settings;

  if (GTK_WIDGET_REALIZED (item))
    {
      GtkWidget *widget = GTK_WIDGET (item);
      GdkDisplay *display;

      display = gdk_drawable_get_display (widget->window);

      /* Set or remove settings property from the window */
      if (priv->settings)
        XChangeProperty (GDK_WINDOW_XDISPLAY (widget->window),
                         GDK_WINDOW_XID (widget->window),
                         gdk_x11_get_xatom_by_name_for_display (display,
                                                                "_HILDON_APPLET_SETTINGS"),
                         XA_CARDINAL, 32, PropModeReplace,
                         (unsigned char *) &(priv->display_on_all_views), 1);
      else
        XDeleteProperty (GDK_WINDOW_XDISPLAY (widget->window),
                         GDK_WINDOW_XID (widget->window),
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
