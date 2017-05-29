bin_PROGRAMS = bibledit-cloud

bibledit_cloud_SOURCES = main.cpp

AM_CFLAGS = -Wall -Wextra -pedantic -g

AM_CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -fno-var-tracking -g

LIBS = -Wall -lpthread -L. -g -rdynamic -ldl

EXTRA_DIST = debian

CLEANFILES = bibledit*.gz *~

dist-hook:
	rm -rf `find $(distdir) -name *.o`
	rm -rf `find $(distdir) -name *.a`
	rm -rf `find $(distdir) -name autom4te.cache`
	rm -f $(distdir)/server
	rm -rf `find $(distdir) -name .dirstamp`
	rm -rf `find $(distdir) -name .deps`
	rm -rf `find $(distdir) -name *~`
	rm -rf `find $(distdir) -name ._*`
	rm -rf `find $(distdir) -name .DS_Store`

