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

#ifndef __EXAMPLE_LABEL_HOME_APPLET_H__
#define __EXAMPLE_LABEL_HOME_APPLET_H__

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define EXAMPLE_TYPE_LABEL_HOME_APPLET            (example_label_home_applet_get_type ())
#define EXAMPLE_LABEL_HOME_APPLET(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXAMPLE_TYPE_LABEL_HOME_APPLET, ExampleLabelHomeApplet))
#define EXAMPLE_LABEL_HOME_APPLET_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXAMPLE_TYPE_LABEL_HOME_APPLET, ExampleLabelHomeAppletClass))
#define EXAMPLE_IS_LABEL_HOME_APPLET(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXAMPLE_TYPE_LABEL_HOME_APPLET))
#define EXAMPLE_IS_LABEL_HOME_APPLET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXAMPLE_TYPE_LABEL_HOME_APPLET))
#define EXAMPLE_LABEL_HOME_APPLET_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXAMPLE_TYPE_LABEL_HOME_APPLET, ExampleLabelHomeAppletClass))

typedef struct _ExampleLabelHomeApplet        ExampleLabelHomeApplet;
typedef struct _ExampleLabelHomeAppletClass   ExampleLabelHomeAppletClass;
typedef struct _ExampleLabelHomeAppletPrivate ExampleLabelHomeAppletPrivate;

struct _ExampleLabelHomeApplet
{
  HDHomePluginItem       parent;

  ExampleLabelHomeAppletPrivate       *priv;
};

struct _ExampleLabelHomeAppletClass
{
  HDHomePluginItemClass  parent;
};

GType example_label_home_applet_get_type (void);

G_END_DECLS

#endif
