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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "hd-notification.h"

/** 
 * SECTION:hd-notification
 * @short_description: A notification received by org.freedesktop.Notifications.Notify.
 *
 * A notification received by org.freedesktop.Notifications.Notify.
 **/

#define HD_NOTIFICATION_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_NOTIFICATION, HDNotificationPrivate))

enum
{
  PROP_0,
  PROP_ID,
  PROP_ICON,
  PROP_SUMMARY,
  PROP_BODY,
  PROP_ACTIONS,
  PROP_HINTS,
  PROP_TIMEOUT,
  PROP_SENDER
};

enum {
    UPDATED,
    CLOSED,
    N_SIGNALS
};

static guint signals[N_SIGNALS];

struct _HDNotificationPrivate
{
  guint id;

  gchar *icon;
  gchar *summary;
  gchar *body;

  gchar **actions;
  GHashTable *hints;

  gint timeout;

  gchar *sender;

  gboolean closed : 1;
};

G_DEFINE_TYPE (HDNotification, hd_notification, G_TYPE_OBJECT);

static void
hd_notification_dispose (GObject *object)
{
  /*   HDNotificationPrivate *priv = HD_NOTIFICATION (object)->priv; */

  G_OBJECT_CLASS (hd_notification_parent_class)->dispose (object);
}

static void
hd_notification_finalize (GObject *object)
{
  HDNotificationPrivate *priv = HD_NOTIFICATION (object)->priv;

  g_free (priv->icon);
  priv->icon = NULL;

  g_free (priv->summary);
  priv->summary = NULL;

  g_free (priv->body);
  priv->body = NULL;

  g_strfreev (priv->actions);
  priv->actions = NULL;

  if (priv->hints != NULL)
    {
      g_hash_table_destroy (priv->hints);
      priv->hints = NULL;
    }

  g_free (priv->sender);
  priv->sender = NULL;

  G_OBJECT_CLASS (hd_notification_parent_class)->finalize (object);
}

