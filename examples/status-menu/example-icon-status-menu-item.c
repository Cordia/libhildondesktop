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

#include "example-icon-status-menu-item.h"

#define EXAMPLE_ICON_STATUS_MENU_ITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, EXAMPLE_ICON_TYPE_STATUS_MENU_ITEM, ExampleIconStatusMenuItemPrivate))

struct _ExampleIconStatusMenuItemPrivate
{
  gpointer data;
};

HD_DEFINE_PLUGIN_MODULE (ExampleIconStatusMenuItem, example_icon_status_menu_item, HD_TYPE_STATUS_MENU_ITEM);

static void
example_icon_status_menu_item_class_finalize (ExampleIconStatusMenuItemClass *klass)
{
}

static void
example_icon_status_menu_item_class_init (ExampleIconStatusMenuItemClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExampleIconStatusMenuItemPrivate));
}

static void
button_clicked_cb (GtkButton *button, ExampleIconStatusMenuItem *menu_item)
{
  GtkWidget *parent;
  GtkWidget *dialog;

  parent = gtk_widget_get_toplevel (GTK_WIDGET (menu_item));

  dialog = hildon_note_new_confirmation (GTK_WINDOW (parent),
		                         "Select connection");
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

static void
example_icon_status_menu_item_init (ExampleIconStatusMenuItem *menu_item)
{
  GtkWidget *button;
  GtkWidget *hbox, *vbox1, *vbox2;
  GtkWidget *image;
  GtkWidget *label1, *label2;
  PangoAttrList *attr_list;

  /* Button with icon and two rows of text */
  button = gtk_button_new ();
  gtk_container_set_border_width (GTK_CONTAINER (button), 3);
  g_signal_connect_after (G_OBJECT (button), "clicked",
                          G_CALLBACK (button_clicked_cb), menu_item);
  hbox = gtk_hbox_new (FALSE, 8);
  vbox1 = gtk_vbox_new (FALSE, 0);
  vbox2 = gtk_vbox_new (FALSE, 0);
  image = gtk_image_new_from_file (HILDON_DATA_DIR "/example-icon.png");
  label1 = gtk_label_new ("Wireless Connection");
  gtk_misc_set_alignment (GTK_MISC (label1), 0.0, 0.5);
  attr_list = pango_attr_list_new ();
  pango_attr_list_insert (attr_list, pango_attr_size_new_absolute (PANGO_SCALE * 24));
  gtk_label_set_attributes (GTK_LABEL (label1), attr_list);
  pango_attr_list_unref (attr_list);
  label2 = gtk_label_new ("TheWayOut");
  gtk_misc_set_alignment (GTK_MISC (label2), 0.0, 0.5);
  attr_list = pango_attr_list_new ();
  pango_attr_list_insert (attr_list, pango_attr_size_new_absolute (PANGO_SCALE * 18));
  gtk_label_set_attributes (GTK_LABEL (label2), attr_list);
  pango_attr_list_unref (attr_list);

  gtk_container_add (GTK_CONTAINER (button), hbox);
  gtk_box_pack_start (GTK_BOX (hbox), image, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), vbox1, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (vbox1), vbox2, TRUE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), label1, FALSE, FALSE, 0);
  gtk_box_pack_start (GTK_BOX (vbox2), label2, FALSE, FALSE, 0);
  gtk_container_add (GTK_CONTAINER (menu_item), button);

  gtk_widget_show_all (GTK_WIDGET (menu_item));
}
