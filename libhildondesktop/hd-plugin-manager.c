/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006, 2008 Nokia Corporation.
 *
 * Based on hd-desktop.c and hd-plugin-manager.c from hildon-desktop.
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
#include <gdk/gdk.h>

#include <string.h>

#include "hd-config.h"
#include "hd-plugin-loader.h"
#include "hd-plugin-loader-factory.h"
#include "hd-stamp-file.h"

#include "hd-plugin-manager.h"

#define HD_PLUGIN_MANAGER_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_PLUGIN_MANAGER, HDPluginManagerPrivate))

#define HD_PLUGIN_MANAGER_CONFIG_GROUP                    "X-PluginManager"
#define HD_PLUGIN_MANAGER_CONFIG_KEY_DEBUG_PLUGINS        "X-Debug-Plugins"
#define HD_PLUGIN_MANAGER_CONFIG_KEY_LOAD_ALL_PLUGINS     "X-Load-All-Plugins"
#define HD_PLUGIN_MANAGER_CONFIG_KEY_PLUGIN_CONFIGURATION "X-Plugin-Configuration"
#define HD_PLUGIN_MANAGER_CONFIG_KEY_SAFE_SET             "X-Safe-Set"

/* PluginInfo struct */
typedef struct _HDPluginInfo HDPluginInfo;

struct _HDPluginInfo
{
  gchar    *plugin_id;
  gchar    *desktop_file;
  guint     priority;
  gpointer  item;
};

static HDPluginInfo *hd_plugin_info_new  (const gchar           *plugin_id,
                                          const gchar           *desktop_file,
                                          guint                  priority);
static void          hd_plugin_info_free (HDPluginInfo          *plugin_definition);

static void hd_plugin_manager_items_configuration_loaded (HDPluginConfiguration *configuration,
                                                          GKeyFile              *keyfile);

static gint cmp_info_plugin_id (const HDPluginInfo *a, const HDPluginInfo *b);

enum
{
  PLUGIN_ADDED,
  PLUGIN_REMOVED,
  LAST_SIGNAL
};

struct _HDPluginManagerPrivate 
{
  GObject                *factory;

  GList                  *plugins;

  HDLoadPriorityFunc      load_priority_func;
  gpointer                load_priority_data;
  GDestroyNotify          load_priority_destroy;

  gboolean                load_new_plugins;
  gboolean                load_all_plugins;

  gchar                 **debug_plugins;

  gchar                  *safe_set;
};

static guint plugin_manager_signals [LAST_SIGNAL] = { 0 };

/** 
 * SECTION:hd-plugin-manager
 * @short_description: Manages plugins defined by configuration files
 *
 * A #HDPluginManager manages plugins defined in configuration files.
 *
 * The configuration is read from the configuration file specified on creation
 * of the #HDPluginManager instance.
 *
 * To use the #HDPluginManager connect to the #HDPluginManager::plugin-added and
 * #HDPluginManager::plugin-removed signals. These signals are emitted if a plugin
 * should be added or removed from the application.
 *
 * Calling hd_plugin_manager_run() results in an initial read of the configuration files
 * in which the #HDPluginManager::plugin-added is emitted for each plugin which
 * is loaded.
 *
 *
 * 
 **/

G_DEFINE_TYPE (HDPluginManager, hd_plugin_manager, HD_TYPE_PLUGIN_CONFIGURATION);

static void
delete_plugin (gpointer  data,
               GObject  *object_pointer)
{
  HDPluginInfo *info = ((GList *) data)->data;

  hd_plugin_info_free (info);
  ((GList *) data)->data = NULL;
}

