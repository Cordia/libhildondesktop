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

#include "hd-status-area-item.h"

static void
hd_status_area_item_class_init (gpointer g_iface)
{
  /*  GType iface_type = G_TYPE_FROM_INTERFACE (g_iface); */

  g_object_interface_install_property (g_iface,
                                       g_param_spec_object ("status-area-pixbuf",
                                                            "Status Area Pixbuf",
                                                            "The current Status Area pixbuf",
                                                            GDK_TYPE_PIXBUF,
                                                            G_PARAM_READWRITE));
}

GType
hd_status_area_item_get_type (void)
{
  static GType status_area_item_type = 0;

  if (! status_area_item_type)
    {
      const GTypeInfo status_area_item_info =
        {
          sizeof (HDStatusAreaItemIface), /* class_size */
          NULL,           /* base_init */
          NULL,		/* base_finalize */
          (GClassInitFunc) hd_status_area_item_class_init,
          NULL,		/* class_finalize */
          NULL,		/* class_data */
          0,
          0,              /* n_preallocs */
          NULL
        };

      status_area_item_type = g_type_register_static (G_TYPE_INTERFACE, "HDStatusAreaItem",
                                                      &status_area_item_info, 0);

      g_type_interface_add_prerequisite (status_area_item_type, G_TYPE_OBJECT);
    }

  return status_area_item_type;
}

/**
 *
 * Returns: the current pixbuf. g_object_unref it.
 */
GdkPixbuf *
hd_status_area_item_get_icon (HDStatusAreaItem *item)
{
  GdkPixbuf *pixbuf;

  g_return_val_if_fail (HD_IS_STATUS_AREA_ITEM (item), NULL);

  g_object_get (G_OBJECT (item),
                "status-area-pixbuf", &pixbuf,
                NULL);

  return pixbuf;
}

void
hd_status_area_item_set_icon (HDStatusAreaItem *item,
                              GdkPixbuf        *pixbuf)
{
  g_return_if_fail (HD_IS_STATUS_AREA_ITEM (item));

  g_object_set (G_OBJECT (item),
                "status-area-pixbuf", pixbuf,
                NULL);
}

