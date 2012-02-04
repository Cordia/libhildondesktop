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
#include <glib/gstdio.h>
#include <glib-object.h>
#include <gio/gio.h>

#include <string.h>
#include <errno.h>

#include "hd-config-file.h"

/* use config dir (~/.config/hildon-desktop) */
#define HD_DESKTOP_USER_CONFIG_PATH "hildon-desktop"

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
  gchar        *system_conf_dir;
  gchar        *user_conf_dir;
  gchar        *filename;

  GFileMonitor *system_conf_monitor;
  GFileMonitor *user_conf_monitor;
  GFile        *system_conf_file;
  GFile        *user_conf_file;
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE (HDConfigFile, hd_config_file, G_TYPE_INITIALLY_UNOWNED);

static void
hd_config_file_monitored_dir_changed (GFileMonitor      *monitor,
                                      GFile             *monitor_file,
                                      GFile             *info,
                                      GFileMonitorEvent  event_type,
                                      HDConfigFile      *config_file)
{
  gchar *basename;
  HDConfigFilePrivate *priv;
  gchar *info_uri = g_file_get_uri (info);

  priv = config_file->priv;

  basename = g_path_get_basename (info_uri);
  if (!strcmp (basename, priv->filename))
    {
      g_signal_emit (config_file, signals[CHANGED], 0);
    }
  g_free (info_uri);
  g_free (basename);
}

