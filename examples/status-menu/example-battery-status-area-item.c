/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
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

#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <dbus/dbus.h>

#include "example-battery-status-area-item.h"

#define EXAMPLE_BATTERY_STATUS_AREA_ITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, EXAMPLE_TYPE_BATTERY_STATUS_AREA_ITEM, ExampleBatteryStatusAreaItemPrivate))

struct _ExampleBatteryStatusAreaItemPrivate
{
  gpointer data;
};

HD_DEFINE_PLUGIN_MODULE (ExampleBatteryStatusAreaItem, example_battery_status_area_item, HD_TYPE_STATUS_MENU_ITEM);

static void
example_battery_status_area_item_class_finalize (ExampleBatteryStatusAreaItemClass *klass)
{
}

static void
example_battery_status_area_item_class_init (ExampleBatteryStatusAreaItemClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExampleBatteryStatusAreaItemPrivate));
}

static void
example_battery_status_area_item_init (ExampleBatteryStatusAreaItem *menu_item)
{
  GdkPixbuf *pixbuf;

  menu_item->priv = EXAMPLE_BATTERY_STATUS_AREA_ITEM_GET_PRIVATE (menu_item);

  /* Show a Status Area icon */
  pixbuf = gdk_pixbuf_new_from_file (HILDON_DATA_DIR "/example-status-area-icon-battery.png",
                                     NULL);
  hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (menu_item),
                                              pixbuf);
  g_object_unref (pixbuf);
}