static void
hd_plugin_manager_remove_plugin_module (HDPluginManager *manager,
                                        const gchar     *desktop_file)
{
  HDPluginManagerPrivate *priv = manager->priv;
  GList *p;

  /* remove all plugins with desktop_file */
  for (p = priv->plugins; p; )
    {
      GList *next = p->next;
      HDPluginInfo *info = p->data;

      if (!info)
        continue;

      if (!strcmp (info->desktop_file, desktop_file))
        {
          g_object_weak_unref (G_OBJECT (info->item), delete_plugin, p);
          priv->plugins = g_list_delete_link (priv->plugins, p);
          g_signal_emit (manager, plugin_manager_signals[PLUGIN_REMOVED], 0, info->item);
        }

      p = next;
    }
}

static void
hd_plugin_manager_remove_plugin (HDPluginManager *manager,
                                 const gchar     *plugin_id)
{
  HDPluginManagerPrivate *priv = manager->priv;
  GList *p;

  /* Remove the plugin with id plugin_id*/
  for (p = priv->plugins; p; p = p->next)
    {
      HDPluginInfo *info = p->data;

      if (!info)
        continue;

      if (!strcmp (info->plugin_id, plugin_id))
        {
          g_object_weak_unref (G_OBJECT (info->item), delete_plugin, p);
          priv->plugins = g_list_delete_link (priv->plugins, p);
          g_signal_emit (manager, plugin_manager_signals[PLUGIN_REMOVED], 0, info->item);
          /* There is only one such plugin */
          return;
        }
    }
}

typedef struct
{
  HDPluginManager *manager;
  gchar           *desktop_file;
  gchar           *plugin_id;
} HDPluginManagerLoadPluginData;

static gboolean
load_plugin_idle (gpointer idle_data)
{
  HDPluginManagerLoadPluginData *data = idle_data;
  HDPluginManager *manager = data->manager;
  gchar *desktop_file = data->desktop_file;
  gchar *plugin_id = data->plugin_id;
  HDPluginManagerPrivate *priv;
  HDPluginInfo *info;
  GList *p;
  GObject *plugin;
  GError *error = NULL;

  priv = HD_PLUGIN_MANAGER (manager)->priv;

  if (!g_file_test (desktop_file, G_FILE_TEST_EXISTS))
    {
      g_warning ("%s. Plugin desktop file %s not found. Ignoring plugin",
                 __FUNCTION__,
                 desktop_file);
      goto cleanup;
    }

  g_debug ("%s. Try to load plugin_id: %s", __FUNCTION__, plugin_id);

  info = hd_plugin_info_new (plugin_id,
                             desktop_file,
                             0);

  if (g_list_find_custom (priv->plugins,
                          info,
                          (GCompareFunc) cmp_info_plugin_id))
    {
      /* plugin already loaded*/
      g_debug ("%s. Plugin with id %s already loaded.",
               __FUNCTION__,
               plugin_id);

      hd_plugin_info_free (info);

      goto cleanup;
    }

  plugin = hd_plugin_loader_factory_create (HD_PLUGIN_LOADER_FACTORY (manager->priv->factory), 
                                            plugin_id,
                                            desktop_file,
                                            &error);
  if (!plugin)
    {
      if (error)
        {
          g_warning ("Error loading plugin: %s. %s", desktop_file, error->message);
          g_error_free (error);
        }
      else
        {
          g_warning ("Error loading plugin: %s", desktop_file);
        }

      hd_plugin_info_free (info);

      goto cleanup;
    }

  info->item = plugin;

  g_debug ("%s Loaded plugin: %s",
           __FUNCTION__,
           info->desktop_file);

  p = g_list_append (NULL, info);
  priv->plugins = g_list_concat (priv->plugins, p);

  g_object_weak_ref (G_OBJECT (plugin), delete_plugin, p);

  g_signal_emit (manager, plugin_manager_signals[PLUGIN_ADDED], 0, plugin);

cleanup:
  g_object_unref (data->manager);
  g_free (data->desktop_file);
  g_free (data->plugin_id);
  g_slice_free (HDPluginManagerLoadPluginData, data);

  return FALSE;
}

