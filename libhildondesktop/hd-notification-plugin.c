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
 * See #HDNotification for more information about the notifications.
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
 * @notification: the #HDNotification which was raised
 *
 * Calls the #HDNotificationPluginIface::notify() fucntion.
 *
 **/
void
hd_notification_plugin_notify (HDNotificationPlugin  *plugin,
                               HDNotification        *notification)
{
  HD_NOTIFICATION_PLUGIN_GET_IFACE (plugin)->notify (plugin,
                                                     notification);
}
