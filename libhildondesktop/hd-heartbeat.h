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

#ifndef __HD_HEARTBEAT_H__
#define __HD_HEARTBEAT_H__

#include <glib.h>

G_BEGIN_DECLS

GSource *hd_heartbeat_source_new (guint          mintime,
                                  guint          maxtime);

guint    hd_heartbeat_add        (guint          mintime,
                                  guint          maxtime,
                                  GSourceFunc    function,
                                  gpointer       data);

guint    hd_heartbeat_add_full   (gint           priority,
                                  guint          mintime,
                                  guint          maxtime,
                                  GSourceFunc    function,
                                  gpointer       data,
                                  GDestroyNotify notify);

G_END_DECLS

#endif
