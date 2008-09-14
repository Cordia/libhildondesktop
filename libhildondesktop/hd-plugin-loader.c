/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006 Nokia Corporation.
 *
 * Author:  Johan Bilien <johan.bilien@nokia.com>
 * Contact: Karoliina Salminen <karoliina.t.salminen@nokia.com>
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

#include "hd-plugin-loader.h"

G_DEFINE_ABSTRACT_TYPE (HDPluginLoader, hd_plugin_loader, G_TYPE_OBJECT)

static void
hd_plugin_loader_class_init (HDPluginLoaderClass *class)
{
}

static void
hd_plugin_loader_init (HDPluginLoader *loader)
{
}

GQuark
hd_plugin_loader_error_quark (void)
{
  return g_quark_from_static_string ("hd-plugin-loader-error-quark");
}

GObject *
hd_plugin_loader_load (HDPluginLoader  *loader,
                       const gchar     *plugin_id,
                       GKeyFile        *keyfile,
                       GError         **error)
{
  return HD_PLUGIN_LOADER_GET_CLASS (loader)->load (loader,
                                                    plugin_id,
                                                    keyfile,
                                                    error);
}