static gboolean 
hd_plugin_manager_load_plugin (HDPluginManager *manager,
                               const gchar     *desktop_file,
                               const gchar     *plugin_id)
{
  HDPluginManagerLoadPluginData *data;

  g_return_val_if_fail (HD_IS_PLUGIN_MANAGER (manager), FALSE);
  g_return_val_if_fail (desktop_file != NULL, FALSE);
  g_return_val_if_fail (plugin_id != NULL, FALSE);

  data = g_slice_new0 (HDPluginManagerLoadPluginData);
  data->manager = g_object_ref (manager);
  data->desktop_file = g_strdup (desktop_file);
  data->plugin_id = g_strdup (plugin_id);

  gdk_threads_add_idle_full (G_PRIORITY_HIGH_IDLE,
                             load_plugin_idle,
                             data,
                             NULL);

  return TRUE;
}

static void
hd_plugin_manager_plugin_module_added (HDPluginConfiguration *configuration,
                                       const gchar           *desktop_file)
{
  HDPluginManager *manager;
  HDPluginManagerPrivate *priv;
  GKeyFile *items_file;

  g_return_if_fail (HD_IS_PLUGIN_MANAGER (configuration));

  manager = HD_PLUGIN_MANAGER (configuration);
  priv = manager->priv;

  /* Try to load plugins in the items file where loading failed */
  items_file = hd_plugin_configuration_get_items_key_file (configuration);
  hd_plugin_manager_items_configuration_loaded (configuration,
                                                items_file);

  /* Load new plugin if configured to do so */
  if (priv->load_new_plugins && !hd_stamp_file_get_safe_mode ())
    {
      gchar *plugin_id;

      plugin_id = g_path_get_basename (desktop_file);

      /* Remove old plugins first */
      hd_plugin_manager_remove_plugin_module (manager, desktop_file);
      hd_plugin_manager_load_plugin (manager, desktop_file, plugin_id);

      g_free (plugin_id);
    }
  else if (priv->debug_plugins != NULL)
    {
      gchar *plugin_id;
      guint i;

      plugin_id = g_path_get_basename (desktop_file);

      for (i = 0; priv->debug_plugins[i]; i++)
        {
          if (strcmp (plugin_id, priv->debug_plugins[i]))
            hd_plugin_manager_load_plugin (manager, desktop_file, plugin_id);
        }

      g_free (plugin_id);
    }
}

static void
hd_plugin_manager_plugin_module_removed (HDPluginConfiguration *configuration,
                                         const gchar           *desktop_file)
{
  hd_plugin_manager_remove_plugin_module (HD_PLUGIN_MANAGER (configuration),
                                          desktop_file);
}

static void
hd_plugin_manager_plugin_module_updated (HDPluginConfiguration *configuration,
                                         const gchar           *desktop_file)
{
  HDPluginManager *manager = HD_PLUGIN_MANAGER (configuration);
  HDPluginManagerPrivate *priv = manager->priv;
  GList *p, *plugin_ids = NULL;
  GKeyFile *items_file;

  /* remove all plugins with desktop_file */
  for (p = priv->plugins; p; )
    {
      GList *next = p->next;
      HDPluginInfo *info = p->data;

      if (!info)
        continue;

      if (!strcmp (info->desktop_file, desktop_file))
        {
          plugin_ids = g_list_prepend (plugin_ids, g_strdup (info->plugin_id));

          g_object_weak_unref (G_OBJECT (info->item), delete_plugin, p);
          priv->plugins = g_list_delete_link (priv->plugins, p);
          g_signal_emit (manager, plugin_manager_signals[PLUGIN_REMOVED], 0, info->item);
        }

      p = next;
    }

  /* readd them again */
  for (p = plugin_ids; p; p = p->next)
    {
      gchar *plugin_id = p->data;

      hd_plugin_manager_load_plugin (manager, desktop_file, plugin_id);
      
      g_free (plugin_id);
    }

  g_list_free (plugin_ids);

  /* Try to load plugins in the items file where loading failed */
  items_file = hd_plugin_configuration_get_items_key_file (configuration);
  hd_plugin_manager_items_configuration_loaded (configuration,
                                                items_file);
}

