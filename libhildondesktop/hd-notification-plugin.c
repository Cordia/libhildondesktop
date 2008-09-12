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

#include "hd-notification-plugin.h"

/** 
 * SECTION:hd-notification-plugin
 * @short_description: Interface for notification plugins.
 *
 * Interface to be implemented by notification plugins.
 *
 * The #HDNotificationPluginIface::notify() function of the plugin is called for each incoming notification, 
 * except for notifications that are internal to or only handled by the desktop code.
 *
 * The #HDNotificationPluginIface::close() function is called to close a notification.
 **/

static void
hd_notification_plugin_class_init (gpointer g_iface)
{
}

GType
hd_notification_plugin_get_type (void)
{
  static GType notification_plugin_type = 0;

  if (!notification_plugin_type)
    {
      notification_plugin_type = g_type_register_static_simple (G_TYPE_INTERFACE,
                                                                "HDNotificationPlugin",
                                                                sizeof (HDNotificationPluginIface),
                                                                (GClassInitFunc) hd_notification_plugin_class_init,
                                                                0, NULL, 0);

      g_type_interface_add_prerequisite (notification_plugin_type, G_TYPE_OBJECT);
    }

  return notification_plugin_type;
}

/**
 * hd_notification_plugin_notify:
 * @plugin: a #HDNotificationPlugin
 * @app_name: an optional string, can be used for debugging and logging messages
 * @id: the id of the notification
 * @icon: the file name of icon to show
 * @summary: a summary string of the notification
 * @body: the message body of the notification
 * @actions: an array of label/actions strings (not used?)
 * @hints: a hash table with additional hints
 * @timeout: a timeout after which the notification is hidden.
 *
 * Calls the #HDNotificationPluginIface::notify() fucntion.
 *
 **/
void
hd_notification_plugin_notify (HDNotificationPlugin  *plugin,
                               const gchar           *app_name,
                               guint                  id,
                               const gchar           *icon,
                               const gchar           *summary,
                               const gchar           *body,
                               gchar                **actions,
                               GHashTable            *hints,
                               gint                   timeout)
{
  HD_NOTIFICATION_PLUGIN_GET_IFACE (plugin)->notify (plugin,
                                                     app_name,
                                                     id,
                                                     icon,
                                                     summary,
                                                     body,
                                                     actions,
                                                     hints,
                                                     timeout);
}

/**
 * hd_notification_plugin_close:
 * @plugin: a #HDNotificationPlugin
 * @id: the id of the notification
 *
 * Calls the #HDNotificationPluginIface::close() fucntion.
 *
 **/
void
hd_notification_plugin_close (HDNotificationPlugin  *plugin,
                              guint                  id)
{
  HD_NOTIFICATION_PLUGIN_GET_IFACE (plugin)->close (plugin,
                                                    id);
}

