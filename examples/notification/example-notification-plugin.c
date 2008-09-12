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

#include "example-notification-plugin.h"

#define EXAMPLE_NOTIFICATION_PLUGIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, \
                                                                                   EXAMPLE_TYPE_NOTIFICATION_PLUGIN, \
                                                                                   ExampleNotificationPluginPrivate))

struct _ExampleNotificationPluginPrivate
{
  gpointer data;
};

static void example_notification_plugin_notification_plugin_iface_init (HDNotificationPluginIface *iface);

G_DEFINE_DYNAMIC_TYPE_EXTENDED (ExampleNotificationPlugin, example_notification_plugin, G_TYPE_OBJECT, 0,
                                G_IMPLEMENT_INTERFACE (HD_TYPE_NOTIFICATION_PLUGIN,
                                                       example_notification_plugin_notification_plugin_iface_init));
HD_PLUGIN_MODULE_SYMBOLS (example_notification_plugin);

static void
example_notification_plugin_notify (HDNotificationPlugin  *plugin,
                                    const gchar           *app_name,
                                    guint                  id,
                                    const gchar           *icon,
                                    const gchar           *summary,
                                    const gchar           *body,
                                    gchar                **actions,
                                    GHashTable            *hints,
                                    gint                   timeout)
{
  g_debug ("ExampleNotificationPlugin::notify (%s, %u, %s, %s, %s, %d)",
           app_name, id, icon, summary, body, timeout);
}

static void
example_notification_plugin_close (HDNotificationPlugin *plugin,
                                   guint                 id)
{
  g_debug ("ExampleNotificationPlugin::close (%u)",
           id);
}

static void
example_notification_plugin_notification_plugin_iface_init (HDNotificationPluginIface *iface)
{
  iface->notify = example_notification_plugin_notify;
  iface->close = example_notification_plugin_close;
}

static void
example_notification_plugin_class_finalize (ExampleNotificationPluginClass *klass)
{
}

static void
example_notification_plugin_class_init (ExampleNotificationPluginClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExampleNotificationPluginPrivate));
}

static void
example_notification_plugin_init (ExampleNotificationPlugin *plugin)
{
}
