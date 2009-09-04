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

#include <glib.h>
#include <gconf/gconf-client.h>
#include <libhildondesktop/libhildondesktop.h>

#include <string.h>

#include "hd-shortcuts.h"

/** 
 * SECTION:hd-shortcuts
 * @short_description: Utils for Home shortcuts.
 *
 * Home shortcuts are a special kind of Home applets #HDShortcuts can be used
 * to create such shortcuts based on a GConf key.
 *
 * hd_shortcuts_add_bookmark_shortcut() can be used to create a bookmark shortcut.
 *
 **/

/* Gconf key for the bookmark shortcuts */
#define BOOKMARK_SHORTCUTS_GCONF_KEY "/apps/osso/hildon-home/bookmark-shortcuts"

/* GConf path for boomarks */
#define BOOKMARKS_GCONF_PATH      "/apps/osso/hildon-home/bookmarks"
#define BOOKMARKS_GCONF_KEY_LABEL BOOKMARKS_GCONF_PATH "/%s/label"
#define BOOKMARKS_GCONF_KEY_URL   BOOKMARKS_GCONF_PATH "/%s/url"
#define BOOKMARKS_GCONF_KEY_ICON  BOOKMARKS_GCONF_PATH "/%s/icon"
#define BOOKMARKS_GCONF_KEY       BOOKMARKS_GCONF_PATH "/%s"

/* Definitions for the ID generation */ 
#define ID_VALID_CHARS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_"
#define ID_SUBSTITUTOR '_'

#define HD_SHORTCUTS_GET_PRIVATE(object) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((object), HD_TYPE_SHORTCUTS, HDShortcutsPrivate))

struct _HDShortcutsPrivate
{
  GHashTable *applets;
  GConfClient *gconf_client;
  gchar *gconf_key;
  GType shortcut_type;

  GSList *current_list;
};

enum
{
  PROP_0,
  PROP_GCONF_KEY,
  PROP_SHORTCUT_TYPE,
};

G_DEFINE_TYPE (HDShortcuts, hd_shortcuts, G_TYPE_OBJECT);

static gboolean
delete_event_cb (GtkWidget   *shortcut,
                 GdkEvent    *event,
                 HDShortcuts *shortcuts)
{
  HDShortcutsPrivate *priv = shortcuts->priv;
  gchar *plugin_id;
  GSList *l;
  GError *error = NULL;

  /* Remove the this shortcut from the list */
  g_object_get (shortcut, "plugin-id", &plugin_id, NULL);

  g_debug ("%s. Plugin %s.", __FUNCTION__, plugin_id);

  l = g_slist_find_custom (priv->current_list, plugin_id, (GCompareFunc) strcmp);
  if (l)
    {
      g_free (l->data);
      priv->current_list = g_slist_delete_link (priv->current_list, l);
    }

  /* Save the new list of strings of task shortcuts */
  gconf_client_set_list (priv->gconf_client,
                         priv->gconf_key,
                         GCONF_VALUE_STRING,
                         priv->current_list,
                         &error);

  /* Check if there was an error */
  if (error)
    {
      g_warning ("Could not store list of shortcuts to GConf: %s", error->message);
      g_clear_error (&error);
    }

  gconf_client_suggest_sync (priv->gconf_client,
                             &error);
  if (error)
    {
      g_warning ("%s. Could not suggest sync to GConf: %s.",
                 __FUNCTION__,
                 error->message);
      g_clear_error (&error);
    }

  g_free (plugin_id);

  /* Do not destroy the widget here, it will be destroyed after syncing the lists */
  gtk_widget_hide (shortcut);

  return TRUE;
}

/* Compare lists new and old and move elements unique in old
 * to to_remove and elements unique in new to to_add, elements
 * common in new and old are removed.
 *
 * old is destroyed by this function
 * new is destroyed by this function
 */
