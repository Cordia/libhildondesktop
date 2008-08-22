/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006, 2008 Nokia Corporation.
 *
 * Based on hd-plugin-manager.h from hildon-desktop.
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

#ifndef __HD_PLUGIN_MANAGER_H__
#define __HD_PLUGIN_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <libhildondesktop/hd-config-file.h>

G_BEGIN_DECLS

#define HD_TYPE_PLUGIN_MANAGER            (hd_plugin_manager_get_type ())
#define HD_PLUGIN_MANAGER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_PLUGIN_MANAGER, HDPluginManager))
#define HD_PLUGIN_MANAGER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  HD_TYPE_PLUGIN_MANAGER, HDPluginManagerClass))
#define HD_IS_PLUGIN_MANAGER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_PLUGIN_MANAGER))
#define HD_IS_PLUGIN_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  HD_TYPE_PLUGIN_MANAGER))
#define HD_PLUGIN_MANAGER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  HD_TYPE_PLUGIN_MANAGER, HDPluginManagerClass))

typedef struct _HDPluginManager        HDPluginManager;
typedef struct _HDPluginManagerClass   HDPluginManagerClass;
typedef struct _HDPluginManagerPrivate HDPluginManagerPrivate;

typedef guint (*HDLoadPriorityFunc) (const gchar *plugin_id,
                                     GKeyFile    *keyfile,
                                     gpointer     data);

struct _HDPluginManager 
{
  GObject gobject;

  HDPluginManagerPrivate *priv;
};

struct _HDPluginManagerClass 
{
  GObjectClass parent_class;

  void (*plugin_module_added)         (HDPluginManager *manager,
                                       const gchar     *plugin_id);
  void (*plugin_module_removed)       (HDPluginManager *manager,
                                       const gchar     *plugin_id);
  void (*plugin_added)                (HDPluginManager *manager,
                                       GObject         *plugin);
  void (*plugin_removed)              (HDPluginManager *manager,
                                       GObject         *plugin);
  void (*configuration_loaded)        (HDPluginManager *manager,
                                       GKeyFile        *keyfile);
  void (*plugin_configuration_loaded) (HDPluginManager *manager,
                                       GKeyFile        *keyfile);
};

GType            hd_plugin_manager_get_type                   (void);

HDPluginManager *hd_plugin_manager_new                        (HDConfigFile       *config_file,
                                                               const gchar        *safe_mode_file);

void             hd_plugin_manager_run                        (HDPluginManager    *manager);

GList *          hd_plugin_manager_get_all_plugin_paths       (HDPluginManager    *manager);

GKeyFile *       hd_plugin_manager_get_plugin_config_key_file (HDPluginManager    *manager);

void             hd_plugin_manager_set_load_priority_func     (HDPluginManager    *manager,
                                                               HDLoadPriorityFunc  load_priority_func,
                                                               gpointer            data,
                                                               GDestroyNotify      destroy);

G_END_DECLS

#endif /* __HD_PLUGIN_MANAGER_H__ */
