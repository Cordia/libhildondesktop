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

#ifndef __EXAMPLE_NOTIFICATION_PLUGIN_H__
#define __EXAMPLE_NOTIFICATION_PLUGIN_H__

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define EXAMPLE_TYPE_NOTIFICATION_PLUGIN            (example_notification_plugin_get_type ())
#define EXAMPLE_NOTIFICATION_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXAMPLE_TYPE_NOTIFICATION_PLUGIN, ExampleNotificationPlugin))
#define EXAMPLE_NOTIFICATION_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXAMPLE_TYPE_NOTIFICATION_PLUGIN, ExampleNotificationPluginClass))
#define EXAMPLE_IS_NOTIFICATION_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXAMPLE_TYPE_NOTIFICATION_PLUGIN))
#define EXAMPLE_IS_NOTIFICATION_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXAMPLE_TYPE_NOTIFICATION_PLUGIN))
#define EXAMPLE_NOTIFICATION_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXAMPLE_TYPE_NOTIFICATION_PLUGIN, ExampleNotificationPluginClass))

typedef struct _ExampleNotificationPlugin        ExampleNotificationPlugin;
typedef struct _ExampleNotificationPluginClass   ExampleNotificationPluginClass;
typedef struct _ExampleNotificationPluginPrivate ExampleNotificationPluginPrivate;

struct _ExampleNotificationPlugin
{
  GObject       parent;

  ExampleNotificationPluginPrivate       *priv;
};

struct _ExampleNotificationPluginClass
{
  GObjectClass  parent;
};

GType      example_notification_plugin_get_type (void);

G_END_DECLS

#endif
