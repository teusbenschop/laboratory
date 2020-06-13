AC_INIT([laboratory],[1.2.3],[http://bibledit.org])
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

if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/laboratory", [Package data directory])
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/laboratory", [Package data directory])
fi

appstreamdir = $(datadir)/metainfo/
appstream_in_files = gedit.metainfo.xml.in
appstream_DATA = $(appstream_in_files:.xml.in=.xml)
@INTLTOOL_XML_RULE@
EXTRA_DIST = $(metainfo_in_files)
CLEANFILES = $(appstream_DATA)

AC_OUTPUT(Makefile)

echo "
Laboratory $VERSION configuration summary:

Host operating system  : ${host_os}
Version                : ${PACKAGE_VERSION}
Installation prefix    : ${prefix}
"