static void
hd_plugin_manager_init (HDPluginManager *manager)
{
  manager->priv = HD_PLUGIN_MANAGER_GET_PRIVATE (manager);

  manager->priv->factory = hd_plugin_loader_factory_new (); 

  g_signal_connect (manager, "plugin-module-updated",
                    G_CALLBACK (hd_plugin_manager_plugin_module_updated), NULL);
}

static void
hd_plugin_manager_plugin_added (HDPluginManager *manager,
                                GObject         *plugin)
{
}

static void
hd_plugin_manager_plugin_removed (HDPluginManager *manager,
                                  GObject         *plugin)
{
}

static void
hd_plugin_manager_finalize (GObject *object)
{
  HDPluginManagerPrivate *priv;

  g_return_if_fail (HD_IS_PLUGIN_MANAGER (object));

  priv = HD_PLUGIN_MANAGER (object)->priv;

  if (priv->factory)
    {
      g_object_unref (priv->factory);
      priv->factory = NULL;
    }

  g_strfreev (priv->debug_plugins);
  priv->debug_plugins = NULL;

  priv->safe_set = (g_free (priv->safe_set), NULL);

  if (priv->load_priority_data && priv->load_priority_destroy)
    {
      priv->load_priority_destroy (priv->load_priority_data);
      priv->load_priority_data = NULL;
    }

  G_OBJECT_CLASS (hd_plugin_manager_parent_class)->finalize (object);
}

/* Compare lists new and old and move elements unique in old
 * to to_remove and elements unique in new to to_add, elements
 * common in new and old are removed.
 *
 * old is destroyed by this function
 * new is destroyed by this function
 */
static void
create_sync_lists (GList          *old,
                   GList          *new,
                   GList         **to_add,
                   GList         **to_remove,
                   GCompareFunc    cmp_func,
                   GDestroyNotify  destroy_func)
{
  GList *add = NULL;
  GList *remove = NULL;

  g_return_if_fail (to_add != NULL);
  g_return_if_fail (to_remove != NULL);

  /* sort lists */
  old = g_list_sort (old, cmp_func);
  new = g_list_sort (new, cmp_func);

  while (old && new)
    {
      gint c = cmp_func (old->data, new->data);

      /* there is an element only in new 
       * move it to list to_add */
      if (c > 0)
        {
          GList *n = new;
          new = g_list_remove_link (new, new);
          add = g_list_concat (n, add);
        }
      /* there is an element only in old 
       * move it to list to_remove */
      else if (c < 0)
        {
          GList *o = old;
          old = g_list_remove_link (old, old);
          remove = g_list_concat (o, remove);
        }
      /* the next element is in old and new
       * remove it */
      else
        {
          destroy_func (old->data);
          destroy_func (new->data);

          old = g_list_delete_link (old, old);
          new = g_list_delete_link (new, new);
        }
    }

  /* add remaining elements to the approbiate lists */
  *to_add = g_list_concat (new, add);
  *to_remove = g_list_concat (old, remove);
}

/* Compare plugin id and desktop file */
static gint
cmp_info_plugin_id (const HDPluginInfo *a,
                    const HDPluginInfo *b)
{
  gint result;

  result = g_strcmp0 (a->plugin_id, b->plugin_id);

  if (result)
    return result;

  /* If plugin IDs are the same compare the desktop_file */
  return strcmp (a->desktop_file, b->desktop_file);
}

/* Compare desktop file */
static gint
cmp_info_desktop_file (const HDPluginInfo *a,
                       const HDPluginInfo *b)
{
  return strcmp (a->desktop_file, b->desktop_file);
}

