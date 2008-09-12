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

static void hd_home_plugin_item_init_plugin_item (gpointer g_iface);

enum
{
  PROP_0,
  PROP_PLUGIN_ID,
  PROP_RESIZE_TYPE,
};

struct _HDHomePluginItemPrivate
{
  gchar                      *plugin_id;

  HDHomePluginItemResizeType  resize_type;

  GtkAllocation               default_geometry;     /* default geometry, loaded from .desktop */
  guint                       default_view;         /* default view, loaded from .desktop */
  gboolean                    display_on_all_views; /* display on all views, loaded from .desktop */
};

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (HDHomePluginItem, hd_home_plugin_item, GTK_TYPE_WINDOW,
                                  G_IMPLEMENT_INTERFACE (HD_TYPE_PLUGIN_ITEM,
                                                         hd_home_plugin_item_init_plugin_item));

static void
hd_home_plugin_item_init_plugin_item (gpointer g_iface)
{
  /* don't do anything */

  return;
}

static void
hd_home_plugin_item_realize (GtkWidget *widget)
{
  GdkDisplay *display;
  Atom atom, wm_type;

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

    case PROP_RESIZE_TYPE:
      g_value_set_enum (value, priv->resize_type);
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

    case PROP_RESIZE_TYPE:
      hd_home_plugin_item_set_resize_type (HD_HOME_PLUGIN_ITEM (object),
                                           g_value_get_enum (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_home_plugin_item_class_init (HDHomePluginItemClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  widget_class->realize = hd_home_plugin_item_realize;

  object_class->dispose = hd_home_plugin_item_dispose;
  object_class->finalize = hd_home_plugin_item_finalize;
  object_class->get_property = hd_home_plugin_item_get_property;
  object_class->set_property = hd_home_plugin_item_set_property;

  g_object_class_override_property (object_class,
                                    PROP_PLUGIN_ID,
                                    "plugin-id");

  g_object_class_install_property (object_class,
                                   PROP_RESIZE_TYPE,
                                   g_param_spec_enum ("resize-type",
                                                      "Resize Type",
                                                      "The resize type of the Home applet",
                                                      HD_TYPE_HOME_PLUGIN_ITEM_RESIZE_TYPE,
                                                      HD_HOME_PLUGIN_ITEM_RESIZE_NONE,
                                                      G_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (HDHomePluginItemPrivate));
}

static void
hd_home_plugin_item_init (HDHomePluginItem *item)
{
  item->priv = HD_HOME_PLUGIN_ITEM_GET_PRIVATE (item);  
}

GType
hd_home_plugin_item_resize_type_get_type (void)
{
  static GType etype = 0;
  if (G_UNLIKELY(etype == 0)) {
      static const GEnumValue values[] = {
            { HD_HOME_PLUGIN_ITEM_RESIZE_NONE, "HD_HOME_PLUGIN_ITEM_RESIZE_NONE", "none" },
            { HD_HOME_PLUGIN_ITEM_RESIZE_VERTICAL, "HD_HOME_PLUGIN_ITEM_RESIZE_VERTICAL", "vertical" },
            { HD_HOME_PLUGIN_ITEM_RESIZE_HORIZONTAL, "HD_HOME_PLUGIN_ITEM_RESIZE_HORIZONTAL", "horizontal" },
            { HD_HOME_PLUGIN_ITEM_RESIZE_BOTH, "HD_HOME_PLUGIN_ITEM_RESIZE_BOTH", "both" },
            { 0, NULL, NULL }
      };
      etype = g_enum_register_static (g_intern_static_string ("HDHomePluginItemResizeType"), values);
  }
  return etype;
}

void
hd_home_plugin_item_set_resize_type (HDHomePluginItem           *item,
                                     HDHomePluginItemResizeType  resize_type)
{
  g_return_if_fail (HD_IS_HOME_PLUGIN_ITEM (item));

  item->priv->resize_type = resize_type;
  g_object_notify (G_OBJECT (item), "resize-type");
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

