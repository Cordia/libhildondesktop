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

#include <glib.h>
#include <glib-object.h>

#include "hd-plugin-configuration.h"

G_DEFINE_TYPE (HDPluginConfiguration, hd_plugin_configuration, HD_TYPE_CONFIG_FILE);

GType
hd_plugin_info_get_type (void)
{
  static GType type = 0;

  if (G_UNLIKELY (!type))
    {
      type = g_boxed_type_register_static ("HDPluginInfo",
                                           (GBoxedCopyFunc) hd_plugin_info_copy,
                                           (GBoxedFreeFunc) hd_plugin_info_free);
    }

  return type;
}

HDPluginInfo *
hd_plugin_info_new (const gchar *plugin_id,
                    const gchar *desktop_file,
                    guint        priority)
{
  HDPluginInfo *new_plugin_info = g_slice_new0 (HDPluginInfo);

  if (plugin_id)
    new_plugin_info->plugin_id = g_strdup (plugin_id);
  if (desktop_file)
    new_plugin_info->desktop_file = g_strdup (desktop_file);
  new_plugin_info->priority = priority;

  return new_plugin_info;
}

HDPluginInfo *
hd_plugin_info_copy (const HDPluginInfo *plugin_info)
{
  HDPluginInfo *new_plugin_info = g_slice_new0 (HDPluginInfo);

  if (plugin_info->plugin_id)
    new_plugin_info->plugin_id = g_strdup (plugin_info->plugin_id);
  if (plugin_info->desktop_file)
    new_plugin_info->desktop_file = g_strdup (plugin_info->desktop_file);
  new_plugin_info->priority = plugin_info->priority;

  return new_plugin_info;
}

void
hd_plugin_info_free (HDPluginInfo *plugin_info)
{
  g_free (plugin_info->plugin_id);
  g_free (plugin_info->desktop_file);
  g_slice_free (HDPluginInfo, plugin_info);
}

static void
hd_plugin_configuration_class_init (HDPluginConfigurationClass *klass)
{
}

static void
hd_plugin_configuration_init (HDPluginConfiguration *configuration)
{
}

HDPluginConfiguration *
hd_plugin_configuration_new (const gchar *system_conf_dir,
                             const gchar *user_conf_dir,
                             const gchar *filename)
{
  return g_object_new (HD_TYPE_PLUGIN_CONFIGURATION,
                       "system-conf-dir", system_conf_dir,
                       "user-conf-dir", user_conf_dir,
                       "filename", filename,
                       NULL);
}

HDPluginConfiguration *
hd_plugin_configuration_new_for_config_file (HDConfigFile *config_file,
                                             const gchar *filename)
{
  gchar *system_conf_dir, *user_conf_dir;

  g_object_get (G_OBJECT (config_file),
                "system-conf-dir", &system_conf_dir,
                "user-conf-dir", &user_conf_dir,
                NULL);

  return g_object_new (HD_TYPE_PLUGIN_CONFIGURATION,
                       "system-conf-dir", system_conf_dir,
                       "user-conf-dir", user_conf_dir,
                       "filename", filename,
                       NULL);
}

GList *
hd_plugin_configuration_get_plugins (HDPluginConfiguration *configuration,
                                     gboolean               force_system_config)
{
  GKeyFile *keyfile;
  gchar **groups;
  guint i;
  GList *plugins = NULL;

  keyfile = hd_config_file_load_file (HD_CONFIG_FILE (configuration), force_system_config);

  groups = g_key_file_get_groups (keyfile, NULL);

  /* return NULL if file is empty */
  if (groups == NULL)
    {
      g_key_file_free (keyfile);

      return NULL;
    }

  /* iterate over all groups */
  for (i = 0; groups[i]; i++)
    {
      gchar *desktop_file;
      guint priority;
      GError *error = NULL;

      /* ignore if X-Load==false */
      if (g_key_file_has_key (keyfile, groups[i], "X-Load", NULL))
        if (!g_key_file_get_boolean (keyfile, groups[i], "X-Load", NULL))
          continue;

      desktop_file = g_key_file_get_string (keyfile, groups[i], "X-Desktop-File", NULL);
      if (desktop_file == NULL)
        {
          g_warning ("No X-Desktop-File entry for plugin %s.", groups[i]);
          continue;
        }

      priority = (guint) g_key_file_get_integer (keyfile, groups[i], "X-Priority", &error);
      if (error != NULL)
        {
          priority = G_MAXUINT;
          g_error_free (error);
        }

      plugins = g_list_prepend (plugins, hd_plugin_info_new (groups[i],
                                                             desktop_file,
                                                             priority));
    }

  g_key_file_free (keyfile);

  return plugins;
}