/* Compare priority */
static gint
cmp_info_priority (const HDPluginInfo *a,
                   const HDPluginInfo *b)
{
  if (a->priority != b->priority)
    return a->priority - b->priority;

  return strcmp (a->plugin_id, b->plugin_id);
}

/* sync new_plugins with already loaded plugins 
 *
 * new_plugins is destroyed by this function
 */
static void
hd_plugin_manager_sync_plugins (HDPluginManager *manager,
                                GList           *new_plugins)
{
  HDPluginManagerPrivate *priv = manager->priv;
  GList *old_plugins = NULL;
  GList *p;
  GList *to_add = NULL, *to_remove = NULL;

  for (p = priv->plugins; p; p = p->next)
    {
      HDPluginInfo *info = p->data;

      if (!info)
        continue;

      old_plugins = g_list_prepend (old_plugins, hd_plugin_info_new (info->plugin_id,
                                                                     info->desktop_file,
                                                                     0));
    }

  create_sync_lists (old_plugins, new_plugins, &to_add, &to_remove,
                     (GCompareFunc) cmp_info_plugin_id, (GDestroyNotify) hd_plugin_info_free);

  /* remove plugins */
  for (p = to_remove; p; p = p->next)
    {
      HDPluginInfo *info = p->data;

      hd_plugin_manager_remove_plugin (manager, info->plugin_id);

      hd_plugin_info_free (info);
    }

  to_add = g_list_sort (to_add, (GCompareFunc) cmp_info_priority);

  /* add plugins */
  for (p = to_add; p; p = p->next)
    {
      HDPluginInfo *info = p->data;

      hd_plugin_manager_load_plugin (manager, info->desktop_file, info->plugin_id);

      hd_plugin_info_free (info);
    }

  g_list_free (to_remove);
  g_list_free (to_add);
}

static void
hd_plugin_manager_configuration_loaded (HDPluginConfiguration *configuration,
                                        GKeyFile              *keyfile)
{
  HDPluginManagerPrivate *priv = HD_PLUGIN_MANAGER (configuration)->priv;

  priv->debug_plugins = (g_strfreev (priv->debug_plugins), NULL);
  priv->safe_set = (g_free (priv->safe_set), NULL);

  /* Load configuration ([X-PluginManager] group) */
  if (!g_key_file_has_group (keyfile, HD_PLUGIN_MANAGER_CONFIG_GROUP))
    {
      g_warning ("Error configuration file doesn't contain group '%s'",
                 HD_PLUGIN_MANAGER_CONFIG_GROUP);

      return;
    }

  priv->load_new_plugins = g_key_file_get_boolean (keyfile, 
                                                   HD_PLUGIN_MANAGER_CONFIG_GROUP, 
                                                   HD_DESKTOP_CONFIG_KEY_LOAD_NEW_PLUGINS,
                                                   NULL);
  priv->load_all_plugins = g_key_file_get_boolean (keyfile, 
                                                   HD_PLUGIN_MANAGER_CONFIG_GROUP, 
                                                   HD_PLUGIN_MANAGER_CONFIG_KEY_LOAD_ALL_PLUGINS,
                                                   NULL);

  priv->debug_plugins = g_key_file_get_string_list (keyfile,
                                                    HD_PLUGIN_MANAGER_CONFIG_GROUP,
                                                    HD_PLUGIN_MANAGER_CONFIG_KEY_DEBUG_PLUGINS,
                                                    NULL,
                                                    NULL);

  priv->safe_set = g_key_file_get_string (keyfile,
                                          HD_PLUGIN_MANAGER_CONFIG_GROUP,
                                          HD_PLUGIN_MANAGER_CONFIG_KEY_SAFE_SET,
                                          NULL);

  HD_PLUGIN_CONFIGURATION_CLASS (hd_plugin_manager_parent_class)->configuration_loaded (configuration,
                                                                                        keyfile);
}

