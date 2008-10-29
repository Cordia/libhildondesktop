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
#include <libgnomevfs/gnome-vfs.h>

#include <string.h>

#include "hd-config-file.h"

#define HD_CONFIG_FILE_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_CONFIG_FILE, HDConfigFilePrivate))

enum
{
  PROP_0,
  PROP_SYSTEM_CONF_DIR,
  PROP_USER_CONF_DIR,
  PROP_FILENAME,
};

enum
{
  CHANGED,
  LAST_SIGNAL
};

struct _HDConfigFilePrivate 
{
  gchar                 *system_conf_dir;
  gchar                 *user_conf_dir;
  gchar                 *filename;

  GnomeVFSMonitorHandle *system_conf_monitor;
  GnomeVFSMonitorHandle *user_conf_monitor;
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (HDConfigFile, hd_config_file, G_TYPE_INITIALLY_UNOWNED);

static void
hd_config_file_monitored_dir_changed (GnomeVFSMonitorHandle    *handle,
                                      const gchar              *monitor_uri,
                                      const gchar              *info_uri,
                                      GnomeVFSMonitorEventType  event_type,
                                      HDConfigFile             *config_file)
{
  gchar *basename;
  HDConfigFilePrivate *priv;

  priv = config_file->priv;

  basename = g_path_get_basename (info_uri);
  if (!strcmp (basename, priv->filename))
    {
      g_signal_emit (config_file, signals[CHANGED], 0);
    }
  g_free (basename);
}

static GObject *
hd_config_file_constructor (GType                  type,
                            guint                  n_construct_params,
                            GObjectConstructParam *construct_params)
{
  GObject *obj;
  HDConfigFilePrivate *priv;

  obj = G_OBJECT_CLASS (hd_config_file_parent_class)->constructor (type,
                                                                   n_construct_params,
                                                                   construct_params);

  priv = HD_CONFIG_FILE (obj)->priv;

  if (priv->system_conf_dir != NULL)
    gnome_vfs_monitor_add (&priv->system_conf_monitor,
                           priv->system_conf_dir,
                           GNOME_VFS_MONITOR_DIRECTORY,
                           (GnomeVFSMonitorCallback) hd_config_file_monitored_dir_changed,
                           obj);
  if (priv->user_conf_dir != NULL)
    gnome_vfs_monitor_add (&priv->user_conf_monitor,
                           priv->user_conf_dir,
                           GNOME_VFS_MONITOR_DIRECTORY,
                           (GnomeVFSMonitorCallback) hd_config_file_monitored_dir_changed,
                           obj);

  return obj;
}

static void
hd_config_file_finalize (GObject *object)
{
  HDConfigFilePrivate *priv;

  g_return_if_fail (HD_IS_CONFIG_FILE (object));

  priv = HD_CONFIG_FILE (object)->priv;

  g_free (priv->system_conf_dir);
  priv->system_conf_dir = NULL;

  g_free (priv->user_conf_dir);
  priv->user_conf_dir = NULL;

  g_free (priv->filename);
  priv->filename = NULL;

  if (priv->system_conf_monitor)
    gnome_vfs_monitor_cancel (priv->system_conf_monitor);
  priv->system_conf_monitor = NULL;

  if (priv->user_conf_monitor)
    gnome_vfs_monitor_cancel (priv->user_conf_monitor);
  priv->user_conf_monitor = NULL;

  G_OBJECT_CLASS (hd_config_file_parent_class)->finalize (object);
}

static void
hd_config_file_set_property (GObject      *object,
                             guint         prop_id,
                             const GValue *value,
                             GParamSpec   *pspec)
{
  HDConfigFilePrivate *priv;

  priv = HD_CONFIG_FILE (object)->priv;

  switch (prop_id)
    {
    case PROP_SYSTEM_CONF_DIR:
      g_free (priv->system_conf_dir);
      priv->system_conf_dir = g_value_dup_string (value);
      break;

    case PROP_USER_CONF_DIR:
      g_free (priv->user_conf_dir);
      priv->user_conf_dir = g_value_dup_string (value);
      break;

    case PROP_FILENAME:
      g_free (priv->filename);
      priv->filename = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_config_file_get_property (GObject      *object,
                             guint         prop_id,
                             GValue       *value,
                             GParamSpec   *pspec)
{
  HDConfigFilePrivate *priv;

  priv = HD_CONFIG_FILE (object)->priv;

  switch (prop_id)
    {
    case PROP_SYSTEM_CONF_DIR:
      g_value_set_string (value, priv->system_conf_dir);
      break;

    case PROP_USER_CONF_DIR:
      g_value_set_string (value, priv->user_conf_dir);
      break;

    case PROP_FILENAME:
      g_value_set_string (value, priv->filename);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_config_file_class_init (HDConfigFileClass *klass)
{
  GObjectClass *g_object_class = (GObjectClass *) klass;

  g_object_class->constructor = hd_config_file_constructor;
  g_object_class->finalize = hd_config_file_finalize;
  g_object_class->get_property = hd_config_file_get_property;
  g_object_class->set_property = hd_config_file_set_property;

  g_type_class_add_private (g_object_class, sizeof (HDConfigFilePrivate));

  g_object_class_install_property (g_object_class,
                                   PROP_SYSTEM_CONF_DIR,
                                   g_param_spec_string ("system-conf-dir",
                                                        "System Conf Dir",
                                                        "System configuration directory",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (g_object_class,
                                   PROP_USER_CONF_DIR,
                                   g_param_spec_string ("user-conf-dir",
                                                        "User Conf Dir",
                                                        "User configuration directory",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (g_object_class,
                                   PROP_FILENAME,
                                   g_param_spec_string ("filename",
                                                        "Filename",
                                                        "Configuration filename",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

  signals [CHANGED] = g_signal_new ("changed",
                                    G_TYPE_FROM_CLASS (klass),
                                    G_SIGNAL_RUN_FIRST,
                                    G_STRUCT_OFFSET (HDConfigFileClass, changed),
                                    NULL, NULL,
                                    g_cclosure_marshal_VOID__VOID,
                                    G_TYPE_NONE, 0);
}

static void
hd_config_file_init (HDConfigFile *config_file)
{
  config_file->priv = HD_CONFIG_FILE_GET_PRIVATE (config_file);
}

HDConfigFile *
hd_config_file_new (const gchar *system_conf_dir,
                    const gchar *user_conf_dir,
                    const gchar *filename)
{
  return g_object_new (HD_TYPE_CONFIG_FILE,
                       "system-conf-dir", system_conf_dir,
                       "user-conf-dir", user_conf_dir,
                       "filename", filename,
                       NULL);
}

GKeyFile *
hd_config_file_load_file (HDConfigFile *config_file,
                          gboolean      force_system_config)
{
  HDConfigFilePrivate *priv;
  GKeyFile *key_file;
  gchar *filename;

  priv = config_file->priv;

  key_file = g_key_file_new ();

  if (priv->user_conf_dir && priv->filename && !force_system_config)
    {
      filename = g_build_filename (priv->user_conf_dir, priv->filename, NULL);
      if (g_file_test (filename, G_FILE_TEST_EXISTS))
        {
          GError *error = NULL;

          if (g_key_file_load_from_file (key_file,
                                         filename,
                                         G_KEY_FILE_NONE,
                                         &error))
            {
              g_free (filename);
              return key_file;
            }
          else
            {
              g_warning ("Couldn't read configuration file: %s. Error: %s", filename, error->message);
              g_error_free (error);
            }
        }
      g_free (filename);
    }

  if (priv->system_conf_dir && priv->filename)
    {
      filename = g_build_filename (priv->system_conf_dir, priv->filename, NULL);
      if (g_file_test (filename, G_FILE_TEST_EXISTS))
        {
          GError *error = NULL;

          if (g_key_file_load_from_file (key_file,
                                         filename,
                                         G_KEY_FILE_NONE,
                                         &error))
            {
              g_free (filename);
              return key_file;
            }
          else
            {
              g_warning ("Couldn't read configuration file: %s. Error: %s", filename, error->message);
              g_error_free (error);
            }
        }
      g_free (filename);
    }

  g_key_file_free (key_file);

  return NULL;
}

