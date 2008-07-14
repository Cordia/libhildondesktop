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

#include "example-button-status-menu-item.h"

#define EXAMPLE_BUTTON_STATUS_MENU_ITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, EXAMPLE_BUTTON_TYPE_STATUS_MENU_ITEM, ExampleButtonStatusMenuItemPrivate))

struct _ExampleButtonStatusMenuItemPrivate
{
  guint timeout_id;
};

HD_DEFINE_PLUGIN_MODULE (ExampleButtonStatusMenuItem, example_button_status_menu_item, HD_TYPE_STATUS_MENU_ITEM);

static void
example_button_status_menu_item_class_finalize (ExampleButtonStatusMenuItemClass *klass)
{
}

static void
example_button_status_menu_item_dispose (GObject *object)
{
  ExampleButtonStatusMenuItem *menu_item = EXAMPLE_BUTTON_STATUS_MENU_ITEM (object);
  
  if (menu_item->priv->timeout_id != 0)
  {
    g_source_remove (menu_item->priv->timeout_id);
    menu_item->priv->timeout_id = 0;
  }

  G_OBJECT_CLASS (example_button_status_menu_item_parent_class)->dispose (object);
}

static gboolean
example_button_status_menu_item_timeout_cb (ExampleButtonStatusMenuItem *menu_item)
{
  gboolean visible;

  /* switch visibility */
  g_object_get (G_OBJECT (menu_item),
                "visible", &visible,
                NULL);
  g_object_set (G_OBJECT (menu_item),
                "visible", !visible,
                NULL);

  return TRUE;
}

static void
example_button_status_menu_item_status_menu_map (HDStatusMenuItem *menu_item)
{
  if (EXAMPLE_BUTTON_STATUS_MENU_ITEM (menu_item)->priv->timeout_id == 0)
    {
      g_warning ("Example button: timeout added");
      EXAMPLE_BUTTON_STATUS_MENU_ITEM (menu_item)->priv->timeout_id = 
        gdk_threads_add_timeout (10000, (GSourceFunc) example_button_status_menu_item_timeout_cb, menu_item);
    }
}

static void
example_button_status_menu_item_status_menu_unmap (HDStatusMenuItem *menu_item)
{
  if (EXAMPLE_BUTTON_STATUS_MENU_ITEM (menu_item)->priv->timeout_id != 0)
  {
    g_warning ("Example button: timeout removed");
    g_source_remove (EXAMPLE_BUTTON_STATUS_MENU_ITEM (menu_item)->priv->timeout_id);
    EXAMPLE_BUTTON_STATUS_MENU_ITEM (menu_item)->priv->timeout_id = 0;
  }
}

static void
example_button_status_menu_item_class_init (ExampleButtonStatusMenuItemClass *klass)
{
  HDStatusMenuItemClass *status_menu_item_class = HD_STATUS_MENU_ITEM_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  status_menu_item_class->status_menu_map = example_button_status_menu_item_status_menu_map;
  status_menu_item_class->status_menu_unmap = example_button_status_menu_item_status_menu_unmap;

  object_class->dispose = example_button_status_menu_item_dispose;

  g_type_class_add_private (klass, sizeof (ExampleButtonStatusMenuItemPrivate));
}

static void
example_button_status_menu_item_init (ExampleButtonStatusMenuItem *menu_item)
{
  GtkWidget *button;

  menu_item->priv = EXAMPLE_BUTTON_STATUS_MENU_ITEM_GET_PRIVATE (menu_item);

  button = hildon_button_new (HILDON_BUTTON_WITH_VERTICAL_VALUE | 
		              HILDON_BUTTON_AUTO_WIDTH |
			      HILDON_BUTTON_FINGER_HEIGHT,
                              "Some Title", "Some Value");
  gtk_container_set_border_width (GTK_CONTAINER (button), 3);
  gtk_widget_show (button);

  /* Pack widgets */
  gtk_container_add (GTK_CONTAINER (menu_item), button);

  /* This is a temporary visible menu item so 
   * gtk_widget_show isn't called. */
}