static void
hd_plugin_manager_items_configuration_loaded (HDPluginConfiguration *configuration,
                                              GKeyFile              *keyfile)
{
  HDPluginManager *manager = HD_PLUGIN_MANAGER (configuration);
  HDPluginManagerPrivate *priv = manager->priv;
  GList *new_plugins = NULL;
  gchar **safe_set = NULL;
  gboolean removed_unsafe_plugins = FALSE;
  gboolean in_startup = hd_plugin_configuration_get_in_startup (configuration);

  /* Get all plugins from the safe set file */
  if (priv->safe_set && hd_stamp_file_get_safe_mode ())
    {
      gchar *filename, *contents = NULL;
      GError *error = NULL;

      filename = g_build_filename (HD_DESKTOP_CONFIG_PATH,
                                   priv->safe_set,
                                   NULL);

      if (g_file_get_contents (filename, &contents, NULL, &error) && !error)
        {
          guint i;

          safe_set = g_strsplit (contents, "\n", 0);

          for (i = 0; safe_set && safe_set[i]; i++)
            {
              g_strstrip (safe_set[i]);
            }
        }
      else if (error)
        {
          g_warning ("%s. Could not load safe set file %s. %s",
                     __FUNCTION__,
                     filename,
                     error->message);
          g_error_free (error);
        }

      g_free (filename);
      g_free (contents);
    }

  if (keyfile)
    {
      gchar **groups;
      guint i;

      groups = g_key_file_get_groups (keyfile, NULL);

      /* Iterate over all groups if any */
      if (groups != NULL)
        {
          for (i = 0; groups[i]; i++)
            {
              gchar *desktop_file;
              guint priority = G_MAXUINT;

              /* Ignore if X-Load==false */
              if (g_key_file_has_key (keyfile, groups[i], "X-Load", NULL))
                if (!g_key_file_get_boolean (keyfile, groups[i], "X-Load", NULL))
                  continue;

              /* Get the .desktop file of the plugin */
              desktop_file = g_key_file_get_string (keyfile, groups[i], "X-Desktop-File", NULL);
              g_strstrip (desktop_file);
              if (desktop_file == NULL)
                {
                  g_warning ("No X-Desktop-File entry for plugin %s.", groups[i]);
                  continue;
                }

              /* If in safe mode and there is a separate safe set file only load plugins listed there */
              if (hd_stamp_file_get_safe_mode () && priv->safe_set && in_startup)
                {
                  guint j;
                  gboolean in_safe_set = FALSE;

                  for (j = 0; safe_set && safe_set[j]; j++)
                    {
                      if (strcmp (safe_set[j], desktop_file) == 0)
                        {
                          in_safe_set = TRUE;
                          break;
                        }
                    }

                  if (!in_safe_set)
                    {
                      GError *error = NULL;

                      /* Remove widget from installed widgets so it can be added again */
                      if (g_key_file_remove_group (keyfile,
                                                   groups[i],
                                                   &error))
                        removed_unsafe_plugins = TRUE;
                      if (error)
                        {
                          g_warning ("%s. Could not remove un-safe plugin from *.plugins Keyfile. %s",
                                     __FUNCTION__,
                                     error->message);
                          g_error_free (error);
                        }
                      g_free (desktop_file);
                      continue;
                    }
                }

              /* Get the load priority of the plugin */
              if (priv->load_priority_func)
                priority = priv->load_priority_func (groups[i], keyfile, priv->load_priority_data);

              new_plugins = g_list_prepend (new_plugins, hd_plugin_info_new (groups[i],
                                                                             desktop_file,
                                                                             priority));
              g_free (desktop_file);
            }
        }
      g_strfreev (groups);
    }

  if (priv->load_all_plugins)
    {
      /*
       * Load all plugins in the X-Plugin-Dirs directories 
       * if X-Load-All-Plugins is true and not in safe 
       */
      if (!hd_stamp_file_get_safe_mode ())
        {
          gchar **all_plugins;
          guint i;

          all_plugins = hd_plugin_configuration_get_all_plugin_paths (configuration);

          for (i = 0; all_plugins[i]; i++)
            {
              HDPluginInfo *info = hd_plugin_info_new (NULL,
                                                       all_plugins[i],
                                                       G_MAXUINT);

              if (!g_list_find_custom (new_plugins, info, (GCompareFunc) cmp_info_desktop_file))
                {
                  info->plugin_id = g_path_get_basename (info->desktop_file);
                  new_plugins = g_list_prepend (new_plugins, info);
                }
              else
                hd_plugin_info_free (info);
            }

          g_strfreev (all_plugins);
        }
      else if (priv->safe_set)
        {
          /* Load all plugins from the safe set file */
          guint i;

          for (i = 0; safe_set && safe_set[i]; i++)
            {
              HDPluginInfo *info;

              if (!safe_set[i][0])
                continue;

              info = hd_plugin_info_new (NULL,
                                         safe_set[i],
                                         G_MAXUINT);

              if (!g_list_find_custom (new_plugins, info, (GCompareFunc) cmp_info_desktop_file))
                {
                  info->plugin_id = g_path_get_basename (info->desktop_file);
                  new_plugins = g_list_prepend (new_plugins, info);
                }
              else
                hd_plugin_info_free (info);
            }
        }
    }

  /* Don't load plugins from X-Debug-Plugins list */
  if (priv->debug_plugins != NULL)
    {
      guint i;

      for (i = 0; priv->debug_plugins[i]; i++)
        {
          GList *p;

          g_strstrip (priv->debug_plugins[i]);

          for (p = new_plugins; p; )
            {
              HDPluginInfo *info = p->data;
              gchar *basename = g_path_get_basename (info->desktop_file);

              if (!strcmp (basename, priv->debug_plugins[i]))
                {
                  GList *q = p->next;

                  hd_plugin_info_free (info);
                  new_plugins = g_list_delete_link (new_plugins, p);

                  p = q;
                }
              else
                p = p->next;

              g_free (basename);
            }
        }
    }

  g_strfreev (safe_set);

  hd_plugin_manager_sync_plugins (manager, new_plugins);

  /* Unsafe plugins were removed from the configuration */
  if (removed_unsafe_plugins)
    hd_plugin_configuration_store_items_key_file (configuration);
}

