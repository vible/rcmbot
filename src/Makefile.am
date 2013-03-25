bin_PROGRAMS = rcmbot

AM_CFLAGS = -Wall -Wextra -pedantic $(libcurl_CFLAGS) $(libxml2_CFLAGS)

rcmbot_LDADD = $(libcurl_LIBS) $(libxml2_LIBS)
rcmbot_SOURCES = includes.h irc.c irc.h settings.c settings.h rcmbot.c\
		 xmem.c xmem.h xml.c xml.h