static void
hd_notification_get_property (GObject      *object,
                              guint         prop_id,
                              GValue       *value,
                              GParamSpec   *pspec)
{
  HDNotificationPrivate *priv = HD_NOTIFICATION (object)->priv;

  switch (prop_id)
    {
    case PROP_ID:
      g_value_set_uint (value, priv->id);
      break;

    case PROP_ICON:
      g_value_set_string (value, priv->icon);
      break;

    case PROP_SUMMARY:
      g_value_set_string (value, priv->summary);
      break;

    case PROP_BODY:
      g_value_set_string (value, priv->body);
      break;

    case PROP_ACTIONS:
      g_value_set_boxed (value, priv->actions);
      break;

    case PROP_HINTS:
      g_value_set_pointer (value, priv->hints);
      break;

    case PROP_TIMEOUT:
      g_value_set_int (value, priv->timeout);
      break;

    case PROP_SENDER:
      g_value_set_string (value, priv->sender);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_notification_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  HDNotificationPrivate *priv = HD_NOTIFICATION (object)->priv;

  switch (prop_id)
    {
    case PROP_ID:
      priv->id = g_value_get_uint (value);
      break;

    case PROP_ICON:
      g_free (priv->icon);
      priv->icon = g_value_dup_string (value);
      break;

    case PROP_SUMMARY:
      g_free (priv->summary);
      priv->summary = g_value_dup_string (value);
      break;

    case PROP_BODY:
      g_free (priv->body);
      priv->body = g_value_dup_string (value);
      break;

    case PROP_ACTIONS:
      g_strfreev (priv->actions);
      priv->actions = g_value_dup_boxed (value);
      break;

    case PROP_HINTS:
      priv->hints = g_value_get_pointer (value);
      break;

    case PROP_TIMEOUT:
      priv->timeout = g_value_get_int (value);
      break;

    case PROP_SENDER:
      g_free (priv->sender);
      priv->sender = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
hd_notification_class_init (HDNotificationClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->dispose = hd_notification_dispose;
  object_class->finalize = hd_notification_finalize;
  object_class->get_property = hd_notification_get_property;
  object_class->set_property = hd_notification_set_property;

  signals[UPDATED] = g_signal_new ("updated",
                                   G_OBJECT_CLASS_TYPE (klass),
                                   G_SIGNAL_RUN_LAST,
                                   G_STRUCT_OFFSET (HDNotificationClass, updated),
                                   NULL, NULL,
                                   g_cclosure_marshal_VOID__VOID,
                                   G_TYPE_NONE, 0);
  signals[CLOSED] = g_signal_new ("closed",
                                  G_OBJECT_CLASS_TYPE (klass),
                                  G_SIGNAL_RUN_LAST,
                                  G_STRUCT_OFFSET (HDNotificationClass, closed),
                                  NULL, NULL,
                                  g_cclosure_marshal_VOID__VOID,
                                  G_TYPE_NONE, 0);

  g_object_class_install_property (object_class,
                                   PROP_ID,
                                   g_param_spec_uint ("id",
                                                      "ID",
                                                      "The ID of the notification",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
  g_object_class_install_property (object_class,
                                   PROP_ICON,
                                   g_param_spec_string ("icon",
                                                        "Icon",
                                                        "The icon-name of the notification",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_SUMMARY,
                                   g_param_spec_string ("summary",
                                                        "Summary",
                                                        "Summary text of the notification",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_BODY,
                                   g_param_spec_string ("body",
                                                        "Body",
                                                        "Body text of the notification",
                                                        NULL,
                                                        G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_ACTIONS,
                                   g_param_spec_boxed ("actions",
                                                       "Actions",
                                                       "Array of actions of the notification",
                                                       G_TYPE_STRV,
                                                       G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_HINTS,
                                   g_param_spec_pointer ("hints",
                                                         "Hints",
                                                         "Table of hints of the notification",
                                                         G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_TIMEOUT,
                                   g_param_spec_int ("timeout",
                                                     "Timeout",
                                                     "Timeout FIXME",
                                                     G_MININT,
                                                     G_MAXINT,
                                                     0,
                                                     G_PARAM_READWRITE));
  g_object_class_install_property (object_class,
                                   PROP_SENDER,
                                   g_param_spec_string ("sender",
                                                        "Sender",
                                                        "The sender of the notification",
                                                        NULL,
                                                        G_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (HDNotificationPrivate));
}

static void
hd_notification_init (HDNotification *notification)
{
  HDNotificationPrivate *priv = HD_NOTIFICATION_GET_PRIVATE (notification);

  notification->priv = priv;
}

/**
 * hd_notification_new:
 * @id: unique ID of the notification
 * @icon: icon name of the notification
 * @summary: summary of the notification
 * @body: body of the notification
 * @actions: an array of action definitions
 * @hints: a table of string->GValue mappings
 * @timeout: a timeout for the notification
 * @sender: the creator of the notification
 *
 * Create a new #HDNotification.
 *
 * Returns: a new #HDNotification.
 **/
HDNotification *
hd_notification_new (guint         id,
                     const gchar  *icon,
                     const gchar  *summary,
                     const gchar  *body,
                     gchar       **actions,
                     GHashTable   *hints,
                     gint          timeout,
                     const gchar  *sender)
{
  return g_object_new (HD_TYPE_NOTIFICATION,
                       "id", id,
                       "icon", icon,
                       "summary", summary,
                       "body", body,
                       "actions", actions,
                       "hints", hints,
                       "timeout", timeout,
                       "sender", sender,
                       NULL);
}

/**
 * hd_notification_get_id:
 * @notification: a #HDNotification.
 *
 * Returns the ID of %notification.
 *
 * Returns: the ID.
 **/
guint
hd_notification_get_id (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), 0);

  return priv->id;
}

/**
 * hd_notification_get_icon:
 * @notification: a #HDNotification
 *
 * Returns the icon name of %notification
 *
 * Returns: the icon name. Must not be freed.
 **/
const gchar *
hd_notification_get_icon (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), NULL);

  return priv->icon;
}

/**
 * hd_notification_get_summary:
 * @notification: a #HDNotification
 *
 * Returns the summary of %notification
 *
 * Returns: the summary. Must not be freed.
 **/
const gchar *
hd_notification_get_summary (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), NULL);

  return priv->summary;
}

/**
 * hd_notification_get_body:
 * @notification: a #HDNotification
 *
 * Returns the body of %notification
 *
 * Returns: the body. Must not be freed.
 **/
const gchar *
hd_notification_get_body (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), NULL);

  return priv->body;
}

/**
 * hd_notification_get_actions:
 * @notification: a #HDNotification
 *
 * Returns an array of actions of %notification
 *
 * Returns: an array of actions. Must not be freed.
 **/
gchar **
hd_notification_get_actions (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), NULL);

  return priv->actions;
}

/**
 * hd_notification_get_hint:
 * @notification: a #HDNotification
 * @key: the key of a hint
 *
 * Returns a value of hint %key of %notification
 *
 * Returns: a #GValue. Must not be freed.
 **/
GValue *
hd_notification_get_hint (HDNotification *notification,
                          const gchar    *key)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), NULL);

  if (priv->hints != NULL)
    return g_hash_table_lookup (priv->hints, key);

  return NULL;
}

