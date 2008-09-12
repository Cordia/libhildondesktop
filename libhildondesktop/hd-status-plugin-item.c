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

#include <dbus/dbus-glib-lowlevel.h>

#include "hd-plugin-module.h"
#include "hd-heartbeat.h"

#include "hd-status-plugin-item.h"


/** 
 * SECTION:hd-status-plugin-item
 * @short_description: Base class for plugable status Hildon Desktop items.
 *
 * Base class for all plugable status Hildon Desktop items.
 *
 * The class provides the hd_status_plugin_item_set_status_area_icon() function to set or update
 * the Status Area icon.
 *
 * The hd_status_plugin_item_get_dl_filename() function can be used to get the filename of
 * the dynamic library for debugging purposes.
 *
 * Plugins should use private D-Bus connections. There is the 
 * hd_status_plugin_item_get_dbus_connection() function which should be used to create such
 * connections.
 **/

#define HD_STATUS_PLUGIN_ITEM_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_STATUS_PLUGIN_ITEM, HDStatusPluginItemPrivate))

static void hd_status_plugin_item_init_plugin_item (gpointer g_iface);

enum
{
  PROP_0,
  PROP_PLUGIN_ID,
  PROP_STATUS_AREA_ICON,
  PROP_STATUS_AREA_WIDGET,
};

struct _HDStatusPluginItemPrivate
{
  gchar     *plugin_id;

  GdkPixbuf *status_area_icon;

  GtkWidget *status_area_widget;
};

G_DEFINE_ABSTRACT_TYPE_WITH_CODE (HDStatusPluginItem, hd_status_plugin_item, GTK_TYPE_BIN,
                                  G_IMPLEMENT_INTERFACE (HD_TYPE_PLUGIN_ITEM,
                                                         hd_status_plugin_item_init_plugin_item));

static void
hd_status_plugin_item_init_plugin_item (gpointer g_iface)
{
  /* don't do anything */

  return;
}

static void
hd_status_plugin_item_size_allocate (GtkWidget     *widget,
                                     GtkAllocation *allocation)
{
  GtkWidget *child;

  GTK_WIDGET_CLASS (hd_status_plugin_item_parent_class)->size_allocate (widget,
                                                                        allocation);

  child = GTK_BIN (widget)->child;

  if (child)
    {
      GtkAllocation child_allocation;
      guint border_width;

      border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));

      child_allocation.x = allocation->x + border_width;
      child_allocation.y = allocation->y + border_width;
      child_allocation.width = allocation->width - 2 * border_width;
      child_allocation.height = allocation->height - 2 * border_width;

      gtk_widget_size_allocate (child, &child_allocation);
    }
}

static void
hd_status_plugin_item_size_request (GtkWidget      *widget,
                                    GtkRequisition *requisition)
{
  GtkWidget *child;
  GtkRequisition child_requisition = {0, 0};

  child = GTK_BIN (widget)->child;

  if (child)
    gtk_widget_size_request (child, &child_requisition);

  requisition->width = child_requisition.width;
  requisition->height = child_requisition.height;
}

static void
hd_status_plugin_item_dispose (GObject *object)
{
  HDStatusPluginItemPrivate *priv;

  priv = HD_STATUS_PLUGIN_ITEM (object)->priv;

  if (priv->status_area_icon)
    {
      g_object_unref (priv->status_area_icon);
      priv->status_area_icon = NULL;
    }

  if (priv->status_area_widget)
    {
      g_object_unref (priv->status_area_widget);
      priv->status_area_widget = NULL;
    }

  G_OBJECT_CLASS (hd_status_plugin_item_parent_class)->dispose (object);
}

static void
hd_status_plugin_item_finalize (GObject *object)
{
  HDStatusPluginItemPrivate *priv = HD_STATUS_PLUGIN_ITEM (object)->priv;

  g_free (priv->plugin_id);
  priv->plugin_id = NULL;

  G_OBJECT_CLASS (hd_status_plugin_item_parent_class)->finalize (object);
}

