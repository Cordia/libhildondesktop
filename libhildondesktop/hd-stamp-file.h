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

#ifndef __HD_STAMP_FILE_H__
#define __HD_STAMP_FILE_H__

#include <glib.h>

G_BEGIN_DECLS

void     hd_stamp_file_init          (const gchar *stamp_file);
gboolean hd_stamp_file_get_safe_mode (void);
void     hd_stamp_file_finalize      (const gchar *stamp_file);

G_END_DECLS

#endif