static void
hd_plugin_manager_class_init (HDPluginManagerClass *klass)
{
  GObjectClass *g_object_class = (GObjectClass *) klass;
  HDPluginConfigurationClass *plugin_configuration_class = HD_PLUGIN_CONFIGURATION_CLASS (klass);

  klass->plugin_added = hd_plugin_manager_plugin_added;
  klass->plugin_removed = hd_plugin_manager_plugin_removed;

  plugin_configuration_class->plugin_module_added = hd_plugin_manager_plugin_module_added;
  plugin_configuration_class->plugin_module_removed = hd_plugin_manager_plugin_module_removed;
  plugin_configuration_class->configuration_loaded = hd_plugin_manager_configuration_loaded;
  plugin_configuration_class->items_configuration_loaded = hd_plugin_manager_items_configuration_loaded;

  g_object_class->finalize = hd_plugin_manager_finalize;

  g_type_class_add_private (g_object_class, sizeof (HDPluginManagerPrivate));

  /**
   *  HDPluginManager::plugin-added:
   *  @manager: a #HDPluginManager.
   *  @plugin: the new plugin.
   *
   *  Emitted if a new plugin instance is created (loaded).
   **/
  plugin_manager_signals [PLUGIN_ADDED] = g_signal_new ("plugin-added",
                                                        G_TYPE_FROM_CLASS (klass),
                                                        G_SIGNAL_RUN_FIRST,
                                                        G_STRUCT_OFFSET (HDPluginManagerClass, plugin_added),
                                                        NULL, NULL,
                                                        g_cclosure_marshal_VOID__OBJECT,
                                                        G_TYPE_NONE, 1,
                                                        G_TYPE_OBJECT);

  /**
   *  HDPluginManager::plugin-removed:
   *  @manager: a #HDPluginManager.
   *  @plugin: the plugin.
   *
   *  Emitted if a plugin instance is removed.
   **/
  plugin_manager_signals [PLUGIN_REMOVED] = g_signal_new ("plugin-removed",
                                                          G_TYPE_FROM_CLASS (klass),
                                                          G_SIGNAL_RUN_LAST,
                                                          G_STRUCT_OFFSET (HDPluginManagerClass,
                                                                           plugin_removed),
                                                          NULL, NULL,
                                                          g_cclosure_marshal_VOID__OBJECT,
                                                          G_TYPE_NONE, 1,
                                                          G_TYPE_OBJECT);

}