static void
create_sync_lists (GSList         *old,
                   GSList         *new,
                   GSList        **to_add,
                   GSList        **to_remove,
                   GCompareFunc    cmp_func,
                   GDestroyNotify  destroy_func)
{
  GSList *add = NULL;
  GSList *remove = NULL;

  g_return_if_fail (to_add != NULL);
  g_return_if_fail (to_remove != NULL);

  /* sort lists */
  old = g_slist_sort (old, cmp_func);
  new = g_slist_sort (new, cmp_func);

  while (old && new)
    {
      gint c = cmp_func (old->data, new->data);

      /* there is an element only in new 
       * move it to list to_add */
      if (c > 0)
        {
          GSList *n = new;
          new = g_slist_remove_link (new, new);
          add = g_slist_concat (n, add);
        }
      /* there is an element only in old 
       * move it to list to_remove */
      else if (c < 0)
        {
          GSList *o = old;
          old = g_slist_remove_link (old, old);
          remove = g_slist_concat (o, remove);
        }
      /* the next element is in old and new
       * remove it */
      else
        {
          destroy_func (old->data);
          destroy_func (new->data);

          old = g_slist_delete_link (old, old);
          new = g_slist_delete_link (new, new);
        }
    }

  /* add remaining elements to the approbiate lists */
  *to_add = g_slist_concat (new, add);
  *to_remove = g_slist_concat (old, remove);
}

static void
shortcuts_sync (HDShortcuts *shortcuts)
{
  HDShortcutsPrivate *priv = shortcuts->priv;
  GHashTableIter iter;
  gpointer key;
  GSList *old = NULL, *new = NULL, *i;
  GSList *to_add, *to_remove;
  GSList *s;

  g_hash_table_iter_init (&iter, priv->applets);
  while (g_hash_table_iter_next (&iter, &key, NULL)) 
    {
      old = g_slist_append (old, g_strdup (key));
    }

  for (i = priv->current_list; i; i = i->next)
    {
      gchar *str = i->data;

      new = g_slist_append (new, g_strdup (str));
    }

  create_sync_lists (old, new,
                     &to_add, &to_remove,
                     (GCompareFunc) strcmp,
                     (GDestroyNotify) g_free);

  for (s = to_remove; s; s = s->next)
    {
      g_hash_table_remove (priv->applets, s->data);
      g_free (s->data);
    }

  for (s = to_add; s; s = s->next)
    {
      GtkWidget *shortcut;

      shortcut = g_object_new (priv->shortcut_type, "plugin-id", s->data, NULL);
      g_signal_connect (shortcut, "delete-event",
                        G_CALLBACK (delete_event_cb), shortcuts);

      g_hash_table_insert (priv->applets, s->data, shortcut);

      gtk_widget_show (shortcut);
    }

  g_slist_free (to_remove);
  g_slist_free (to_add);
}

static gboolean
is_value_a_string_list (GConfValue *value)
{
  return value->type == GCONF_VALUE_LIST &&
         gconf_value_get_list_type (value) == GCONF_VALUE_STRING;
}

static GSList *
copy_and_convert_string_list (GSList *list)
{
  GSList *i;
  GSList *result = NULL;

  for (i = list; i; i = i->next)
    {
      GConfValue *v = i->data;

      result = g_slist_append (result, g_strdup (gconf_value_get_string (v)));
    }

  return result;
}

static GSList *
get_shortcuts_list_from_value (GConfValue *value)
{
  if (is_value_a_string_list (value))
    {
      return copy_and_convert_string_list (gconf_value_get_list (value));
    }

  return NULL;
}

static GSList *
get_shortcuts_list_from_entry (GConfEntry *entry)
{
  GConfValue *value;

  value = gconf_entry_get_value (entry);

  return get_shortcuts_list_from_value (value);
}

static void
shortcuts_notify (GConfClient *client,
                  guint        cnxn_id,
                  GConfEntry  *entry,
                  HDShortcuts *shortcuts)
{
  HDShortcutsPrivate *priv = shortcuts->priv;

  g_slist_foreach (priv->current_list, (GFunc) g_free, NULL);
  g_slist_free (priv->current_list);

  priv->current_list = get_shortcuts_list_from_entry (entry);

  shortcuts_sync (shortcuts);
}

