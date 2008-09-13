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

#ifndef __HD_PLUGIN_ITEM_H__
#define __HD_PLUGIN_ITEM_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HD_TYPE_PLUGIN_ITEM           (hd_plugin_item_get_type ())
#define HD_PLUGIN_ITEM(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_PLUGIN_ITEM, HDPluginItem))
#define HD_IS_PLUGIN_ITEM(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_PLUGIN_ITEM))
#define HD_PLUGIN_ITEM_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HD_TYPE_PLUGIN_ITEM, HDPluginItemIface))

typedef struct _HDPluginItem HDPluginItem;
typedef struct _HDPluginItemIface HDPluginItemIface;

struct _HDPluginItemIface
{
  GTypeInterface parent;

  void (*load_desktop_file) (HDPluginItem *item,
                             GKeyFile     *key_file);
};

GType        hd_plugin_item_get_type          (void);

gchar       *hd_plugin_item_get_plugin_id     (HDPluginItem *item);
const gchar *hd_plugin_item_get_dl_filename   (HDPluginItem *item);

void         hd_plugin_item_load_desktop_file (HDPluginItem *item,
                                               GKeyFile     *key_file);

G_END_DECLS

#endif
