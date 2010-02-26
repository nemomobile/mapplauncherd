## Process this file with autoconf to produce a configure script.
#

AC_INIT([maemo-launcher], [0.35])
AC_PREREQ([2.59])
AC_CONFIG_SRCDIR([launcher/launcher.c])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.7 foreign])
AM_MAINTAINER_MODE

AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_C_CONST

AC_HEADER_STDC

AC_MSG_CHECKING([for prctl with PR_SET_NAME support])
AC_LINK_IFELSE([
#include <sys/prctl.h>
int main()
{
  char name[] = "name-test";
  prctl(PR_SET_NAME, name);
  return 0;
}
], [AC_DEFINE([HAVE_PRCTL_SET_NAME], [1],
              [Define to 1 if prctl has PR_SET_NAME support])
    prctl=yes],
   [prctl=no])
AC_MSG_RESULT([$prctl])

#
# Launcher dependencies
#

PKG_CHECK_MODULES([DBUS], [
	dbus-1 >= 0.60
])

#
# Booster modules section
#

#AC_ARG_ENABLE([hildon-booster],
#	[AS_HELP_STRING([--enable-hildon-booster],
#			[link with hildon libs to reduce runtime linker time])],
#	[case "${enableval}" in
#	  (yes) hildon_booster=true ;;
#	  (no)  hildon_booster=false ;;
#	  (*) AC_MSG_ERROR([bad value ${enableval} for --enable-hildon-booster]) ;;
#	esac], [hildon_booster=false])

#PKG_CHECK_MODULES([BOOSTER_GTK], [
#	gtk+-2.0 >= 2.6.4
#	fontconfig
#])

#if test x$hildon_booster = xtrue
#then

#PKG_CHECK_MODULES([BOOSTER_HILDON], [
#	glib-2.0
#	gio-2.0
#	libpulse
#	gnome-vfs-2.0 >= 2.8.0
#	alarm
#	libosso
#	libhildonmime
#	hildon-fm-2
#	libhildondesktop-1
#	hildon-1
#	libi18n
#	telepathy-glib
#	libedataserver-1.2
#	libedata-book-1.2
#	libmodest-dbus-client-1.0
#	sqlite3
#	conic
#])

# Hack to add some more common items not available via pkg-config.

#LIBS="$LIBS -ljpeg -lgvfscommon"

#fi

#AC_ARG_ENABLE([qt-booster],
#	[AS_HELP_STRING([--enable-qt-booster],
#			[link with QT libs to reduce runtime linker time])],
#	[case "${enableval}" in
#	  (yes) qt_booster=true ;;
#	  (no)  qt_booster=false ;;
#	  (*) AC_MSG_ERROR([bad value ${enableval} for --enable-qt-booster]) ;;
#	esac], [qt_booster=false])
#AM_CONDITIONAL([USE_QT_BOOSTER], [test x$qt_booster = xtrue])

#if test x$qt_booster = xtrue
#then

#PKG_CHECK_MODULES([BOOSTER_QT], [
#	QtCore
#	QtDBus
#	QtGui
#])

#fi 

#AC_ARG_ENABLE([cpp-booster],
#	[AS_HELP_STRING([--enable-cpp-booster],
#		[link with libstdc++ to reduce runtime linker time])],
#	[case "${enableval}" in
#		(yes) cpp_booster=true ;;
#		(no)  cpp_booster=false ;;
#		(*) AC_MSG_ERROR([bad value ${enableval} for --enable-cpp-booster]) ;;
#	esac], [cpp_booster=false])
#AM_CONDITIONAL([USE_CPP_BOOSTER], [test x$cpp_booster = xtrue])

#if test x$cpp_booster = xtrue
#then
#GLIBS=$LIBS
#AC_CHECK_LIB(stdc++, main,,AC_MSG_ERROR(gdu configure option requires libstdc++))
#LIBS=$GLIBS
#fi

#
# Test client program
#

PIC="-fPIC"
AC_SUBST([PIC])

AC_ARG_ENABLE([pie],
	[AS_HELP_STRING([--enable-pie],
			[link the client as a PIE])],
	[case "${enableval}" in
	  yes) pie=true ;;
	  no)  pie=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-pie]) ;;
	esac], [pie=false])
AM_CONDITIONAL([USE_PIE], [test x$pie = xtrue])

PKG_CHECK_MODULES([APP_HILDON], [
	gtk+-2.0 >= 2.6.4
	hildon-1
], [build_app_hildon=true], [build_app_hildon=false])
AM_CONDITIONAL([BUILD_APP_HILDON], [test x$build_app_hildon = xtrue])

AC_ARG_ENABLE([client],
	[AS_HELP_STRING([--enable-client],
			[build the test client])],
	[case "${enableval}" in
	  yes) client=true ;;
	  no)  client=false ;;
	  *) AC_MSG_ERROR([bad value ${enableval} for --enable-client]) ;;
	esac], [client=false])
AM_CONDITIONAL([BUILD_CLIENT], [test x$client = xtrue])

if test x$client = xtrue
then

PKG_CHECK_MODULES([CLIENT], [
	$GTK_DEPENDS
	hildon-1
])

fi

AC_CONFIG_FILES([
	Makefile
	launcher/Makefile
	launcher/maemo-launcher-app.pc:launcher/launcher.pc.in
	launcher/maemo-invoker.1:launcher/invoker.1.in
	launcher/maemo-summoner.1:launcher/summoner.1.in
	launcher/maemo-launcher.1:launcher/launcher.1.in
	client/Makefile
])
AC_OUTPUT
