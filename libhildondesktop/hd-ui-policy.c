/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006, 2008 Nokia Corporation.
 *
 * Author: Lucas Rocha <lucas.rocha@nokia.com>
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

#include "hd-ui-policy.h"

#define HD_UI_POLICY_GET_PRIVATE(object) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_UI_POLICY, HDUIPolicyPrivate))

G_DEFINE_TYPE (HDUIPolicy, hd_ui_policy, G_TYPE_OBJECT)

enum 
{
  PROP_0,
  PROP_MODULE_PATH
};

struct _HDUIPolicyPrivate 
{
  GModule            *library;
  gchar              *path;
  
  gchar              *(*get_filtered_plugin)  (const gchar *module_id,
                                               gboolean     safe_mode);
 
  gchar              *(*get_default_plugin)   (const gchar *module_id,
                                               gboolean     safe_mode);

  GObject            *(*get_failure_plugin)   (const gchar *module_id,
                                               gboolean     safe_mode);
};

static void
hd_ui_policy_finalize (GObject *object)
{
  HDUIPolicyPrivate *priv;
  
  priv = HD_UI_POLICY_GET_PRIVATE (object);

  if (priv->path)
  {
    g_free (priv->path);
    priv->path = NULL;
  }

  if (priv->library)
  {
    g_module_close (priv->library);
    priv->library = NULL;
  }

  priv->get_filtered_plugin = NULL;
  priv->get_default_plugin = NULL;
  priv->get_failure_plugin = NULL;
  
  G_OBJECT_CLASS (hd_ui_policy_parent_class)->finalize (object);
}

static void
hd_ui_policy_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  HDUIPolicyPrivate *priv; 

  priv = HD_UI_POLICY_GET_PRIVATE (object);
  
  switch (prop_id)
  {
    case PROP_MODULE_PATH:
      if (priv->path != NULL)
        g_free (priv->path);

      priv->path = g_strdup (g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hd_ui_policy_get_property (GObject      *object,
                           guint         prop_id,
                           GValue       *value,
                           GParamSpec   *pspec)
{
  HDUIPolicyPrivate *priv; 

  priv = HD_UI_POLICY_GET_PRIVATE (object);
  
  switch (prop_id)
  {
    case PROP_MODULE_PATH:
        g_value_set_string (value, priv->path);
        break;

    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static GObject *
hd_ui_policy_constructor (GType                   gtype,
                          guint                   n_params,
                          GObjectConstructParam  *params)
{
  HDUIPolicy *policy;
  HDUIPolicyPrivate *priv;
  GObject *object;

  object = G_OBJECT_CLASS (hd_ui_policy_parent_class)->constructor (gtype,
                                                                    n_params,
                                                                    params);

  policy = HD_UI_POLICY (object);
  priv = policy->priv; 

  priv->library = 
    g_module_open (priv->path, G_MODULE_BIND_MASK);

  if (!priv->library) 
  {
    g_warning (g_module_error ());

    return object;
  }

  if (!g_module_symbol (priv->library,
			"hd_ui_policy_module_get_filtered_plugin",
			(void *) &priv->get_filtered_plugin) ||
      !g_module_symbol (priv->library,
	      		"hd_ui_policy_module_get_default_plugin",
			(void *) &priv->get_default_plugin)   || 
      !g_module_symbol (priv->library,
	      		"hd_ui_policy_module_get_failure_plugin",
			(void *) &priv->get_failure_plugin)) 
  {
    g_warning (g_module_error ());

    g_module_close (priv->library);

    policy->priv->library = NULL; 
    policy->priv->get_filtered_plugin = NULL; 
    policy->priv->get_default_plugin = NULL;
    policy->priv->get_failure_plugin = NULL;
  }

  return object;
}
  
static void
hd_ui_policy_class_init (HDUIPolicyClass *class)
{
  GObjectClass *object_class;
  GParamSpec   *pspec;

  object_class = G_OBJECT_CLASS (class);

  object_class->constructor = hd_ui_policy_constructor;
  object_class->finalize = hd_ui_policy_finalize;
  object_class->set_property = hd_ui_policy_set_property;
  object_class->get_property = hd_ui_policy_get_property;

  pspec = g_param_spec_string ("module-path",
                               "module-path",
                               "Module Path",
			       NULL,
                                G_PARAM_READWRITE | G_PARAM_CONSTRUCT);

  g_object_class_install_property (object_class,
                                   PROP_MODULE_PATH,
                                   pspec);

  g_type_class_add_private (object_class, sizeof (HDUIPolicyPrivate));
}

static void
hd_ui_policy_init (HDUIPolicy *policy)
{
  policy->priv = HD_UI_POLICY_GET_PRIVATE (policy);

  policy->priv->library = NULL; 
  policy->priv->path = NULL; 
  policy->priv->get_filtered_plugin = NULL; 
  policy->priv->get_default_plugin = NULL;
  policy->priv->get_failure_plugin = NULL;
}

HDUIPolicy *
hd_ui_policy_new (const gchar *path)
{
  GObject *policy;

  policy = g_object_new (HD_TYPE_UI_POLICY, 
		         "module-path", path,
			 NULL);

  return HD_UI_POLICY (policy);
}

gchar *
hd_ui_policy_get_filtered_plugin (HDUIPolicy  *policy,
                                  const gchar *module_id,
                                  gboolean     safe_mode)
{
  g_return_val_if_fail (HD_IS_UI_POLICY (policy), NULL);
	
  if (policy->priv->get_filtered_plugin != NULL) 
  {
    return policy->priv->get_filtered_plugin (module_id,
                                              safe_mode);
  }

  return NULL;
}

gchar *
hd_ui_policy_get_default_plugin (HDUIPolicy  *policy,
                                 const gchar *module_id,
                                 gboolean     safe_mode)
{
  g_return_val_if_fail (HD_IS_UI_POLICY (policy), NULL);

  if (policy->priv->get_default_plugin != NULL) 
  {
    return policy->priv->get_default_plugin (module_id,
                                             safe_mode);
  }

  return NULL;
}

GObject *
hd_ui_policy_get_failure_plugin (HDUIPolicy  *policy,
                                 const gchar *module_id,
                                 gboolean     safe_mode)
{
  g_return_val_if_fail (HD_IS_UI_POLICY (policy), NULL);

  if (policy->priv->get_failure_plugin != NULL) 
  {
    return policy->priv->get_failure_plugin (module_id,
                                             safe_mode);
  }

  return NULL;
}
