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

#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <dbus/dbus.h>

#include "example-label-home-applet.h"

#define EXAMPLE_LABEL_HOME_APPLET_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj,\
							                         EXAMPLE_TYPE_LABEL_HOME_APPLET,\
							                         ExampleLabelHomeAppletPrivate))

struct _ExampleLabelHomeAppletPrivate
{
  gpointer data;
};

HD_DEFINE_PLUGIN_MODULE (ExampleLabelHomeApplet, example_label_home_applet, HD_TYPE_HOME_PLUGIN_ITEM);

static void
example_label_home_applet_class_finalize (ExampleLabelHomeAppletClass *klass)
{
}

static void
example_label_home_applet_class_init (ExampleLabelHomeAppletClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExampleLabelHomeAppletPrivate));
}

static void
example_label_home_applet_init (ExampleLabelHomeApplet *applet)
{
  GtkWidget *label;

  applet->priv = EXAMPLE_LABEL_HOME_APPLET_GET_PRIVATE (applet);

  label = gtk_label_new ("An example applet");
  gtk_widget_show (label);

  gtk_container_add (GTK_CONTAINER (applet), label);
}
