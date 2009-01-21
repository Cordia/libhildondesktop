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

#ifndef __HD_HOME_PLUGIN_ITEM_H__
#define __HD_HOME_PLUGIN_ITEM_H__

#include <gtk/gtk.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>

G_BEGIN_DECLS

#define HD_TYPE_HOME_PLUGIN_ITEM             (hd_home_plugin_item_get_type ())
#define HD_HOME_PLUGIN_ITEM(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_HOME_PLUGIN_ITEM, HDHomePluginItem))
#define HD_HOME_PLUGIN_ITEM_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HD_TYPE_HOME_PLUGIN_ITEM, HDHomePluginItemClass))
#define HD_IS_HOME_PLUGIN_ITEM(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_HOME_PLUGIN_ITEM))
#define HD_IS_HOME_PLUGIN_ITEM_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HD_TYPE_HOME_PLUGIN_ITEM))
#define HD_HOME_PLUGIN_ITEM_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HD_TYPE_HOME_PLUGIN_ITEM, HDHomePluginItemClass))

/**
 * HDHomePluginItemResizeType:
 * @HD_HOME_PLUGIN_ITEM_RESIZE_NONE: cannot be resized.
 * @HD_HOME_PLUGIN_ITEM_RESIZE_HORIZONTAL: can only be resized horizontally
 * @HD_HOME_PLUGIN_ITEM_RESIZE_VERTICAL: can only be resized vertically
 * @HD_HOME_PLUGIN_ITEM_RESIZE_BOTH: can be resized both horizontally and vertically
 *
 * Deprecated. Applets cannot be resized.
 *
 **/
typedef enum
{
  HD_HOME_PLUGIN_ITEM_RESIZE_NONE,
  HD_HOME_PLUGIN_ITEM_RESIZE_VERTICAL,
  HD_HOME_PLUGIN_ITEM_RESIZE_HORIZONTAL,
  HD_HOME_PLUGIN_ITEM_RESIZE_BOTH
} HDHomePluginItemResizeType;

typedef struct _HDHomePluginItem        HDHomePluginItem;
typedef struct _HDHomePluginItemClass   HDHomePluginItemClass;
typedef struct _HDHomePluginItemPrivate HDHomePluginItemPrivate;

/** HDHomePluginItem:
 *
 * A base class for other items.
 */
struct _HDHomePluginItem
{
  GtkWindow parent;

  HDHomePluginItemPrivate *priv;
};

struct _HDHomePluginItemClass
{
  GtkWindowClass parent;

  /* virtual functions */
  gchar *(*get_applet_id) (HDHomePluginItem *item);

  /* signals */
  void   (*show_settings) (HDHomePluginItem *item);
};

GType            hd_home_plugin_item_get_type               (void);

const gchar     *hd_home_plugin_item_get_dl_filename        (HDHomePluginItem            *item);

DBusConnection  *hd_home_plugin_item_get_dbus_connection    (HDHomePluginItem            *item,
                                                             DBusBusType                  type,
                                                             DBusError                   *error);
DBusGConnection *hd_home_plugin_item_get_dbus_g_connection  (HDHomePluginItem            *item,
                                                             DBusBusType                  type,
                                                             GError                     **error);

guint            hd_home_plugin_item_heartbeat_signal_add   (HDHomePluginItem            *item,
                                                             guint                        mintime,
                                                             guint                        maxtime,
                                                             GSourceFunc                  source_func,
                                                             gpointer                     data,
                                                             GDestroyNotify               destroy);

gchar           *hd_home_plugin_item_get_applet_id          (HDHomePluginItem            *item);

void             hd_home_plugin_item_set_settings           (HDHomePluginItem            *item,
                                                             gboolean                     settings);

/* Deprecated */
__attribute__((__deprecated__))
void             hd_home_plugin_item_set_resize_type        (HDHomePluginItem            *item,
                                                             HDHomePluginItemResizeType   resize_type);

G_END_DECLS

#endif

