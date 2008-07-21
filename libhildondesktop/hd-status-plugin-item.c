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

#include "hd-status-plugin-item.h"

/** 
 * SECTION:hd-status-plugin-item
 * @short_description: Base class for plugable status Hildon Desktop items.
 *
 * Base class for all plugable status Hildon Desktop items.
 *
 * 
 **/

G_DEFINE_ABSTRACT_TYPE (HDStatusPluginItem, hd_status_plugin_item, GTK_TYPE_BIN);

static void
hd_status_plugin_item_size_allocate (GtkWidget     *widget,
                              GtkAllocation *allocation)
{
  GtkWidget *child;

  GTK_WIDGET_CLASS (hd_status_plugin_item_parent_class)->size_allocate (widget,
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
hd_status_plugin_item_size_request (GtkWidget      *widget,
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
hd_status_plugin_item_class_init (HDStatusPluginItemClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  widget_class->size_allocate = hd_status_plugin_item_size_allocate;
  widget_class->size_request = hd_status_plugin_item_size_request;
}

static void
hd_status_plugin_item_init (HDStatusPluginItem *menu_item)
{
}

/**
 * hd_status_plugin_item_set_status_area_icon:
 * @item: a #HDStatusPluginItem
 * @icon: a #GdkPixbuf
 *
 * Sets the Status Area icon corresponding to this item to @icon.
 *
 **/
void 
hd_status_plugin_item_set_status_area_icon (HDStatusPluginItem *item,
                                            GdkPixbuf          *icon)
{
  /* FIXME stub */
}
