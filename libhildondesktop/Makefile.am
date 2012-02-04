lib_LTLIBRARIES = libhildondesktop-@API_VERSION_MAJOR@.la

libhildondesktop_@API_VERSION_MAJOR@_la_LDFLAGS = $(LIBHILDONDESKTOP_LT_LDFLAGS)

libhildondesktop_@API_VERSION_MAJOR@_la_CFLAGS = \
	$(HILDON_CFLAGS)							\
	$(GCONF_CFLAGS)							\
	$(DBUS_CFLAGS)								\
	-I$(top_srcdir) 							\
	-DLOCALEDIR=\"$(localedir)\" 						\
	-DDATADIR=\"$(datadir)\"						\
	-DHD_DESKTOP_ENTRY_PATH=\"$(hildondesktopentrydir)\" 			\
	-DHD_DESKTOP_CONFIG_PATH=\"$(hildondesktopconfdir)\" 			\
	-DHD_DESKTOP_MODULE_PATH=\"$(hildondesktoplibdir)\"			\
	-DHD_DESKTOP_MENU_PATH=\"$(hildondesktopmenudir)\"			\
	-DHD_DESKTOP_BACKGROUNDS_PATH=\"$(hildondesktopbackgroundsdir)\"	\
	-DHD_PLUGIN_LOADER_MODULES_PATH=\"$(hildonpluginloaderlibdir)\"		\
	-DHD_UI_POLICY_MODULES_PATH=\"$(hildonuipolicylibdir)\"

libhildondesktop_@API_VERSION_MAJOR@_la_SOURCES = \
	hd-config-file.c							\
	hd-heartbeat.c								\
	hd-home-plugin-item.c							\
	hd-notification.c							\
	hd-notification-plugin.c						\
	hd-plugin-configuration.c						\
	hd-plugin-item.c							\
	hd-plugin-loader-default.c						\
	hd-plugin-loader-factory.c						\
	hd-plugin-loader.c							\
	hd-plugin-manager.c							\
	hd-plugin-module.c							\
	hd-shortcuts.c								\
	hd-stamp-file.c								\
	hd-status-menu-item.c							\
	hd-status-plugin-item.c							\
	hd-pvr-texture.c							\
	pvr-texture.c

libhildondesktop_@API_VERSION_MAJOR@_la_LIBADD = \
	$(HILDON_LIBS)								\
	$(GCONF_LIBS)							\
	$(DBUS_LIBS)								\
	@LIBHILDONDESKTOP_LT_LDFLAGS@

libhildondesktop_@API_VERSION_MAJOR@_includedir = $(includedir)/$(PACKAGE)-$(API_VERSION_MAJOR)/$(PACKAGE)

libhildondesktop_@API_VERSION_MAJOR@_public_headers = \
	hd-config-file.h							\
	hd-heartbeat.h								\
	hd-home-plugin-item.h							\
	hd-notification.h							\
	hd-notification-plugin.h						\
	hd-plugin-configuration.h						\
	hd-plugin-item.h							\
	hd-plugin-loader-default.h						\
	hd-plugin-loader-factory.h						\
	hd-plugin-loader.h							\
	hd-plugin-manager.h							\
	hd-plugin-module.h							\
	hd-shortcuts.h								\
	hd-stamp-file.h								\
	hd-status-menu-item.h							\
	hd-status-plugin-item.h							\
	hd-pvr-texture.h							\
	libhildondesktop.h

libhildondesktop_@API_VERSION_MAJOR@_include_HEADERS = \
	$(libhildondesktop_@API_VERSION_MAJOR@_public_headers)

noinst_HEADERS = hd-config.h

libhildondesktop-@API_VERSION_MAJOR@.pc: libhildondesktop.pc
	cp $< $@

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libhildondesktop-@API_VERSION_MAJOR@.pc

EXTRA_DIST = libhildondesktop.pc.in

CLEANFILES = \
	libhildondesktop-@API_VERSION_MAJOR@.pc
