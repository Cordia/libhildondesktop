/*
 * This file is part of libhildondesktop
 *
 * Copyright (C) 2006, 2007, 2008 Nokia Corporation.
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
#include <string.h>

#include "hd-pvr-texture.h"
#include "pvr-texture.h"

/* Save the given pixbuf as a PVRTC4 texture. PVRTC4 textures must be 2^n
 * in width and height, so any texture not of these dimensions will be
 * padded with black (zero alpha if alpha is used).
 */
gboolean hd_pvr_texture_save         (const gchar *file, GdkPixbuf *pixbuf)
{
  guint width, height, bpp;
  guint compress_width, compress_height;
  guchar *compressed = 0;
  const guchar *pixels = 0;
  const guchar *uncompressed = 0;
  guchar *allocated = 0;
  guint compressed_size = 0;

  if (!file || !pixbuf)
    return FALSE;

  width           = gdk_pixbuf_get_width (pixbuf);
  height          = gdk_pixbuf_get_height (pixbuf);
  bpp             = gdk_pixbuf_get_bits_per_sample (pixbuf) *
                    gdk_pixbuf_get_n_channels (pixbuf);
  pixels          = gdk_pixbuf_get_pixels (pixbuf);
  uncompressed    = pixels;

  /* GDK usually only returns 8 bit pixels. this is all we want to deal with */
  if (bpp != 32 && bpp != 24)
    return FALSE;

  /* work out what size width + height we need */
  compress_width = 4;
  compress_height = 4;
  while (compress_width <= width)
    compress_width *= 2;
  while (compress_height <= height)
    compress_height *= 2;

  /* If we had to change something, we need to allocate another memory
   * area and copy the data across */
  if (bpp == 24 || compress_width!=width || compress_height!=height)
    {
      gint x, y;
      guchar *uc;
      const guchar *p;
      uncompressed = uc = allocated = g_malloc(4*compress_width*compress_height);
      p = pixels;
      if (bpp == 24)
        {
          /* for 24 bit we have to loop over everything as we need to pad
           * it all out */
          for (y=0;y<height;y++)
            {
              for (x=0;x<width;x++)
                {
                  uc[0] = p[0];
                  uc[1] = p[1];
                  uc[2] = p[2];
                  uc[3] = 255;
                  uc += 4;
                  p += 3;
                }
              for (x=0;x<compress_width-width;x++)
                {
                  uc[0] = p[0];
                  uc[1] = p[1];
                  uc[2] = p[2];
                  uc[3] = 255;
                  uc += 4;
                }
            }
          for (x=0;x<(compress_height-height)*compress_width;x++)
            {
              uc[0] = p[0];
              uc[1] = p[1];
              uc[2] = p[2];
              uc[3] = 255;
              uc += 4;
            }
        }
      else
        {
          /* for 32 bit we just do memcpy's and memsets */
          for (y=0;y<height;y++)
            {
              memcpy(uc, p, width*4);
              uc += width*4;
              p += width*4;
              memset(uc, '\0', (compress_width-width)*4);
              uc += (compress_width-width)*4;
            }
          memset(uc, '\0', (compress_height-height)*compress_width*4);
          uc += (compress_height-height)*compress_width*4;
        }
    }

  /* now, compress the data */
  compressed = pvr_texture_compress_pvrtc4(
                        uncompressed, compress_width, compress_height,
                        &compressed_size);

  /* free data if we created it above */
  if (allocated)
    g_free(allocated);

  if (!compressed)
    return FALSE;

  /* and finally write it out to a file! */
  if (!pvr_texture_save_pvrtc4( file, compressed, compressed_size,
                                compress_width, compress_height))
    {
      g_free (compressed);
      return FALSE;
    }

  g_free (compressed);
  return TRUE;
}
