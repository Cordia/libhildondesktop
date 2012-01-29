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

#ifndef __EXAMPLE_SIGNAL_STATUS_AREA_ITEM_H__
#define __EXAMPLE_SIGNAL_STATUS_AREA_ITEM_H__

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define EXAMPLE_TYPE_SIGNAL_STATUS_AREA_ITEM            (example_signal_status_area_item_get_type ())
#define EXAMPLE_SIGNAL_STATUS_AREA_ITEM(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXAMPLE_TYPE_SIGNAL_STATUS_AREA_ITEM, ExampleSignalStatusAreaItem))
#define EXAMPLE_SIGNAL_STATUS_AREA_ITEM_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXAMPLE_TYPE_SIGNAL_STATUS_AREA_ITEM, ExampleSignalStatusAreaItemClass))
#define EXAMPLE_IS_SIGNAL_STATUS_AREA_ITEM(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXAMPLE_TYPE_SIGNAL_STATUS_AREA_ITEM))
#define EXAMPLE_IS_SIGNAL_STATUS_AREA_ITEM_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXAMPLE_TYPE_SIGNAL_STATUS_AREA_ITEM))
#define EXAMPLE_SIGNAL_STATUS_AREA_ITEM_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXAMPLE_TYPE_SIGNAL_STATUS_AREA_ITEM, ExampleSignalStatusAreaItemClass))

typedef struct _ExampleSignalStatusAreaItem        ExampleSignalStatusAreaItem;
typedef struct _ExampleSignalStatusAreaItemClass   ExampleSignalStatusAreaItemClass;
typedef struct _ExampleSignalStatusAreaItemPrivate ExampleSignalStatusAreaItemPrivate;

struct _ExampleSignalStatusAreaItem
{
  HDStatusMenuItem          parent;

  ExampleSignalStatusAreaItemPrivate *priv;
};

struct _ExampleSignalStatusAreaItemClass
{
  HDStatusMenuItemClass     parent;
};

GType example_signal_status_area_item_get_type (void);

G_END_DECLS

#endif
