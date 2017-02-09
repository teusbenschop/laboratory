bin_PROGRAMS = bibledit

bibledit_SOURCES = src/bibledit.cpp

AM_CFLAGS = -Wall -Wextra -pedantic -g

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fno-var-tracking -g
AM_CXXFLAGS += $(GTK_CFLAGS)
AM_CXXFLAGS += $(WEBKIT2GTK_CFLAGS)

LIBS = -Wall -lpthread -L. -g -rdynamic -ldl
LIBS += $(GTK_LIBS)
LIBS += $(WEBKIT2GTK_LIBS)

EXTRA_DIST = debian src *.desktop *.xpm *.1

CLEANFILES = bibledit*.gz *~

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`
	rm -f $(distdir)/server
	rm -rf `find $(distdir) -name .dirstamp`
	rm -rf `find $(distdir) -name .deps`
	rm -rf `find $(distdir) -name *~`

man_MANS = *.1

applicationdir = $(datadir)/applications
application_DATA = bibledit.desktop

appicondir = $(datadir)/pixmaps
appicon_DATA = bibledit.xpm

install-data-hook:
	rsync --archive -v --delete --exclude "*.h" --exclude "*.c" --exclude "*.cpp" --exclude "*.o" --exclude "*.deps*" --exclude "*autom4te.cache*" --exclude ".dirstamp" --exclude ".DS_Store" --exclude "bibledit" --exclude "*~" --exclude "COPYING"  $(srcdir) $(DESTDIR)$(pkgdatadir)

uninstall-local:
	rm -rf $(DESTDIR)$(pkgdatadir)/*

