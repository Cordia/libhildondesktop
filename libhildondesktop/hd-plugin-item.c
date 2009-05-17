/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2008 Nokia Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
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

#include "hd-plugin-module.h"

#include "hd-plugin-item.h"

/** 
 * SECTION:hd-plugin-item
 * @short_description: Base interface for plugable items.
 * @include: libhildondesktop/libhildondesktop.h
 *
 * #HDPluginItem is a base interface for plugable items. It defines functions properties
 * which are basically only useful for the Hildon desktop itself and not for widget/plugin
 * developers.
 *
 * It defines a hd_plugin_item_get_dl_filename() function which returns the filename of
 * the dynamic library for debugging purposes.
 *
 * It defines the #HDPluginItem::plugin-id property
 * for the unique plugin ID which is used internally by the Hildon Desktop to identify widget/plugins. It is
 * usually not used by widget/plugin developers.
 *
 * And it defines a hd_plugin_item_load_desktop_file() function for internal use by the Hildon desktop. 
 **/

static void
hd_plugin_item_class_init (gpointer g_iface)
{
  g_object_interface_install_property (g_iface,
                                       g_param_spec_string ("plugin-id",
                                                            "Plugin id",
                                                            "The id to identify the plugin item (used " \
                                                            "internally by the Hildon desktop to identify " \
                                                            "widgets/plugins. It is usually not used by " \
                                                            "widget/plugin developers)",
                                                            NULL,
                                                            G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

GType
hd_plugin_item_get_type (void)
{
  static GType plugin_item_type = 0;

  if (!plugin_item_type)
    {
      plugin_item_type = g_type_register_static_simple (G_TYPE_INTERFACE,
                                                        "HDPluginItem",
                                                        sizeof (HDPluginItemIface),
                                                        (GClassInitFunc) hd_plugin_item_class_init,
                                                        0, NULL, 0);

      g_type_interface_add_prerequisite (plugin_item_type, G_TYPE_OBJECT);
    }

  return plugin_item_type;
}

/**
 * hd_plugin_item_get_plugin_id:
 * @item: a #HDPluginItem.
 *
 * Returns the unique plugin ID of @item.
 *
 * It is used internally by the Hildon desktop to identify widgets/plugins. It is usually 
 * not used by widget/plugin developers.
 * 
 * Returns: the plugin ID. The result should be freed if no longer used.
 **/
gchar *
hd_plugin_item_get_plugin_id (HDPluginItem *item)
{
  gchar *plugin_id;

  g_return_val_if_fail (HD_IS_PLUGIN_ITEM (item), NULL);

  g_object_get (G_OBJECT (item),
                "plugin-id", &plugin_id,
                NULL);

  return plugin_id;
}

/**
 * hd_plugin_item_get_dl_filename:
 * @item: a #HDPluginItem.
 *
 * Returns the filename of the dynamic library file from which this item was loaded.
 * Useful for debugging purposes.
 *
 * Returns: filename of the dynamic library file. The result must not be freed. 
 **/
const gchar *
hd_plugin_item_get_dl_filename (HDPluginItem *item)
{
  static GQuark dl_filename_quark = 0;
  GType type;

  g_return_val_if_fail (HD_IS_PLUGIN_ITEM (item), NULL);

  /* Create hd-plugin-module-dl-filename quark */
  if (G_UNLIKELY (!dl_filename_quark))
    dl_filename_quark = g_quark_from_static_string (HD_PLUGIN_MODULE_DL_FILENAME);

  /* The dl filename is stored in the type data */
  type = G_TYPE_FROM_INSTANCE (item);
  return g_type_get_qdata (type, dl_filename_quark);
}

/**
 * hd_plugin_item_load_desktop_file:
 * @item: a #HDPluginItem.
 * @key_file: the plugins .desktop #GKeyFile
 *
 * Used internally by the Hildon desktop to load additional keys from
 * the plugins .desktop file.
 **/
void
hd_plugin_item_load_desktop_file (HDPluginItem *item,
                                  GKeyFile     *key_file)
{
  HDPluginItemIface *iface;

  g_return_if_fail (HD_IS_PLUGIN_ITEM (item));
  
  iface = HD_PLUGIN_ITEM_GET_IFACE (item);

  if (iface->load_desktop_file)
    iface->load_desktop_file (item, key_file);
}

