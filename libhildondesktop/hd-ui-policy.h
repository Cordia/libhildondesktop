/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2007, 2008 Nokia Corporation.
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


#ifndef __HD_UI_POLICY_H__
#define __HD_UI_POLICY_H__

#include <glib-object.h>
#include <gtk/gtkwidget.h>

G_BEGIN_DECLS

#define HD_TYPE_UI_POLICY            (hd_ui_policy_get_type ())
#define HD_UI_POLICY(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_UI_POLICY, HDUIPolicy))
#define HD_UI_POLICY_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  HD_TYPE_UI_POLICY, HDUIPolicyClass))
#define HD_IS_UI_POLICY(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_UI_POLICY))
#define HD_IS_UI_POLICY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  HD_TYPE_UI_POLICY))
#define HD_UI_POLICY_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  HD_TYPE_UI_POLICY, HDUIPolicyClass))

typedef struct _HDUIPolicy        HDUIPolicy;
typedef struct _HDUIPolicyClass   HDUIPolicyClass;
typedef struct _HDUIPolicyPrivate HDUIPolicyPrivate;

struct _HDUIPolicy 
{
  GObject gobject;

  HDUIPolicyPrivate *priv;
};

struct _HDUIPolicyClass 
{
  GObjectClass parent_class;
};

GType               hd_ui_policy_get_type            (void);

HDUIPolicy         *hd_ui_policy_new                 (const gchar *module);

gchar              *hd_ui_policy_get_filtered_plugin (HDUIPolicy  *policy,
		                                      const gchar *module_id,
                                                      gboolean     safe_mode);

gchar              *hd_ui_policy_get_default_plugin  (HDUIPolicy  *policy,
		 				      const gchar *module_id,
                                                      gboolean     safe_mode);

GObject            *hd_ui_policy_get_failure_plugin  (HDUIPolicy  *policy,
		 				      const gchar *module_id,
                                                      gboolean     safe_mode);

G_END_DECLS

#endif /* __HD_UI_POLICY_H__ */
