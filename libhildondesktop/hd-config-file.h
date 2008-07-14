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

#ifndef __HD_CONFIG_FILE_H__
#define __HD_CONFIG_FILE_H__

#include <glib-object.h>
#include <glib.h>

G_BEGIN_DECLS

#define HD_TYPE_CONFIG_FILE            (hd_config_file_get_type ())
#define HD_CONFIG_FILE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_CONFIG_FILE, HDConfigFile))
#define HD_CONFIG_FILE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  HD_TYPE_CONFIG_FILE, HDConfigFileClass))
#define HD_IS_CONFIG_FILE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_CONFIG_FILE))
#define HD_IS_CONFIG_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  HD_TYPE_CONFIG_FILE))
#define HD_CONFIG_FILE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  HD_TYPE_CONFIG_FILE, HDConfigFileClass))

typedef struct _HDConfigFile        HDConfigFile;
typedef struct _HDConfigFileClass   HDConfigFileClass;
typedef struct _HDConfigFilePrivate HDConfigFilePrivate;

struct _HDConfigFile 
{
  GObject gobject;

  HDConfigFilePrivate *priv;
};

struct _HDConfigFileClass 
{
  GObjectClass parent_class;

  void (* changed) (HDConfigFile *config_file);
};

GType         hd_config_file_get_type  (void);

HDConfigFile *hd_config_file_new       (const gchar  *system_conf_dir,
                                        const gchar  *user_conf_dir,
                                        const gchar  *filename);

GKeyFile     *hd_config_file_load_file (HDConfigFile *config_file,
                                        gboolean      force_system_config);

G_END_DECLS

#endif /* __HD_CONFIG_FILE_H__ */
