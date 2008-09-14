/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006, 2007, 2008 Nokia Corporation.
 *
 * Based on main.c from hildon-desktop.
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

#include <glib.h>
#include <glib/gstdio.h>

#include "hd-stamp-file.h"

static gboolean hd_stamp_file_initialized = FALSE;
static gboolean hd_stamp_file_safe_mode = FALSE;

void
hd_stamp_file_init (const gchar *stamp_file)
{
  const gchar *dev_mode = g_getenv ("SBOX_PRELOAD");

  if (hd_stamp_file_initialized)
    {
      g_warning ("Stamp File already initialized");
      return;
    }

  if (!dev_mode && stamp_file)
    {
      /* 
       * Check for safe mode. The stamp file is created here and
       * Removed in main after gtk_main by g_object_unref in a call to finalize
       * function of this gobject in case of clean non-crash exit 
       * Added by Karoliina <karoliina.t.salminen@nokia.com> 31.7.2007 
       */
      if (g_file_test (stamp_file, G_FILE_TEST_EXISTS)) 
        {
          /* Enters safe mode */
          g_warning ("The program did not exit properly on the previous "
                     "session. All plugins will be disabled.");

          hd_stamp_file_safe_mode = TRUE;
        } 
      else 
        {
          /* Hildon Desktop enters normal mode and creates the stamp to track crashes */
          gchar *stamp_dir = g_path_get_dirname (stamp_file);

          g_mkdir_with_parents (stamp_dir, 0755);
          g_free (stamp_dir);

          g_file_set_contents (stamp_file, "1", 1, NULL);

          hd_stamp_file_safe_mode = FALSE;
        }
    }

  hd_stamp_file_initialized = TRUE;
}

gboolean
hd_stamp_file_get_safe_mode (void)
{
  if (!hd_stamp_file_initialized)
    {
      g_warning ("Stamp file support not initialized");
      return FALSE;
    }

  return hd_stamp_file_safe_mode;
}

void
hd_stamp_file_finalize (const gchar *stamp_file)
{
  /* 
   * Clean up stamp file created by hd_stamp_file_init
   * On next startup the stamp file is created again and hildon-desktop remains
   * in normal operation mode without entering into safe mode where some plugins
   * are disabled.
   */
  if (g_file_test (stamp_file, G_FILE_TEST_EXISTS)) 
    {
      g_unlink (stamp_file);
    }
}