static void
hd_shortcuts_get_property (GObject      *object,
                           guint         prop_id,
                           GValue       *value,
                           GParamSpec   *pspec)
{
  HDShortcutsPrivate *priv = HD_SHORTCUTS (object)->priv;

  switch (prop_id) {
    case PROP_GCONF_KEY:
      g_value_set_string (value, priv->gconf_key);
      break;

    case PROP_SHORTCUT_TYPE:
      g_value_set_gtype (value, priv->shortcut_type);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hd_shortcuts_set_property (GObject      *object,
                           guint         prop_id,
                           const GValue *value,
                           GParamSpec   *pspec)
{
  HDShortcutsPrivate *priv = HD_SHORTCUTS (object)->priv;

  switch (prop_id) {
    case PROP_GCONF_KEY:
      priv->gconf_key = g_value_dup_string (value);
      break;

    case PROP_SHORTCUT_TYPE:
      priv->shortcut_type = g_value_get_gtype (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
  }
}

static void
hd_shortcuts_constructed (GObject *object)
{
  HDShortcuts *shortcuts = HD_SHORTCUTS (object);
  HDShortcutsPrivate *priv = shortcuts->priv;

  /* Add notification of shortcuts key */
  gconf_client_notify_add (priv->gconf_client,
                           priv->gconf_key,
                           (GConfClientNotifyFunc) shortcuts_notify,
                           shortcuts,
                           NULL, NULL);

  /* Get the list of strings shortcuts */
  gconf_client_notify (priv->gconf_client,
                       priv->gconf_key);
}

static void
hd_shortcuts_finalize (GObject *object)
{
  HDShortcutsPrivate *priv = HD_SHORTCUTS (object)->priv;

  g_slist_foreach (priv->current_list, (GFunc) g_free, NULL);
  g_slist_free (priv->current_list);

  g_hash_table_destroy (priv->applets);
  g_object_unref (priv->gconf_client);
  g_free (priv->gconf_key);

  G_OBJECT_CLASS (hd_shortcuts_parent_class)->finalize (object);
}

static void
hd_shortcuts_class_init (HDShortcutsClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  object_class->finalize = hd_shortcuts_finalize;
  object_class->constructed = hd_shortcuts_constructed;
  object_class->get_property = hd_shortcuts_get_property;
  object_class->set_property = hd_shortcuts_set_property;

  g_object_class_install_property (object_class, PROP_GCONF_KEY,
                                   g_param_spec_string ("gconf-key",
                                                        "GConf key",
                                                        "The GConf key containing the list of shortcuts",
                                                        NULL,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                                                        G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                                                        G_PARAM_STATIC_BLURB));

  g_object_class_install_property (object_class, PROP_SHORTCUT_TYPE,
                                   g_param_spec_gtype ("shortcut-type",
                                                       "The shortcut GType",
                                                       "The GType of shortcut widgets",
                                                       HD_TYPE_HOME_PLUGIN_ITEM,
                                                       G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY |
                                                       G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |
                                                       G_PARAM_STATIC_BLURB));

  g_type_class_add_private (klass, sizeof (HDShortcutsPrivate));
}

static void
hd_shortcuts_init (HDShortcuts *shortcuts)
{
  shortcuts->priv = HD_SHORTCUTS_GET_PRIVATE (shortcuts);
  shortcuts->priv->gconf_client = gconf_client_get_default ();
  shortcuts->priv->applets = g_hash_table_new_full (g_str_hash,
                                                    g_str_equal,
                                                    (GDestroyNotify) g_free,
                                                    (GDestroyNotify) gtk_widget_destroy);
}

/**
 * hd_shortcuts_new:
 * @gconf_key: the GConf key where the shortcuts are stored
 * @shortcut_type: the #GType of the shortcut instances
 *
 * Creates a #HDShortcuts instance which handles the creation of instances
 * of a subclass @shortcut_type of #HDHomePluginItem based on a GConf key @gconf_key.
 *
 * Returns: a new #HDShortcuts instance.
 *
 **/
HDShortcuts *
hd_shortcuts_new (const gchar *gconf_key, GType shortcut_type)
{
  g_return_val_if_fail (gconf_key != NULL, NULL);
  g_return_val_if_fail (g_type_is_a (shortcut_type, HD_TYPE_HOME_PLUGIN_ITEM), NULL);

  return g_object_new (HD_TYPE_SHORTCUTS,
                       "gconf-key", gconf_key,
                       "shortcut-type", shortcut_type,
                       NULL);
}

/**
 * hd_shortcuts_add_bookmark_shortcut:
 * @url: the URL of the bookmark
 * @label: the title of the bookmark
 * @icon: the optional icon of the bookmark
 *
 * Creates a new bookmark shortcut with @url, @label and
 * optional @icon.
 *
 * @icon should be the path to a 160x96 sized image file in
 * ~/.bookmarks/shortcut-thumbnails.
 **/
void
hd_shortcuts_add_bookmark_shortcut (const gchar *url,
                                    const gchar *label,
                                    const gchar *icon)
{
  GConfClient *client;
  gchar *canon_url, *id = NULL;
  guint count = 0;
  gchar *key;
  GSList *list;
  GError *error = NULL;

  g_return_if_fail (url != NULL);
  g_return_if_fail (label != NULL);

  client = gconf_client_get_default ();

  /* Get the current list of bookmark shortcuts from GConf */
  list = gconf_client_get_list (client,
                                BOOKMARK_SHORTCUTS_GCONF_KEY,
                                GCONF_VALUE_STRING,
                                &error);

  if (error)
    {
      g_debug ("Could not get string list from GConf (%s): %s.",
               BOOKMARK_SHORTCUTS_GCONF_KEY,
               error->message);
      g_error_free (error);
      error = NULL;
    }

  /* Create an unique id for the bookmark */
  canon_url = g_strdup (url);
  g_strcanon (canon_url, ID_VALID_CHARS, ID_SUBSTITUTOR);
  do
    {
      g_free (id);
      id = g_strdup_printf ("%s-%u", canon_url, count++);
    }
  while (g_slist_find_custom (list, id, (GCompareFunc) strcmp));

  /* Store the bookmark itself into GConf */
  key = g_strdup_printf (BOOKMARKS_GCONF_KEY_LABEL, id);
  gconf_client_set_string (client,
                           key,
                           label,
                           &error);
  if (error)
    {
      g_warning ("Could not store label for bookmark %s into GConf: %s.",
                 id,
                 error->message);
      g_error_free (error);
      error = NULL;
    }
  g_free (key);

  /* Store icon if available */
  if (icon)
    {
      key = g_strdup_printf (BOOKMARKS_GCONF_KEY_ICON, id);
      gconf_client_set_string (client,
                               key,
                               icon,
                               &error);
      if (error)
        {
          g_warning ("Could not store icon for bookmark %s into GConf: %s.",
                     id,
                     error->message);
          g_error_free (error);
          error = NULL;
        }
      g_free (key);
    }

  key = g_strdup_printf (BOOKMARKS_GCONF_KEY_URL, id);
  gconf_client_set_string (client,
                           key,
                           url,
                           &error);
  if (error)
    {
      g_warning ("Could not store URL for bookmark %s into GConf: %s.",
                 id,
                 error->message);
      g_error_free (error);
      error = NULL;
    }
  g_free (key);

  /* Append the new bookmark to bookmark shortcut list */
  list = g_slist_append (list, id);

  /* Store the new list in GConf */
  gconf_client_set_list (client,
                         BOOKMARK_SHORTCUTS_GCONF_KEY,
                         GCONF_VALUE_STRING,
                         list,
                         &error);
  if (error)
    {
      g_warning ("Could not write string list to GConf (%s): %s.",
                 BOOKMARK_SHORTCUTS_GCONF_KEY,
                 error->message);
      g_error_free (error);
      error = NULL;
    }

  g_free (canon_url);

  g_slist_foreach (list, (GFunc) g_free, NULL);
  g_slist_free (list);

  g_object_unref (client);
}

