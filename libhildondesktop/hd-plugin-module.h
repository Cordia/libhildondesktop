/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006, 2008 Nokia Corporation.
 *
 * Author:  Moises Martinez <moises.martinez@nokia.com>
 * Contact: Karoliina Salminen <karoliina.t.salminen@nokia.com>
 *
 * Based on libhildondesktop.h from hildon-desktop
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

#ifndef __HD_PLUGIN_MODULE_H__
#define __HD_PLUGIN_MODULE_H__

#include <glib-object.h>

#include <libhildondesktop/hd-plugin-item.h>

G_BEGIN_DECLS

#define HD_TYPE_PLUGIN_MODULE         (hd_plugin_module_get_type ())
#define HD_PLUGIN_MODULE(o)   	      (G_TYPE_CHECK_INSTANCE_CAST ((o), HD_TYPE_PLUGIN_MODULE, HDPluginModule))
#define HD_PLUGIN_MODULE_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), HD_TYPE_PLUGIN_MODULE, HDPluginModuleClass))
#define HD_IS_PLUGIN_MODULE(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), HD_TYPE_PLUGIN_MODULE))
#define HD_IS_PLUGIN_MODULE_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), HD_TYPE_PLUGIN_MODULE))
#define HD_PLUGIN_MODULE_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), HD_TYPE_PLUGIN_MODULE, HDPluginModuleClass))

#define HD_PLUGIN_MODULE_DL_FILENAME "hd-plugin-module-dl-filename"

typedef struct _HDPluginModule        HDPluginModule;
typedef struct _HDPluginModuleClass   HDPluginModuleClass;
typedef struct _HDPluginModulePrivate HDPluginModulePrivate;

struct _HDPluginModule
{
  GTypeModule parent;

  HDPluginModulePrivate  *priv;
};

struct _HDPluginModuleClass
{
  GTypeModuleClass parent_class;
};

GType           hd_plugin_module_get_type   (void);

HDPluginModule *hd_plugin_module_new        (const gchar    *path);

GObject        *hd_plugin_module_new_object (HDPluginModule *module,
                                             const gchar    *plugin_id);

void            hd_plugin_module_add_type   (HDPluginModule *module,
                                             GType           type);

#define HD_PLUGIN_MODULE_SYMBOLS(t_n)					\
G_MODULE_EXPORT void hd_plugin_module_load (HDPluginModule *plugin);	\
void hd_plugin_module_load (HDPluginModule *plugin)			\
{									\
  t_n##_register_type (G_TYPE_MODULE (plugin));				\
  hd_plugin_module_add_type (plugin, t_n##_get_type ());		\
}									\
G_MODULE_EXPORT void hd_plugin_module_unload (HDPluginModule *plugin); 	\
void hd_plugin_module_unload (HDPluginModule *plugin)			\
{									\
  (void) plugin;							\
}

#define HD_PLUGIN_MODULE_SYMBOLS_CODE(t_n, CODE_LOAD, CODE_UNLOAD)	\
G_MODULE_EXPORT void hd_plugin_module_load (HDPluginModule *plugin); 	\
void hd_plugin_module_load (HDPluginModule *plugin)		 	\
{									\
  t_n##_register_type (G_TYPE_MODULE (plugin));				\
  hd_plugin_module_add_type (plugin, t_n##_get_type ());		\
  { CODE_LOAD }								\
}									\
G_MODULE_EXPORT void hd_plugin_module_unload (HDPluginModule *plugin); 	\
void hd_plugin_module_unload (HDPluginModule *plugin)			\
{									\
  { CODE_UNLOAD }							\
}

/**
 * SECTION:hd-plugin-module
 * @short_description: Support for the definition of Hildon Desktop plugins.
 *
 * There are some macros to define Hildon Desktop plugins:
 *
 * HD_DEFINE_PLUGIN_MODULE_EXTENDED()
 *
 */

/**
 * HD_DEFINE_PLUGIN_MODULE_EXTENDED:
 * @TN: The name of the object type, in Camel case. (ex: ObjectType)
 * @t_n: The name of the object type, in lowercase, with words separated by '_'.  (ex: object_type)
 * @T_P: The GType of the parent (ex: #STATUSBAR_TYPE_ITEM)
 * @CODE: Custom code that gets inserted in the *_register_type() function 
 * @CODE_LOAD: code executed when the plugin is loaded.
 * @CODE_UNLOAD: code executed when the plugin is unloaded.
 *
 * Register an object supplied by a plugin in Hildon Desktop.
 */
#define HD_DEFINE_PLUGIN_MODULE_EXTENDED(TN, t_n, T_P, CODE, CODE_LOAD, CODE_UNLOAD)   	\
G_DEFINE_DYNAMIC_TYPE_EXTENDED (TN, t_n, T_P, 0, CODE)                             	\
HD_PLUGIN_MODULE_SYMBOLS_CODE (t_n, CODE_LOAD, CODE_UNLOAD)

/**
 * HD_DEFINE_PLUGIN_MODULE:
 * @TN: The name of the object type, in Camel case. (ex: ObjectType)
 * @t_n: The name of the object type, in lowercase, with words separated by '_'.  (ex: object_type)
 * @T_P: The GType of the parent (ex: #STATUSBAR_TYPE_ITEM)
 *
 * Register an object supplied by a plugin in Hildon Desktop.
 * FIXME: link to an example.
 */
#define HD_DEFINE_PLUGIN_MODULE(TN, t_n, T_P)			\
HD_DEFINE_PLUGIN_MODULE_EXTENDED (TN, t_n, T_P, {}, {}, {})

#define HD_DYNAMIC_IMPLEMENT_INTERFACE(TYPE_IFACE, iface_init)  \
{                                                               \
  const GInterfaceInfo g_implement_interface_info =             \
    {                                                           \
      (GInterfaceInitFunc) iface_init, NULL, NULL               \
    };                                                          \
  g_type_module_add_interface (type_module, g_define_type_id,   \
                               TYPE_IFACE,                      \
                               &g_implement_interface_info);    \
}

G_END_DECLS

#endif /*__HD_PLUGIN_MODULE_H__*/
