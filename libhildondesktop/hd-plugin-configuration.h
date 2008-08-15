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

#ifndef __HD_PLUGIN_CONFIGURATION_H__
#define __HD_PLUGIN_CONFIGURATION_H__

#include <glib.h>
#include <glib-object.h>

#include <libhildondesktop/hd-config-file.h>

G_BEGIN_DECLS

#define HD_TYPE_PLUGIN_CONFIGURATION            (hd_plugin_configuration_get_type ())
#define HD_PLUGIN_CONFIGURATION(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_PLUGIN_CONFIGURATION, HDPluginConfiguration))
#define HD_PLUGIN_CONFIGURATION_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  HD_TYPE_PLUGIN_CONFIGURATION, HDPluginConfigurationClass))
#define HD_IS_PLUGIN_CONFIGURATION(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_PLUGIN_CONFIGURATION))
#define HD_IS_PLUGIN_CONFIGURATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  HD_TYPE_PLUGIN_CONFIGURATION))
#define HD_PLUGIN_CONFIGURATION_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  HD_TYPE_PLUGIN_CONFIGURATION, HDPluginConfigurationClass))

typedef struct _HDPluginInfo HDPluginInfo;

struct _HDPluginInfo
{
  gchar    *plugin_id;
  gchar    *desktop_file;
  guint     priority;
  gpointer  item;
};

typedef struct _HDPluginConfiguration        HDPluginConfiguration;
typedef struct _HDPluginConfigurationClass   HDPluginConfigurationClass;
typedef struct _HDPluginConfigurationPrivate HDPluginConfigurationPrivate;

struct _HDPluginConfiguration 
{
  HDConfigFile parent_instance;

  HDPluginConfigurationPrivate *priv;
};

struct _HDPluginConfigurationClass 
{
  HDConfigFileClass parent_class;
};

GType                  hd_plugin_info_get_type                     (void);

HDPluginInfo          *hd_plugin_info_new                          (const gchar           *plugin_id,
                                                                    const gchar           *desktop_file,
                                                                    guint                  priority);
HDPluginInfo          *hd_plugin_info_copy                         (const HDPluginInfo    *plugin_definition);
void                   hd_plugin_info_free                         (HDPluginInfo          *plugin_definition);


GType                  hd_plugin_configuration_get_type            (void);

HDPluginConfiguration *hd_plugin_configuration_new                 (const gchar           *system_conf_dir,
                                                                    const gchar           *user_conf_dir,
                                                                    const gchar           *filename);
HDPluginConfiguration *hd_plugin_configuration_new_for_config_file (HDConfigFile          *config_file,
                                                                    const gchar           *filename);


GList                 *hd_plugin_configuration_get_plugins         (HDPluginConfiguration *configuration,
                                                                    gboolean               force_system_config);

G_END_DECLS

#endif /* __HD_PLUGIN_CONFIGURATION_H__ */
