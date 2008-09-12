hildondesktoplib_LTLIBRARIES = \
	example-label-home-applet.la

example_label_home_applet_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)

example_label_home_applet_la_LDFLAGS = \
	-module -avoid-version

example_label_home_applet_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_label_home_applet_la_SOURCES = \
	example-label-home-applet.c \
	example-label-home-applet.h

hildonhomedesktopentry_DATA = \
	example-label-applet.desktop
