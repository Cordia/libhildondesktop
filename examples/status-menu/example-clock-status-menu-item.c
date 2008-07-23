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

#include "example-clock-status-menu-item.h"

#define EXAMPLE_CLOCK_STATUS_MENU_ITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, EXAMPLE_CLOCK_TYPE_STATUS_MENU_ITEM, ExampleClockStatusMenuItemPrivate))

struct _ExampleClockStatusMenuItemPrivate
{
  GtkWidget    *label;
  guint         timeout_id;
};

HD_DEFINE_PLUGIN_MODULE (ExampleClockStatusMenuItem, example_clock_status_menu_item, HD_TYPE_STATUS_MENU_ITEM);

static void
example_clock_status_menu_item_class_finalize (ExampleClockStatusMenuItemClass *klass)
{
}

static void
example_clock_status_menu_item_dispose (GObject *object)
{
  ExampleClockStatusMenuItem *menu_item = EXAMPLE_CLOCK_STATUS_MENU_ITEM (object);
  
  if (menu_item->priv->timeout_id != 0)
  {
    g_source_remove (menu_item->priv->timeout_id);
    menu_item->priv->timeout_id = 0;
  }

  G_OBJECT_CLASS (example_clock_status_menu_item_parent_class)->dispose (object);
}

static void
example_clock_status_menu_item_class_init (ExampleClockStatusMenuItemClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = example_clock_status_menu_item_dispose;

  g_type_class_add_private (klass, sizeof (ExampleClockStatusMenuItemPrivate));
}

static gboolean
example_clock_status_menu_item_timeout_cb (ExampleClockStatusMenuItem *menu_item)
{
  char time_str[200];
  time_t t;
  struct tm *tmp;

  t = time (NULL);
  tmp = localtime (&t);

  strftime (time_str, sizeof (time_str), "%d.%m.%Y %H:%M:%S", tmp);

  gtk_button_set_label (GTK_BUTTON (menu_item->priv->label), time_str);

  return TRUE;
}

static void
example_clock_status_menu_item_init (ExampleClockStatusMenuItem *menu_item)
{
  menu_item->priv = EXAMPLE_CLOCK_STATUS_MENU_ITEM_GET_PRIVATE (menu_item);

  menu_item->priv->label = gtk_button_new_with_label ("...");
  gtk_container_set_border_width (GTK_CONTAINER (menu_item->priv->label), 3);
  gtk_widget_show (menu_item->priv->label);

  gtk_container_add (GTK_CONTAINER (menu_item), menu_item->priv->label);

  /* Add timeout */
  example_clock_status_menu_item_timeout_cb (EXAMPLE_CLOCK_STATUS_MENU_ITEM (menu_item));
  menu_item->priv->timeout_id = gdk_threads_add_timeout (1000, (GSourceFunc) example_clock_status_menu_item_timeout_cb, menu_item);

  /* permanent visible */
  gtk_widget_show (GTK_WIDGET (menu_item));
}
