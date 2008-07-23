hildondatadir = $(datadir)/hildon-desktop
hildondata_DATA = example-icon.png

hildondesktoplib_LTLIBRARIES = \
	example-clock-status-menu-item.la	\
	example-icon-status-menu-item.la	\
	example-button-status-menu-item.la	\
	example-volume-status-menu-item.la

example_clock_status_menu_item_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)

example_clock_status_menu_item_la_LDFLAGS = \
	-module -avoid-version

example_clock_status_menu_item_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_clock_status_menu_item_la_SOURCES = \
	example-clock-status-menu-item.c \
	example-clock-status-menu-item.h

example_icon_status_menu_item_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	-DHILDON_DATA_DIR=\"$(hildondatadir)\"

example_icon_status_menu_item_la_LDFLAGS = \
	-module -avoid-version

example_icon_status_menu_item_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_icon_status_menu_item_la_SOURCES = \
	example-icon-status-menu-item.c \
	example-icon-status-menu-item.h

example_button_status_menu_item_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	-DHILDON_DATA_DIR=\"$(hildondatadir)\"

example_button_status_menu_item_la_LDFLAGS = \
	-module -avoid-version

example_button_status_menu_item_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_button_status_menu_item_la_SOURCES = \
	example-button-status-menu-item.c \
	example-button-status-menu-item.h

example_volume_status_menu_item_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	-DHILDON_DATA_DIR=\"$(hildondatadir)\"

example_volume_status_menu_item_la_LDFLAGS = \
	-module -avoid-version

example_volume_status_menu_item_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_volume_status_menu_item_la_SOURCES = \
	example-volume-status-menu-item.c \
	example-volume-status-menu-item.h

hildonstatusmenudesktopentry_DATA = example-clock.desktop example-icon.desktop example-button.desktop example-volume.desktop