static void
hd_status_plugin_item_get_property (GObject      *object,
                                    guint         prop_id,
                                    GValue       *value,
                                    GParamSpec   *pspec)
{
  HDStatusPluginItemPrivate *priv = HD_STATUS_PLUGIN_ITEM (object)->priv;

  switch (prop_id)
    {
    case PROP_PLUGIN_ID:
      g_value_set_string (value, priv->plugin_id);
      break;

    case PROP_STATUS_AREA_ICON:
      g_value_set_object (value, priv->status_area_icon);
      break;

    case PROP_STATUS_AREA_WIDGET:
      g_value_set_object (value, priv->status_area_widget);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_status_plugin_item_set_property (GObject      *object,
                                    guint         prop_id,
                                    const GValue *value,
                                    GParamSpec   *pspec)
{
  HDStatusPluginItemPrivate *priv = HD_STATUS_PLUGIN_ITEM (object)->priv;

  switch (prop_id)
    {
    case PROP_PLUGIN_ID:
      g_free (priv->plugin_id);
      priv->plugin_id = g_value_dup_string (value);
      break;

    case PROP_STATUS_AREA_ICON:
      hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (object),
                                                  g_value_get_object (value));
      break;

    case PROP_STATUS_AREA_WIDGET:
      hd_status_plugin_item_set_status_area_widget (HD_STATUS_PLUGIN_ITEM (object),
                                                    g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_status_plugin_item_class_init (HDStatusPluginItemClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  widget_class->size_allocate = hd_status_plugin_item_size_allocate;
  widget_class->size_request = hd_status_plugin_item_size_request;

  object_class->dispose = hd_status_plugin_item_dispose;
  object_class->finalize = hd_status_plugin_item_finalize;
  object_class->get_property = hd_status_plugin_item_get_property;
  object_class->set_property = hd_status_plugin_item_set_property;

  g_object_class_override_property (object_class,
                                    PROP_PLUGIN_ID,
                                    "plugin-id");

  g_object_class_install_property (object_class,
                                   PROP_STATUS_AREA_ICON,
                                   g_param_spec_object ("status-area-icon",
                                                        "Status Area icon",
                                                        "The Status Area icon which should be displayed for the item",
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_STATUS_AREA_WIDGET,
                                   g_param_spec_object ("status-area-widget",
                                                        "Status Area widget",
                                                        "The widget which should be displayed in the Status Area (should be used by clock plugin only)",
                                                        GTK_TYPE_WIDGET,
                                                        G_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (HDStatusPluginItemPrivate));
}

static void
hd_status_plugin_item_init (HDStatusPluginItem *item)
{
  item->priv = HD_STATUS_PLUGIN_ITEM_GET_PRIVATE (item);  
}

/**
 * hd_status_plugin_item_set_status_area_icon:
 * @item: a #HDStatusPluginItem
 * @icon: a #GdkPixbuf used as the new icon or %NULL
 *
 * Sets the Status Area icon corresponding to this item to @icon.
 *
 * To hide (or unset) the Status Area icon use %NULL for @icon.
 *
 **/
void 
hd_status_plugin_item_set_status_area_icon (HDStatusPluginItem *item,
                                            GdkPixbuf          *icon)
{
  HDStatusPluginItemPrivate *priv;

  g_return_if_fail (HD_IS_STATUS_PLUGIN_ITEM (item));

  priv = item->priv;

  if (priv->status_area_icon)
    g_object_unref (priv->status_area_icon);

  if (icon)
    priv->status_area_icon = g_object_ref (icon);
  else
    priv->status_area_icon = NULL;

  g_object_notify (G_OBJECT (item), "status-area-icon");
}

/**
 * hd_status_plugin_item_set_status_area_widget:
 * @item: a #HDStatusPluginItem
 * @widget: a #GtkWidget which should be displayed in the Status Area
 *
 * Sets a widget which should be displayed in the Status Area. This function
 * should only be used by the Clock plugin.
 *
 **/
void 
hd_status_plugin_item_set_status_area_widget (HDStatusPluginItem *item,
                                              GtkWidget          *widget)
{
  HDStatusPluginItemPrivate *priv;

  g_return_if_fail (HD_IS_STATUS_PLUGIN_ITEM (item));

  priv = item->priv;

  if (priv->status_area_widget)
    g_object_unref (priv->status_area_widget);

  if (widget)
    priv->status_area_widget = g_object_ref_sink (widget);
  else
    priv->status_area_widget = NULL;

  g_object_notify (G_OBJECT (item), "status-area-widget");
}

/**
 * hd_status_plugin_item_get_dl_filename:
 * @item: a #HDStatusPluginItem
 *
 * Returns the filename of the dynamic library file from which this item was loaded.
 * Useful for debugging purposes.
 *
 * Returns: filename of the dynamic library file. The result must not be freed. 
 **/
const gchar *
hd_status_plugin_item_get_dl_filename (HDStatusPluginItem *item)
{
  g_return_val_if_fail (HD_IS_STATUS_PLUGIN_ITEM (item), NULL);

  return hd_plugin_item_get_dl_filename (HD_PLUGIN_ITEM (item));
}

/**
 * hd_status_plugin_item_get_dbus_connection:
 * @item: A #HDStatusPluginItem
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
 * Returns: A new private connection to bus %type. The connection must be unrefed with dbus_connection_unref() when it is not longer needed.
 **/
DBusConnection *
hd_status_plugin_item_get_dbus_connection (HDStatusPluginItem *item,
                                           DBusBusType         type,
                                           DBusError          *error)
{
  HDStatusPluginItemPrivate *priv;
  DBusConnection *connection;

  g_return_val_if_fail (HD_IS_STATUS_PLUGIN_ITEM (item), NULL);

  priv = item->priv;

  /* Create a private connection */
  connection = dbus_bus_get_private (type, error);

  if (!connection || (error != NULL && dbus_error_is_set (error)))
  return NULL;

  /* Do not exit on disconnect */
  dbus_connection_set_exit_on_disconnect (connection, FALSE);

  /* Log the connection name for debug purposes */
  g_debug ("Plugin '%s' opened D-Bus connection '%s'.",
           hd_status_plugin_item_get_dl_filename (item),
           dbus_bus_get_unique_name (connection));

  return connection;
}

/**
 * hd_status_plugin_item_get_dbus_g_connection:
 * @item: A #HDStatusPluginItem
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
hd_status_plugin_item_get_dbus_g_connection (HDStatusPluginItem  *item,
                                             DBusBusType          type,
                                             GError             **error)
{
  HDStatusPluginItemPrivate *priv;
  DBusGConnection *g_connection;
  DBusConnection *connection;
  GError *tmp_error = NULL;

  g_return_val_if_fail (HD_IS_STATUS_PLUGIN_ITEM (item), NULL);

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
           hd_status_plugin_item_get_dl_filename (item),
           dbus_bus_get_unique_name (connection));

  return g_connection;
}

/**
 * hd_status_plugin_item_heartbeat_signal_add:
 * @item: A #HDStatusPluginItem.
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
hd_status_plugin_item_heartbeat_signal_add (HDStatusPluginItem *item,
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
