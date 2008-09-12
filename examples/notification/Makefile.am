hildondesktoplib_LTLIBRARIES = \
	example-notification-plugin.la

example_notification_plugin_la_CPPFLAGS = \
	$(HILDON_CFLAGS)		\
	$(DBUS_CFLAGS)

example_notification_plugin_la_LDFLAGS = \
	-module -avoid-version

example_notification_plugin_la_LIBADD = \
	$(HILDON_LIBS)			\
	$(DBUS_LIBS)

example_notification_plugin_la_SOURCES = \
	example-notification-plugin.c \
	example-notification-plugin.h

hildonnotificationdesktopentry_DATA = \
	example-notification.desktop
