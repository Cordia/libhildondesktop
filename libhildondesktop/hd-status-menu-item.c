/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2008 Nokia Corporation.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hd-status-menu-item.h"

/** 
 * SECTION:hd-status-menu-item
 * @short_description: A widget to be used as base class for Status Menu items
 * @include libhildondesktop/libhildondesktop.h
 *
 * A widget used as base class for Status Menu items.
 *
 * To create a Status Menu plugin derive a class from #HDStatusMenuItem and
 * register it with HD_REGISTER_PLUGIN_MODULE().
 *
 * To get informed about visibility changes of the Status Menu connect to the
 * #HDStatusMenuItem::status-menu-map and #HDStatusMenuItem::status-menu-unmap
 * signals.
 *
 * Use gtk_widget_show() and gtk_widget_hide() or #GtkWidget::visible to permanently
 * or temporarly show or hide the item.
 *
 **/

#define HD_STATUS_MENU_ITEM_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, HD_TYPE_STATUS_MENU_ITEM, HDStatusMenuItemPrivate))

struct _HDStatusMenuItemPrivate
{
  gpointer data;
};

enum
{
  PROP_0,
};

enum
{
  STATUS_MENU_MAP,
  STATUS_MENU_UNMAP,
  NUM_OF_SIGNALS
};

static guint signal_ids[NUM_OF_SIGNALS];

G_DEFINE_TYPE (HDStatusMenuItem, hd_status_menu_item, GTK_TYPE_BIN);

static void
hd_status_menu_item_size_allocate (GtkWidget     *widget,
                                   GtkAllocation *allocation)
{
  GtkWidget *child;

  GTK_WIDGET_CLASS (hd_status_menu_item_parent_class)->size_allocate (widget,
                                                                      allocation);

  child = GTK_BIN (widget)->child;

  if (child)
    {
      GtkAllocation child_allocation;
      guint border_width;

      border_width = gtk_container_get_border_width (GTK_CONTAINER (widget));

      child_allocation.x = allocation->x + border_width;
      child_allocation.y = allocation->y + border_width;
      child_allocation.width = allocation->width - 2 * border_width;
      child_allocation.height = allocation->height - 2 * border_width;

      gtk_widget_size_allocate (child, &child_allocation);
    }
}

static void
hd_status_menu_item_size_request (GtkWidget      *widget,
                                  GtkRequisition *requisition)
{
  GtkWidget *child;
  GtkRequisition child_requisition = {0, 0};

  child = GTK_BIN (widget)->child;

  if (child)
    gtk_widget_size_request (child, &child_requisition);

  requisition->width = child_requisition.width;
  requisition->height = child_requisition.height;
}

static void
hd_status_menu_item_get_property (GObject      *object,
                                  guint         prop_id,
                                  GValue       *value,
                                  GParamSpec   *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_status_menu_item_set_property (GObject      *object,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  switch (prop_id)
    {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_status_menu_item_button_clicked_cb (GtkButton *button,
                                       gpointer   data)
{
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));

  if (toplevel != NULL)
    gtk_widget_hide (toplevel);
}

static void
hd_status_menu_item_add (GtkContainer *container,
                         GtkWidget    *child)
{
  GTK_CONTAINER_CLASS (hd_status_menu_item_parent_class)->add (container, child);

  if (GTK_IS_BUTTON (child))
    g_signal_connect (G_OBJECT (child), "clicked",
                      G_CALLBACK (hd_status_menu_item_button_clicked_cb), NULL);
}

static void
hd_status_menu_item_class_init (HDStatusMenuItemClass *klass)
{
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  container_class->add = hd_status_menu_item_add;

  widget_class->size_allocate = hd_status_menu_item_size_allocate;
  widget_class->size_request = hd_status_menu_item_size_request;

  object_class->get_property = hd_status_menu_item_get_property;
  object_class->set_property = hd_status_menu_item_set_property;

  /**
   * HDStatusMenuItem::status-menu-map:
   * @menu_item: a #HDStatusMenuItem.
   *
   * Emitted when the Status Menu is shown. It is also emitted when the
   * menu item is added to an already shown Status Menu.
   *
   **/
  signal_ids[STATUS_MENU_MAP] = g_signal_new ("status-menu-map",
                                              HD_TYPE_STATUS_MENU_ITEM,
                                              G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                              G_STRUCT_OFFSET (HDStatusMenuItemClass, status_menu_map),
                                              NULL, NULL,
                                              g_cclosure_marshal_VOID__VOID,
                                              G_TYPE_NONE,
                                              0);
  /**
   * HDStatusMenuItem::status-menu-unmap:
   * @menu_item: a #HDStatusMenuItem.
   *
   * Emitted when the Status Menu is hidden.
   *
   **/
  signal_ids[STATUS_MENU_UNMAP] = g_signal_new ("status-menu-unmap",
                                                HD_TYPE_STATUS_MENU_ITEM,
                                                G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
                                                G_STRUCT_OFFSET (HDStatusMenuItemClass, status_menu_unmap),
                                                NULL, NULL,
                                                g_cclosure_marshal_VOID__VOID,
                                                G_TYPE_NONE,
                                                0);

  g_type_class_add_private (klass, sizeof (HDStatusMenuItemPrivate));
}

static void
hd_status_menu_item_init (HDStatusMenuItem *menu_item)
{
  menu_item->priv = HD_STATUS_MENU_ITEM_GET_PRIVATE (menu_item);
}
