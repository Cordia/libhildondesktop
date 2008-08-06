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
 * the dynamic library for debuggin purposes.
 *
 * Plugins should use private D-Bus connections. There is the 
 * hd_status_plugin_item_get_dbus_connection() function which should be used to create such
 * connections.
 **/

#define HD_STATUS_PLUGIN_ITEM_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_STATUS_PLUGIN_ITEM, HDStatusPluginItemPrivate))

enum
{
  PROP_0,
  PROP_STATUS_AREA_ICON,
  PROP_DL_FILENAME,
};

struct _HDStatusPluginItemPrivate
{
  GdkPixbuf *status_area_icon;

  gchar     *dl_filename;
};

G_DEFINE_ABSTRACT_TYPE (HDStatusPluginItem, hd_status_plugin_item, GTK_TYPE_BIN);

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

  G_OBJECT_CLASS (hd_status_plugin_item_parent_class)->dispose (object);
}

static void
hd_status_plugin_item_finalize (GObject *object)
{
  HDStatusPluginItemPrivate *priv;

  priv = HD_STATUS_PLUGIN_ITEM (object)->priv;

  if (priv->dl_filename)
    {
      g_free (priv->dl_filename);
      priv->dl_filename = NULL;
    }

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
    case PROP_STATUS_AREA_ICON:
      g_value_set_object (value, priv->status_area_icon);
      break;

    case PROP_DL_FILENAME:
      g_value_set_string (value, priv->dl_filename);
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
    case PROP_STATUS_AREA_ICON:
      hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (object),
                                                  g_value_get_object (value));
      break;

    case PROP_DL_FILENAME:
      g_free (priv->dl_filename);
      priv->dl_filename = g_value_dup_string (value);
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

  g_object_class_install_property (object_class,
                                   PROP_STATUS_AREA_ICON,
                                   g_param_spec_object ("status-area-icon",
                                                        "Status Area icon",
                                                        "The Status Area icon which should be displayed for the item",
                                                        GDK_TYPE_PIXBUF,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_DL_FILENAME,
                                   g_param_spec_string ("dl-filename",
                                                        "Dynamic library filename",
                                                        "The filename of the dynamic library file from which this item was loaded (used for debugging)",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

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
 * hd_status_plugin_item_get_dl_filename:
 * @item: a #HDStatusPluginItem
 *
 * Returns the filename of the dynamic library file from which this item was loaded.
 * Useful for debugging purposes.
 *
 * Returns: filename of the dynamic library file. The result must be freed with g_free() when the application is finished with it. 
 **/
gchar *
hd_status_plugin_item_get_dl_filename (HDStatusPluginItem *item)
{
  HDStatusPluginItemPrivate *priv;

  g_return_val_if_fail (HD_IS_STATUS_PLUGIN_ITEM (item), NULL);

  priv = item->priv;

  if (priv->dl_filename)
    return g_strdup (priv->dl_filename);

  return NULL;
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
                                           DBusBusType       type,
                                           DBusError        *error)
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

  /* FIXME: log the connection name for debug purposes */
  g_debug ("D-Bus connection %s for plugin %s opened.",
           dbus_bus_get_unique_name (connection),
           priv->dl_filename);

  return connection;
}
