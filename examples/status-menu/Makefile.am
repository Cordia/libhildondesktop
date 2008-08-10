hildondatadir = $(datadir)/hildon-desktop
hildondata_DATA = \
	example-icon.png			\
	example-status-area-icon-1.png		\
	example-status-area-icon-2.png		\
	example-status-area-icon-3.png		\
	example-status-area-icon-4.png		\
	example-status-area-icon-battery.png	\
	example-status-area-icon-signal.png

hildondesktoplib_LTLIBRARIES = \
	example-clock-status-menu-item.la	\
	example-icon-status-menu-item.la	\
	example-button-status-menu-item.la	\
	example-volume-status-menu-item.la	\
	example-signal-status-area-item.la	\
	example-battery-status-area-item.la

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

example_signal_status_area_item_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	-DHILDON_DATA_DIR=\"$(hildondatadir)\"

example_signal_status_area_item_la_LDFLAGS = \
	-module -avoid-version

example_signal_status_area_item_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_battery_status_area_item_la_SOURCES = \
	example-battery-status-area-item.c \
	example-battery-status-area-item.h

example_battery_status_area_item_la_CPPFLAGS = \
	$(HILDON_CFLAGS)			\
	$(DBUS_CFLAGS)				\
	-DHILDON_DATA_DIR=\"$(hildondatadir)\"

example_battery_status_area_item_la_LDFLAGS = \
	-module -avoid-version

example_battery_status_area_item_la_LIBADD = \
	$(HILDON_LIBS)				\
	$(DBUS_LIBS)

example_battery_status_area_item_la_SOURCES = \
	example-battery-status-area-item.c \
	example-battery-status-area-item.h

hildonstatusmenudesktopentry_DATA = \
	example-clock.desktop	\
	example-icon.desktop	\
	example-button.desktop	\
	example-volume.desktop	\
	example-signal.desktop	\
	example-battery.desktop
