AC_INIT([bibledit],[5.0.102],[http://bibledit.org])
AM_INIT_AUTOMAKE
AM_INIT_AUTOMAKE([tar-ustar subdir-objects])
AC_CANONICAL_BUILD
ac_default_prefix=/usr

AC_PROG_CC
AC_PROG_CXX
AC_LANG([C++])
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_PROG_RANLIB
AC_CONFIG_HEADER(config.h)

PKG_PROG_PKG_CONFIG
AC_PATH_PROG(PKG_CONFIG_PATH, pkg-config, no)
if test x$PKG_CONFIG_PATH = xno; then
  AC_MSG_ERROR(Program "pkg-config" is needed. Install this first.)
fi

PKG_CHECK_MODULES(GTK, gtk+-3.0,,AC_MSG_ERROR(Gtk3 development version is needed.))
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

PKG_CHECK_MODULES([WEBKIT2GTK], [webkit2gtk-4.0],, [
  PKG_CHECK_MODULES([WEBKIT2GTK], [webkit2gtk-3.0],,AC_MSG_ERROR(Webkit2Gtk development version is needed.))
])
AC_SUBST(WEBKIT2GTK_LIBS)
AC_SUBST(WEBKIT2GTK_CFLAGS)

if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/bibledit", [Package data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/bibledit", [Package data directory])
fi

AC_OUTPUT(Makefile)

echo "
Bibledit-Gtk $VERSION configuration summary:

Host operating system  : ${host_os}
Version                : ${PACKAGE_VERSION}
Installation prefix    : ${prefix}
"
