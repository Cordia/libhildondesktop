/*
 * This file is part of hildon-home
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

#ifndef __HD_NOTIFICATION_H__
#define __HD_NOTIFICATION_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define HD_TYPE_NOTIFICATION             (hd_notification_get_type ())
#define HD_NOTIFICATION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), HD_TYPE_NOTIFICATION, HDNotification))
#define HD_NOTIFICATION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), HD_TYPE_NOTIFICATION, HDNotificationClass))
#define HD_IS_NOTIFICATION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), HD_TYPE_NOTIFICATION))
#define HD_IS_NOTIFICATION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), HD_TYPE_NOTIFICATION))
#define HD_NOTIFICATION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), HD_TYPE_NOTIFICATION, HDNotificationClass))

typedef struct _HDNotification        HDNotification;
typedef struct _HDNotificationClass   HDNotificationClass;
typedef struct _HDNotificationPrivate HDNotificationPrivate;

/** HDNotification:
 *
 * A notification received by the org.freedesktop.Notifications.Notify.
 *
 **/
struct _HDNotification
{
  GObject parent;

  HDNotificationPrivate *priv;
};

struct _HDNotificationClass
{
  GObjectClass parent;

  void (*updated) (HDNotification *notification);
  void (*closed)  (HDNotification *notification);
};
 
GType           hd_notification_get_type        (void);

HDNotification *hd_notification_new             (guint           id,
                                                 const gchar    *icon,
                                                 const gchar    *summary,
                                                 const gchar    *body,
                                                 gchar         **actions,
                                                 GHashTable     *hints,
                                                 gint            timeout,
                                                 const gchar    *sender);

guint           hd_notification_get_id          (HDNotification *notification);
const gchar    *hd_notification_get_icon        (HDNotification *notification);
const gchar    *hd_notification_get_summary     (HDNotification *notification);
const gchar    *hd_notification_get_body        (HDNotification *notification);
gchar         **hd_notification_get_actions     (HDNotification *notification);
GValue         *hd_notification_get_hint        (HDNotification *notification,
                                                 const gchar    *key);
const gchar    *hd_notification_get_sender      (HDNotification *notification);

const gchar    *hd_notification_get_category    (HDNotification *notification);
const gchar    *hd_notification_get_dbus_cb     (HDNotification *notification,
                                                 const gchar    *action_id);
guint           hd_notification_get_dialog_type (HDNotification *notification);
gboolean        hd_notification_get_persistent  (HDNotification *notification);
time_t          hd_notification_get_time        (HDNotification *notification);

void            hd_notification_closed          (HDNotification *notification);
void            hd_notification_updated         (HDNotification *notification);

G_END_DECLS

#endif

