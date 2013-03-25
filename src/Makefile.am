bin_PROGRAMS = rcmbot

AM_CFLAGS = -Wall -Wextra -pedantic $(LIBCURL_CFLAGS) $(LIBXML_CFLAGS)

rcmbot_LDADD = $(LIBCURL_LIBS) $(LIBXML_LIBS)
rcmbot_SOURCES = includes.h irc.c irc.h settings.c settings.h rcmbot.c\
		 xmem.c xmem.h xml.c xml.h
