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

#include "hd-status-menu-item.h"

/** 
 * SECTION:hd-status-menu-item
 * @short_description: Base class for plugable Status Menu items.
 *
 * A base class for plugable Status Menu items.
 *
 * To create a Status Menu plugin derive a class from #HDStatusMenuItem and
 * register it with HD_DEFINE_PLUGIN_MODULE().
 *
 * To show a Status Menu plugin permanently use gtk_widget_show() in the instance 
 * init function. To show (and hide) a plugin temporary or conditional use gtk_widget_show() 
 * and gtk_widget_hide() or #GtkWidget::visible.
 *
 * If a button is added as a direct child of the #HDStatusMenuItem the Status Menu
 * will automatically closed on click. 
 *
 **/

G_DEFINE_ABSTRACT_TYPE (HDStatusMenuItem, hd_status_menu_item, HD_TYPE_STATUS_PLUGIN_ITEM);

static void
hd_status_menu_item_button_clicked_cb (GtkButton *button,
                                       gpointer   data)
{
  GtkWidget *toplevel;

  toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));

  if (toplevel != NULL)
    gtk_widget_hide (toplevel);
}

static void
hd_status_menu_item_add (GtkContainer *container,
                         GtkWidget    *child)
{
  GTK_CONTAINER_CLASS (hd_status_menu_item_parent_class)->add (container, child);

  if (GTK_IS_BUTTON (child))
    g_signal_connect (G_OBJECT (child), "clicked",
                      G_CALLBACK (hd_status_menu_item_button_clicked_cb), NULL);
}

static void
hd_status_menu_item_class_init (HDStatusMenuItemClass *klass)
{
  GtkContainerClass *container_class = GTK_CONTAINER_CLASS (klass);

  container_class->add = hd_status_menu_item_add;
}

static void
hd_status_menu_item_init (HDStatusMenuItem *menu_item)
{
}

/**
 * hd_status_menu_item_get_dbus_connection:
 * @menu_item: A #HDStatusMenuItem.
 * @type: The #DBusBusType %DBUS_BUS_SESSION or %DBUS_BUS_SYSTEM.
 * @error: A #DBusError to return error messages.
 *
 * Creates a new private #DBusConnection to the bus @type.
 *
 * See dbus_bus_get_private.
 *
 * Returns: A new private connection which must be unrefed by the caller.
 **/
DBusConnection *
hd_status_menu_item_get_dbus_connection (HDStatusMenuItem *menu_item,
                                         DBusBusType       type,
                                         DBusError        *error)
{
  DBusConnection *connection;

  /* Create a private connection */
  connection = dbus_bus_get_private (type, error);

  if (!connection)
  return NULL;

  /* Do not exit on disconnect */
  dbus_connection_set_exit_on_disconnect (connection, FALSE);

  /* FIXME: log the connection name for debug purposes */

  return connection;
}