static void
hd_config_file_constructed (GObject *object)
{
  HDConfigFilePrivate *priv = HD_CONFIG_FILE (object)->priv;

  if (priv->system_conf_dir != NULL)
    {
      priv->system_conf_file = g_file_new_for_path (priv->system_conf_dir);

      priv->system_conf_monitor =
        g_file_monitor_directory (priv->system_conf_file,
                                  G_FILE_MONITOR_NONE,
                                  NULL,NULL);

      g_signal_connect (G_OBJECT (priv->system_conf_monitor),
                        "changed",
                        G_CALLBACK (hd_config_file_monitored_dir_changed),
                        (gpointer)object);
    }

  if (priv->user_conf_dir != NULL)
    {
      /* Try to create the user config dir if it not exist yet */
      if (!g_mkdir_with_parents (priv->user_conf_dir,
                                 S_IRWXU |
                                 S_IRGRP | S_IXGRP |
                                 S_IROTH | S_IXOTH))
        {
          /* There exist an user config dir, try to monitor */
          priv->user_conf_file = g_file_new_for_path (priv->user_conf_dir);

          priv->user_conf_monitor =
            g_file_monitor_directory (priv->user_conf_file,
                                      G_FILE_MONITOR_NONE,
                                      NULL,NULL);

          g_signal_connect (G_OBJECT (priv->user_conf_monitor),
                            "changed",
                            G_CALLBACK (hd_config_file_monitored_dir_changed),
                            (gpointer)object);
        }
      else
        {
          /* User config dir could not be created */
          g_warning ("Could not mkdir '%s', %s",
                     priv->user_conf_dir,
                     g_strerror (errno));
        }
    }
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
    {
      g_file_monitor_cancel (priv->system_conf_monitor);
      g_object_unref (priv->system_conf_monitor);
    }
  priv->system_conf_monitor = NULL;
  if (priv->system_conf_file)
    {
      g_object_unref (priv->system_conf_file);
    }
  priv->system_conf_file = NULL;

  if (priv->user_conf_monitor)
    {
      g_file_monitor_cancel (priv->user_conf_monitor);
      g_object_unref (priv->user_conf_monitor);
    }
  priv->user_conf_monitor = NULL;
  if (priv->user_conf_file)
    {
      g_object_unref (priv->user_conf_file);
    }
  priv->user_conf_file = NULL;

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

  g_object_class->constructed = hd_config_file_constructed;
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

  config_file->priv->system_conf_monitor = NULL;
  config_file->priv->system_conf_file = NULL;

  config_file->priv->user_conf_monitor = NULL;
  config_file->priv->user_conf_file = NULL;
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

HDConfigFile *
hd_config_file_new_with_defaults (const gchar *filename)
{
  HDConfigFile *config_file;
  gchar *user_conf_dir;

  /* Default user config dir (~/.config/hildon-desktop) */
  user_conf_dir = g_build_filename (g_get_user_config_dir (),
                                    HD_DESKTOP_USER_CONFIG_PATH,
                                    NULL);

  config_file = g_object_new (HD_TYPE_CONFIG_FILE,
                              "system-conf-dir", HD_DESKTOP_CONFIG_PATH,
                              "user-conf-dir", user_conf_dir,
                              "filename", filename,
                              NULL);

  g_free (user_conf_dir);

  return config_file;
}

/**
 * hd_config_file_load_file:
 * @config_file: a #HDConfigFile.
 * @force_system_config: %TRUE if the user config file should not be loaded
 *
 * Creates a new #GKeyFile and loads from config file. If available and 
 * @force_system_config is %FALSE the user config file is used, else 
 * the system config file is used
 *
 * Returns: a new #GKeyFile. Should be freed with g_key_file_free.
 **/
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
      GError *error = NULL;

      filename = g_build_filename (priv->user_conf_dir, priv->filename, NULL);

      /* Try to read key file */
      if (g_key_file_load_from_file (key_file,
                                     filename,
                                     G_KEY_FILE_NONE,
                                     &error))
        {
          g_free (filename);
          return key_file;
        }
      else if (g_error_matches (error,
                                G_KEY_FILE_ERROR,
                                G_KEY_FILE_ERROR_PARSE))
        {
          g_debug ("User configuration file `%s' is treated as empty. %s",
                   filename,
                   error->message);
          g_error_free (error);
          g_free (filename);
          return key_file;
        }
      else if (g_error_matches (error,
                                G_KEY_FILE_ERROR,
                                G_KEY_FILE_ERROR_NOT_FOUND))
        {
          g_debug ("User configuration file `%s' not found. %s",
                   filename,
                   error->message);
          g_error_free (error);
        }
      else
        {
          g_debug ("Could not read user configuration file `%s'. %s",
                   filename,
                   error->message);
          g_error_free (error);
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

/**
 * hd_config_file_save_file:
 * @config_file: a #HDConfigFile.
 * @key_file: a #GKeyFile which should be stored.
 *
 * Atomically store @key_file to the user config file.
 *
 * Returns: %TRUE if the file could be stored successful, %FALSE otherwise.
 **/
gboolean
hd_config_file_save_file (HDConfigFile *config_file,
                          GKeyFile     *key_file)
{
  HDConfigFilePrivate *priv;
  gchar *tmpl, *tmpl_filename, *real_filename;
  gint fd;
  gchar *data;
  gsize length;
  GError *error = NULL;

  priv = config_file->priv;

  if (!priv->user_conf_dir || !priv->filename)
    {
      g_warning ("Cannot save file: no user conf dir or filename set");
      return FALSE;
    }

  /* Get the data which should be written */
  data = g_key_file_to_data (key_file, &length, &error);
  if (!data)
    {
      g_warning ("Cannot save file: %s", error->message);
      g_error_free (error);
      return FALSE;
    }

  /* Check if user config dir exists or try to create it */
  if (g_mkdir_with_parents (priv->user_conf_dir,
                            S_IRWXU |
                            S_IRGRP | S_IXGRP |
                            S_IROTH | S_IXOTH) == -1)
    {
      g_warning ("Cannot save file: Cannot mkdir \"%s\"", priv->user_conf_dir);
      g_free (data);
      return FALSE;
    }

  /* Create a temporary file */
  tmpl_filename = g_build_filename (priv->user_conf_dir, priv->filename, NULL);
  tmpl = g_strdup_printf ("%sXXXXXX", tmpl_filename);
  g_free (tmpl_filename);
  fd = g_mkstemp (tmpl);
  if (fd == -1)
    {
      g_warning ("Cannot save file: Cannot mkstemp \"%s\"", tmpl);
      g_free (tmpl);
      g_free (data);
      return FALSE;
    }

  /* Write data to temporary file */
  if (write (fd, data, length) == -1)
    {
      g_warning ("Cannot save file: Failed to write to file.");
      g_free (tmpl);
      g_free (data);
      close (fd);
      return FALSE;
    }

  g_free (data);

  /* Sync the file content to disc */
  if (fsync (fd) == -1)
    {
      g_warning ("Cannot save file: Failed to sync file.");
      g_free (tmpl);
      close (fd);
      return FALSE;
    }

  if (close (fd) == -1)
    {
      g_warning ("Cannot save file: Failed to close file.");
      g_free (tmpl);
      return FALSE;
    }

  /* Move the temporary file to the real file and overwrite it */
  real_filename = g_build_filename (priv->user_conf_dir, priv->filename, NULL);
  if (rename (tmpl, real_filename) == -1)
    {
      g_warning ("Cannot save file: Failed to rename file.");
      g_free (tmpl);
      g_free (real_filename);
      return FALSE;
    }

  g_free (tmpl);
  g_free (real_filename);
  return TRUE;
}