/**
 * hd_plugin_manager_new:
 * @config_file: a HDConfigFile which specify the configuration file.
 *
 * This function creates a new #HDPluginManager instance.
 *
 * Returns: a new #HDPluginManager instance.
 **/
HDPluginManager *
hd_plugin_manager_new (HDConfigFile *config_file)
{
  HDPluginManager *manager = g_object_new (HD_TYPE_PLUGIN_MANAGER,
                                           "conf-file", config_file,
                                           NULL);

  return manager;
}

/**
 * hd_plugin_manager_run:
 * @manager: a #HDPluginManager
 *
 * This function should be called after the callback signals
 * are connected to @manager. It does an initial read of the configuration
 * files, loads the plugins according to the configuration and emits the
 * appropiate callback signals.
 **/
void
hd_plugin_manager_run (HDPluginManager *manager)
{
  g_return_if_fail (HD_IS_PLUGIN_MANAGER (manager));
 
  hd_plugin_configuration_run (HD_PLUGIN_CONFIGURATION (manager));
}

/**
 * hd_plugin_manager_get_plugin_config_key_file:
 * @manager: a #HDPluginManager
 *
 * This function can be used in the HDPluginManager::plugin-added and
 * HDPluginManager::plugin-configuration-loaded to get a reference
 * of the plugin configuration key file.
 *
 * Returns: a reference to the plugin configuration key file. It is owned by the manager and must not be freed.
 **/
GKeyFile *
hd_plugin_manager_get_plugin_config_key_file (HDPluginManager *manager)
{
  return hd_plugin_configuration_get_items_key_file (HD_PLUGIN_CONFIGURATION (manager));
}

/**
 * hd_plugin_manager_set_load_priority_func:
 * @manager: a #HDPluginManager
 * @load_priority_func: a #HDLoadPriorityFunc which should be used to calculate the load priority of a plugin
 * @data: data which is passed to @load_priority_func
 * @destroy: function to call when @data should be destroyed
 *
 * This function allows it to specify a function which calculates the priority in which plugins
 * are loaded. The lower the unsigned integer returned by @load_priority_func is the earlier
 * the plugin is loaded.
 **/
void
hd_plugin_manager_set_load_priority_func (HDPluginManager    *manager,
                                          HDLoadPriorityFunc  load_priority_func,
                                          gpointer            data,
                                          GDestroyNotify      destroy)
{
  HDPluginManagerPrivate *priv = manager->priv;

  g_return_if_fail (HD_IS_PLUGIN_MANAGER (manager));

  /* Destroy old data */
  if (priv->load_priority_data && priv->load_priority_destroy)
    priv->load_priority_destroy (priv->load_priority_data);

  priv->load_priority_func = load_priority_func;
  priv->load_priority_data = data;
  priv->load_priority_destroy = destroy;
}

/* PluginInfo */
static HDPluginInfo *
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

static void
hd_plugin_info_free (HDPluginInfo *plugin_info)
{
  g_free (plugin_info->plugin_id);
  g_free (plugin_info->desktop_file);
  g_slice_free (HDPluginInfo, plugin_info);
}

