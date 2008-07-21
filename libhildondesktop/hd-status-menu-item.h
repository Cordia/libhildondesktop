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

#ifndef __HD_STATUS_MENU_ITEM_H__
#define __HD_STATUS_MENU_ITEM_H__

#include <glib-object.h>
#include <libhildondesktop/hd-status-plugin-item.h>

G_BEGIN_DECLS

#define HD_TYPE_STATUS_MENU_ITEM            (hd_status_menu_item_get_type ())
#define HD_STATUS_MENU_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_STATUS_MENU_ITEM, HDStatusMenuItem))
#define HD_STATUS_MENU_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), HD_TYPE_STATUS_MENU_ITEM, HDStatusMenuItemClass))
#define HD_IS_STATUS_MENU_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_STATUS_MENU_ITEM))
#define HD_IS_STATUS_MENU_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), HD_TYPE_STATUS_MENU_ITEM))
#define HD_STATUS_MENU_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), HD_TYPE_STATUS_MENU_ITEM, HDStatusMenuItemClass))

/** HDStatusMenuItem:
 *
 * An item in the Hildon Status Menu.
 */
typedef struct _HDStatusMenuItem        HDStatusMenuItem;
typedef struct _HDStatusMenuItemClass   HDStatusMenuItemClass;

struct _HDStatusMenuItem
{
  HDStatusPluginItem parent;
};

struct _HDStatusMenuItemClass
{
  HDStatusPluginItemClass parent;
};

GType hd_status_menu_item_get_type (void);

G_END_DECLS

#endif