/**
 * hd_notification_get_hints:
 * @notification: a #HDNotification
 *
 * Returns all hints of %notification
 *
 * Returns: a #GHashTable owned by the notification.
 **/
GHashTable *
hd_notification_get_hints (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), NULL);

  return priv->hints; 
}

/**
 * hd_notification_get_sender:
 * @notification: a #HDNotification
 *
 * Returns the D-Bus sender of %notification
 *
 * Returns: the name of the sender. Must not be freed.
 **/
const gchar *
hd_notification_get_sender (HDNotification *notification)
{
  HDNotificationPrivate *priv = notification->priv;

  return priv->sender;
}

/**
 * hd_notification_get_category:
 * @notification: a #HDNotification
 *
 * Returns the catgeory hint of %notification
 *
 * Returns: the category hint. Must not be freed.
 **/
const gchar *
hd_notification_get_category (HDNotification *notification)
{
  GValue *hint = hd_notification_get_hint (notification,
                                           "category");

  return hint ? g_value_get_string (hint) : NULL;
}

/**
 * hd_notification_get_dialog_type:
 * @notification: a #HDNotification
 *
 * Returns the dialog-type hint of %notification
 *
 * Returns: the dialog-type hint.
 **/
guint
hd_notification_get_dialog_type (HDNotification *notification)
{
  GValue *hint = hd_notification_get_hint (notification,
                                           "dialog-type");

  return hint ? g_value_get_uint (hint) : 0;
}

/**
 * hd_notification_get_persistent:
 * @notification: a #HDNotification
 *
 * Returns the persistent hint of %notification
 *
 * Returns: the persistent hint.
 **/
gboolean
hd_notification_get_persistent (HDNotification *notification)
{
  GValue *hint = hd_notification_get_hint (notification,
                                           "persistent");

  return hint ? g_value_get_uchar (hint) : FALSE;
}

/**
 * hd_notification_get_time:
 * @notification: a #HDNotification
 *
 * Returns the time hint of %notification
 *
 * Returns: the time hint.
 **/
time_t
hd_notification_get_time (HDNotification *notification)
{
  GValue *hint = hd_notification_get_hint (notification,
                                           "time");

  if (G_VALUE_HOLDS_INT64 (hint))
    return (time_t) g_value_get_int64 (hint);
  else if (G_VALUE_HOLDS_LONG (hint))
    return (time_t) g_value_get_long (hint);
  else if (G_VALUE_HOLDS_INT (hint))
    return (time_t) g_value_get_int (hint);
  else
    return (time_t) -1;
}

/**
 * hd_notification_get_dbus_cb:
 * @notification: a #HDNotification
 * @action_id: the id of an action
 *
 * Returns the dbus-callback hint for %action_id of %notification
 *
 * Returns: the dbus-callback hint for %action_id. Must not be freed.
 **/
const gchar *
hd_notification_get_dbus_cb (HDNotification *notification,
                             const gchar    *action_id)
{
  gchar *key;
  GValue *hint;

  key = g_strconcat ("dbus-callback-", action_id, NULL);

  hint = hd_notification_get_hint (notification, key);

  g_free (key);

  return hint ? g_value_get_string (hint) : NULL;
}

/**
 * hd_notification_closed:
 * @notification: a #HDNotification
 *
 * Emits the #HDNotification::closed signal on %notification.
 **/
void
hd_notification_closed (HDNotification *notification)
{
  HDNotificationPrivate *priv;

  g_return_if_fail (HD_IS_NOTIFICATION (notification));

  priv = notification->priv;

  g_signal_emit (notification, signals[CLOSED], 0);

  priv->closed = TRUE;
}

/**
 * hd_notification_is_closed:
 * @notification: a #HDNotification
 *
 * Returns: %TRUE when the notification is closed.
 **/
gboolean
hd_notification_is_closed (HDNotification *notification)
{
  HDNotificationPrivate *priv;

  g_return_val_if_fail (HD_IS_NOTIFICATION (notification), FALSE);

  priv = notification->priv;

  return priv->closed;
}
/**
 * hd_notification_updated:
 * @notification: a #HDNotification
 *
 * Emits the #HDNotification::updated signal on %notification.
 **/
void
hd_notification_updated (HDNotification *notification)
{
  g_signal_emit (notification, signals[UPDATED], 0);
}
