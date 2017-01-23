bin_PROGRAMS = bibledit

bibledit_SOURCES = bibledit.cpp

AM_CFLAGS = -Wall -Wextra -pedantic -g

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fno-var-tracking -g
AM_CXXFLAGS += $(GTK_CFLAGS)
AM_CXXFLAGS += $(WEBKIT2GTK_CFLAGS)

LIBS = -Wall -lpthread -L. -g -rdynamic -ldl
LIBS += $(GTK_LIBS)
LIBS += $(WEBKIT2GTK_LIBS)

EXTRA_DIST = *.h *.desktop *.xpm *.1

all-local:
	find . -type f ! -name '*.h' ! -name '*.cpp' ! -name '*.c' ! -name '*.o' ! -name '.DS_Store' ! -path '*xcode*' ! -path '*.deps*' ! -path '*unittests*' ! -name '*.tgz' ! -path '*autom4te*' -print > pkgdata.txt

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`
	rm -f $(distdir)/server
	rm -rf `find $(distdir) -name .dirstamp`
	rm -rf `find $(distdir) -name .deps`

CLEANFILES = bibledit*.gz *~

man_MANS = *.1

applicationdir = $(datadir)/applications
application_DATA = bibledit.desktop

appicondir = $(datadir)/pixmaps
appicon_DATA = bibledit.xpm

dist_pkgdata_DATA = pkgdata.txt
