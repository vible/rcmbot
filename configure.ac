dnl vibbot ./confix
AC_INIT([rcmbot], [1.0])
AC_CONFIG_SRCDIR([src/rcmbot.c])
AM_INIT_AUTOMAKE

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_SRCDIR([src/rcmbot.c])

AC_PROG_CC
AC_C_CONST
AC_TYPE_SIZE_T

dnl CHECK HEADERS
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h netdb.h stdlib.h string.h unistd.h])

dnl CHECK FUNCS
AC_CHECK_FUNCS([gethostbyname memmove select socket strchr strdup])
AC_FUNC_MALLOC
AC_FUNC_VPRINTF

dnl CHECK for libs using pkgconfig
PKG_CHECK_MODULES(libcurl, libcurl >= 7.0)
PKG_CHECK_MODULES(libxml2, libxml-2.0 >= 2.4)

AC_OUTPUT([
	   src/Makefile
	   Makefile
	   ])
