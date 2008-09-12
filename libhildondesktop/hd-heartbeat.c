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

#include <errno.h>
#include <string.h>
#include <time.h>
#include <iphbd/libiphb.h>

#include "hd-heartbeat.h"

typedef struct _HeartbeatSource HeartbeatSource;

struct _HeartbeatSource
{
  GSource source;
  iphb_t iphb;
  GPollFD poll;
  guint mintime;
  guint maxtime;
};

static gboolean 
heartbeat_prepare (GSource *source,
                   gint    *timeout)
{
  *timeout = -1;

  return FALSE;
}

static gboolean 
heartbeat_check (GSource *source)
{
  HeartbeatSource *heartbeat_source = (HeartbeatSource *)source;

  return heartbeat_source->poll.revents != 0;
}

static gboolean
heartbeat_dispatch (GSource    *source, 
                    GSourceFunc callback,
                    gpointer    user_data)
{
  HeartbeatSource *heartbeat_source = (HeartbeatSource *)source;

  if (!callback)
    {
      g_warning ("Idle source dispatched without callback\n"
                 "You must call g_source_set_callback().");
      return FALSE;
    }

  if (callback (user_data))
    {
      g_source_remove_poll (source, &heartbeat_source->poll);

      iphb_wait (heartbeat_source->iphb, heartbeat_source->mintime, heartbeat_source->maxtime, 0);

      heartbeat_source->poll.fd = iphb_get_fd (heartbeat_source->iphb);
      heartbeat_source->poll.events = G_IO_IN;
      heartbeat_source->poll.revents = 0;

      g_source_add_poll (source, &heartbeat_source->poll);

      return TRUE;
    }
  else
    {
      return FALSE;

    }
}

static void
heartbeat_finalize (GSource *source)
{
  HeartbeatSource *heartbeat_source = (HeartbeatSource *)source;

  heartbeat_source->iphb = iphb_close (heartbeat_source->iphb);
}


GSourceFuncs heartbeat_funcs =
{
  heartbeat_prepare,
  heartbeat_check,
  heartbeat_dispatch,
  heartbeat_finalize
};

GSource *
hd_heartbeat_source_new (guint mintime,
                         guint maxtime)
{
  iphb_t iphb;
  GSource *source;
  HeartbeatSource *heartbeat_source;
  int heartbeat_interval;

  iphb = iphb_open (&heartbeat_interval);
  if (!iphb)
    {
      g_debug ("ERROR, iphb_open() failed %s\n", strerror (errno));
      return NULL;
    }

  source = g_source_new (&heartbeat_funcs, sizeof (HeartbeatSource));
  heartbeat_source = (HeartbeatSource *)source;

  heartbeat_source->iphb = iphb;
  heartbeat_source->mintime = mintime;
  heartbeat_source->maxtime = maxtime;

  iphb_wait (heartbeat_source->iphb, mintime, maxtime, 0);

  heartbeat_source->poll.fd = iphb_get_fd (heartbeat_source->iphb);
  heartbeat_source->poll.events = G_IO_IN;

  g_source_add_poll (source, &heartbeat_source->poll);

  return source;
}

guint
hd_heartbeat_add (guint       mintime,
                  guint       maxtime,
                  GSourceFunc function,
                  gpointer    data)
{
  return hd_heartbeat_add_full (G_PRIORITY_DEFAULT,
                                mintime, maxtime,
                                function,
                                data, NULL);
}

guint
hd_heartbeat_add_full (gint           priority,
                       guint          mintime,
                       guint          maxtime,
                       GSourceFunc    function,
                       gpointer       data,
                       GDestroyNotify notify)
{
  GSource *source;
  guint id;

  /* Try to create heartbeat source */
  source = hd_heartbeat_source_new (mintime, maxtime);

  /* If heartbeat source could not be opened use timeout source */
  if (!source)
    source = g_timeout_source_new_seconds (maxtime);

  /* Set priority */
  g_source_set_priority (source, priority);

  /* Set the callback */
  g_source_set_callback (source, function, data, notify);

  /* Attach the source */
  id = g_source_attach (source, NULL);

  g_source_unref (source);

  return id;
}

