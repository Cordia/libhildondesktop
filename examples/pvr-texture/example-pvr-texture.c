#include <glib.h>
#include <libhildondesktop/hd-pvr-texture.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  g_type_init ();

  GdkPixbuf *pixbuf;
  gchar *in_file;
  gchar *out_file;
  GError *error = NULL;

  if (argc!=2)
    {
      printf("PVRTC4 Texture Compressor\n");
      printf("USAGE: example-pvr-texture image.png\n");

      return EXIT_FAILURE;
    }

  in_file = argv[1];
  pixbuf = gdk_pixbuf_new_from_file(in_file, 0);
  if (!pixbuf)
    {
      printf("Couldn't load PNG file %s\n", in_file);
      return EXIT_FAILURE;
    }

  out_file = g_malloc(strlen(in_file)+4);
  strcpy(out_file, in_file);
  if (g_str_has_suffix(out_file, ".png"))
    {
      gint l = strlen(out_file);
      out_file[l-2] = 'v';
      out_file[l-1] = 'r';
    }
  else
    strcat(out_file, ".pvr");

  if (!hd_pvr_texture_save(out_file, pixbuf, &error))
    {
      printf("Couldn't compress to PVR file %s. %s\n",
             out_file,
             error->message);
      g_error_free (error);
      g_free(out_file);
      return EXIT_FAILURE;
    }

  g_free(out_file);
  return EXIT_SUCCESS;
}

