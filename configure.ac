AC_INIT([libhildondesktop], [2.1.4], [], [libhildondesktop])
AC_CANONICAL_SYSTEM

AM_CONFIG_HEADER(config.h)
AC_CONFIG_SRCDIR([libhildondesktop/libhildondesktop.h])
AC_CONFIG_MACRO_DIR([m4])

AM_MAINTAINER_MODE

# define PACKAGE_VERSION_* variables
AS_VERSION

#define API_VERSION_MAJOR
API_VERSION_MAJOR=1
AC_SUBST(API_VERSION_MAJOR)

# set the libtool versioning
# Quick guide into libtool versioning:
# 1. Start with version information of `0:0:0' for each libtool library.
# 2. Update the version information only immediately before a public release of your software. 
#    More frequent updates are unnecessary, and only guarantee that the current interface number 
#    gets larger faster.
# 3. If the library source code has changed at all since the last update, then increment revision 
#    (`c:r:a' becomes `c:r+1:a').
# 4. If any interfaces have been added, removed, or changed since the last update, 
#    increment current, and set revision to 0.
# 5. If any interfaces have been added since the last public release, then increment age.
# 6. If any interfaces have been removed since the last public release, then set age to 0.  
AS_LIBTOOL(LIBHILDONDESKTOP, 0, 0, 0)
AM_INIT_AUTOMAKE

AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

AC_HEADER_STDC

AC_PATH_X
AC_PATH_XTRA
AC_SUBST(X_CFLAGS)

#+++++++++++++++
# Misc programs 
#+++++++++++++++

AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

GTK_DOC_CHECK([1.3])

#++++++++++++++++++
# Misc build flags
#++++++++++++++++++

AC_ARG_ENABLE(debug, 
	      [AC_HELP_STRING([--enable-debug],[Debug (default=no)])],
              [hildon_use_debug=yes],[hildon_use_debug=no])

AC_ARG_ENABLE(timestamping,
	      [AC_HELP_STRING([--enable-timestamping],[Define HILDON_USE_TIMESTAMPING (default=no)])],
	      [hildon_use_timestamping=yes],[hildon_use_timestamping=no])

AC_ARG_ENABLE(instrumenting,
	      [AC_HELP_STRING([--enable-instrumenting],[Compile with instrumentation flags (default=no)])],
	      [hildon_use_instrumenting=yes],[hildon_use_instrumenting=no])

if test "x${hildon_use_timestamping}" = "xyes"
then
    CFLAGS="$CFLAGS -DHILDON_USE_TIMESTAMPING"
fi

if test "x${hildon_use_instrumenting}" = "xyes"
then
    CFLAGS="$CFLAGS -Wall -Wmissing-prototypes -Wmissing-declarations -Werror -Wno-format-extra-args -g -finstrument-functions"
else
    CFLAGS="$CFLAGS -Wall -Wmissing-prototypes -Wmissing-declarations -Werror -Wno-format-extra-args"
fi

AC_SUBST(CFLAGS)

#++++++++++++
# i18n setup 
#++++++++++++

# ALL_LINGUAS="en_GB"
# AC_SUBST(ALL_LINGUAS)
# AM_GLIB_GNU_GETTEXT
# AC_DEFINE(GETTEXT_PACKAGE, ["libhildondesktop"], [Package name for gettext])

PKG_CHECK_MODULES(HILDON,
                  [hildon-1], 
                  [AC_DEFINE(HAVE_LIBHILDON, [], [Whether libhildon-1 is present on the system])], 
                  [PKG_CHECK_MODULES(HILDON, hildon-1 >= 2.1.0)])

AC_SUBST(HILDON_LIBS)
AC_SUBST(HILDON_CFLAGS)

PKG_CHECK_MODULES(GNOME_VFS, gnome-vfs-2.0 >= 2.8.3)
AC_SUBST(GNOME_VFS_CFLAGS)
AC_SUBST(GNOME_VFS_LIBS)

PKG_CHECK_MODULES(DBUS, dbus-1 >= 1.0.2 dbus-glib-1 >= 0.74)
AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)

AC_CHECK_LIB([iphb], [iphb_open])

#+++++++++++++++++++
# Directories setup
#+++++++++++++++++++


hildondesktoplibdir="${libdir}/hildon-desktop"
                  
hildonhomedesktopentrydir="${datadir}/applications/hildon-home"
hildonnotificationdesktopentrydir="${datadir}/applications/hildon-notification"
hildonstatusmenudesktopentrydir="${datadir}/applications/hildon-status-menu"

hildondesktopconfdir=${sysconfdir}/hildon-desktop
AC_SUBST(hildondesktoplibdir)
AC_SUBST(hildondesktopconfdir)

AC_SUBST(hildonstatusmenudesktopentrydir)
AC_SUBST(hildonhomedesktopentrydir)
AC_SUBST(hildonnotificationdesktopentrydir)

# Expand nested variables
# eval "hildonstatusmenudesktopentrydir=$hildonstatusmenudesktopentrydir"

hildondesktopbackgroundsdir=${datadir}/backgrounds
AC_SUBST(hildondesktopbackgroundsdir)

hildoncpdesktopentrydir=${datadir}/applications/hildon-control-panel
AC_SUBST(hildoncpdesktopentrydir)

hildonpluginloaderlibdir=${hildondesktoplibdir}/loaders
AC_SUBST(hildonpluginloaderlibdir)

hildonuipolicylibdir=${hildondesktoplibdir}/policies
AC_SUBST(hildonuipolicylibdir)

hildondesktopstartupdir=${sysconfdir}/osso-af-init
AC_SUBST(hildondesktopstartupdir)

hildondesktopmenudir=${sysconfdir}/xdg/menus
AC_SUBST(hildondesktopmenudir)

AC_CONFIG_FILES([			\
Makefile				\
doc/Makefile				\
doc/version.xml				\
examples/Makefile			\
examples/home/Makefile			\
examples/notification/Makefile		\
examples/status-menu/Makefile		\
libhildondesktop/Makefile		\
libhildondesktop/libhildondesktop.pc	\
])

AC_OUTPUT
