#include "example-plugin.h"

#include <hildon/hildon.h>

#define EXAMPLE_PLUGIN_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE (obj, EXAMPLE_TYPE_PLUGIN, ExamplePluginPrivate))

struct _ExamplePluginPrivate
{
  gpointer data;
};

HD_DEFINE_PLUGIN_MODULE (ExamplePlugin, example_plugin, HD_TYPE_STATUS_MENU_ITEM);

static void
visibility_changed (HDStatusMenuItem *plugin)
{
  gboolean is_visible;

  /* the compositor moves invisible windows off the screen, so we can use that
   * to determine whether the status area is visible */
  g_object_get (G_OBJECT (plugin), "status-area-visible", &is_visible, NULL);

  g_debug ("status area visibility changed; now %s",
           is_visible ? "visible" : "INvisible");
}

static void
example_plugin_class_finalize (ExamplePluginClass *klass)
{
  /* 
   * This is used for class finalization, you usually do not want to
   * add here anything
   */
}

static void
example_plugin_class_init (ExamplePluginClass *klass)
{
  g_type_class_add_private (klass, sizeof (ExamplePluginPrivate));
}

static void
example_plugin_init (ExamplePlugin *menu_item)
{
  GtkWidget *button;
  GdkPixbuf *icon = NULL;

  menu_item->priv = EXAMPLE_PLUGIN_GET_PRIVATE (menu_item);

  /* A HildonButton */
  button = hildon_button_new_with_text (HILDON_SIZE_FINGER_HEIGHT | HILDON_SIZE_AUTO_WIDTH,
		  			HILDON_BUTTON_ARRANGEMENT_VERTICAL,
		  			"Example Title",
					"Value");

  /* Enable if the button should be picker style */
  /* hildon_button_set_style (HILDON_BUTTON (button), HILDON_BUTTON_STYLE_PICKER); */

  gtk_widget_show (button);
  gtk_container_add (GTK_CONTAINER (menu_item), button);

  /* Set up a handler to be called when the status area itself is hidden. This
   * can be used to avoid unnecessary work when a full-screen program hides the
   * status area */
  g_signal_connect_swapped (G_OBJECT (menu_item), "notify::status-area-visible",
                            G_CALLBACK (visibility_changed), menu_item);

  /* To set (or unset) the status area icon */
  hd_status_plugin_item_set_status_area_icon (HD_STATUS_PLUGIN_ITEM (menu_item),
		  		              icon);

  /* Permament visible */
  gtk_widget_show (GTK_WIDGET (menu_item));
}
