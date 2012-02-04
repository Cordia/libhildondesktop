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
example_label_home_applet_realize (GtkWidget *widget)
{
  GdkScreen *screen = gtk_widget_get_screen (widget);
  GdkVisual *visual = gdk_screen_get_rgba_visual (screen);

  if (visual == NULL)
    visual = gdk_screen_get_system_visual (screen);

  gtk_widget_set_visual (widget, visual);

  gtk_widget_set_app_paintable (widget, TRUE);

  GTK_WIDGET_CLASS (example_label_home_applet_parent_class)->realize (widget);
}

static gboolean
example_label_home_applet_draw (GtkWidget *widget,
                                cairo_t   *cr)
{
  /* Draw alpha background */
  cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
  cairo_set_source_rgba (cr, 0.0, 0.0, 0.0, 0.0);
  cairo_paint (cr);

  return GTK_WIDGET_CLASS (example_label_home_applet_parent_class)->draw (widget,
                                                                          cr);
}

static void
example_label_home_applet_class_init (ExampleLabelHomeAppletClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->realize = example_label_home_applet_realize;
  widget_class->draw = example_label_home_applet_draw;

  g_type_class_add_private (klass, sizeof (ExampleLabelHomeAppletPrivate));
}

static void
example_label_home_applet_visible_notify (GObject                *object,
                                          GParamSpec             *spec,
                                          ExampleLabelHomeApplet *applet)
{
  gboolean visible;

  g_object_get (object, "is-on-current-desktop", &visible, NULL);

  g_debug ("is-on-current-desktop changed. visible: %u", visible);
}

static void
example_label_home_applet_init (ExampleLabelHomeApplet *applet)
{
  GtkWidget *label;

  applet->priv = EXAMPLE_LABEL_HOME_APPLET_GET_PRIVATE (applet);

  label = gtk_label_new ("An example applet");
  gtk_widget_show (label);

  gtk_container_add (GTK_CONTAINER (applet), label);

  g_signal_connect (applet, "notify::is-on-current-desktop",
                    G_CALLBACK (example_label_home_applet_visible_notify), applet);
}
