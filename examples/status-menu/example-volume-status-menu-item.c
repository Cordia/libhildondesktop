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

#include "example-volume-status-menu-item.h"

#define EXAMPLE_VOLUME_STATUS_MENU_ITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, EXAMPLE_VOLUME_TYPE_STATUS_MENU_ITEM, ExampleVolumeStatusMenuItemPrivate))

struct _ExampleVolumeStatusMenuItemPrivate
{
  gpointer data;
};

HD_DEFINE_PLUGIN_MODULE (ExampleVolumeStatusMenuItem, example_volume_status_menu_item, HD_TYPE_STATUS_MENU_ITEM);

static void
example_volume_status_menu_item_class_finalize (ExampleVolumeStatusMenuItemClass *klass)
{
}

static void
example_volume_status_menu_item_class_init (ExampleVolumeStatusMenuItemClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExampleVolumeStatusMenuItemPrivate));
}

static void
example_volume_status_menu_item_init (ExampleVolumeStatusMenuItem *menu_item)
{
  GtkWidget *box, *image, *scale;

  box = gtk_hbox_new (FALSE, 3);
  gtk_container_set_border_width (GTK_CONTAINER (box), 11);
  gtk_widget_show (box);

  image = gtk_image_new_from_file (HILDON_DATA_DIR "/example-icon.png");
  gtk_widget_show (image);

  scale = gtk_hscale_new_with_range (0, 100, 5);
  gtk_scale_set_draw_value (GTK_SCALE (scale), FALSE);
  gtk_widget_show (scale);

  /* Pack widgets */
  gtk_box_pack_start (GTK_BOX (box), image, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (box), scale, TRUE, TRUE, 5);
  gtk_container_add (GTK_CONTAINER (menu_item), box);

  /* Permament visible */
  gtk_widget_show (GTK_WIDGET (menu_item));
}
