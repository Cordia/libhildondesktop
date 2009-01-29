#ifndef __EXAMPLE_PLUGIN_H__
#define __EXAMPLE_PLUGIN_H__

#include <libhildondesktop/libhildondesktop.h>

G_BEGIN_DECLS

#define EXAMPLE_TYPE_PLUGIN            (example_plugin_get_type ())
#define EXAMPLE_PLUGIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), EXAMPLE_TYPE_PLUGIN, ExamplePlugin))
#define EXAMPLE_PLUGIN_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), EXAMPLE_TYPE_PLUGIN, ExamplePluginClass))
#define EXAMPLE_IS_PLUGIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EXAMPLE_TYPE_PLUGIN))
#define EXAMPLE_IS_PLUGIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), EXAMPLE_TYPE_PLUGIN))
#define EXAMPLE_PLUGIN_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), EXAMPLE_TYPE_PLUGIN, ExamplePluginClass))

typedef struct _ExamplePlugin        ExamplePlugin;
typedef struct _ExamplePluginClass   ExamplePluginClass;
typedef struct _ExamplePluginPrivate ExamplePluginPrivate;

struct _ExamplePlugin
{
  HDStatusMenuItem       parent;

  ExamplePluginPrivate  *priv;
};

struct _ExamplePluginClass
{
  HDStatusMenuItemClass  parent;
};

GType example_plugin_get_type (void);

G_END_DECLS

#endif
