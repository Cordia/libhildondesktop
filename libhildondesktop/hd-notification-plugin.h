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

#ifndef __HD_NOTIFICATION_PLUGIN_H__
#define __HD_NOTIFICATION_PLUGIN_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HD_TYPE_NOTIFICATION_PLUGIN           (hd_notification_plugin_get_type ())
#define HD_NOTIFICATION_PLUGIN(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_NOTIFICATION_PLUGIN, HDNotificationPlugin))
#define HD_IS_NOTIFICATION_PLUGIN(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_NOTIFICATION_PLUGIN))
#define HD_NOTIFICATION_PLUGIN_GET_IFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), HD_TYPE_NOTIFICATION_PLUGIN, HDNotificationPluginIface))

typedef struct _HDNotificationPlugin      HDNotificationPlugin;
typedef struct _HDNotificationPluginIface HDNotificationPluginIface;

/**
 * HDNotificationPluginIface:
 * @notify: The @notify function is called for each incoming notification, except
 *   for notifications that are internal to or only handled by the desktop code. 
 *   Overriding @notify is required. See hd_notification_plugin_notify().
 * @close: The @close function is called for each closed notification, except
 *   for notifications that are internal to or only handled by the desktop code. 
 *   Overriding @close is required. See hd_notification_plugin_close().
 **/
struct _HDNotificationPluginIface {
    /*< private >*/
    GTypeInterface parent;

    /*< public >*/
    void (*notify) (HDNotificationPlugin  *plugin,
                    const gchar           *app_name,
                    guint                  id,
                    const gchar           *icon,
                    const gchar           *summary,
                    const gchar           *body,
                    gchar                **actions,
                    GHashTable            *hints,
                    gint                   timeout);

    void (*close)  (HDNotificationPlugin  *plugin,
                    guint                  id);
};

GType hd_notification_plugin_get_type (void);

void hd_notification_plugin_notify (HDNotificationPlugin  *plugin,
                                    const gchar           *app_name,
                                    guint                  id,
                                    const gchar           *icon,
                                    const gchar           *summary,
                                    const gchar           *body,
                                    gchar                **actions,
                                    GHashTable            *hints,
                                    gint                   timeout);
void hd_notification_plugin_close  (HDNotificationPlugin  *plugin,
                                    guint                  id);

G_END_DECLS

#endif
