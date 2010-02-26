## Process this file with automake to produce Makefile.in
#

helperdir = $(libdir)/maemo-launcher
helper_PROGRAMS = defender

DEFENDER = "\"$(helperdir)/defender\""
LAUNCHER = "\"$(bindir)/maemo-launcher\""

defender_SOURCES = defender.c report.c report.h
defender_CPPFLAGS = \
	-DPROG_NAME="\"defender\"" \
	-DLAUNCHER=$(LAUNCHER) \
	-DDEFENDER=$(DEFENDER)

bin_PROGRAMS = maemo-launcher maemo-invoker maemo-summoner

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	maemo-launcher-app.pc 

maemo_launcherdir = $(pkgincludedir)
maemo_launcher_HEADERS = 
maemo_launcher_SOURCES = \
	launcher.c report.c report.h  \
	invokelib.c invokelib.h \
	comm_msg.c comm_msg.h comm_dbus.c comm_dbus.h \
	prog.c prog.h
maemo_launcher_CPPFLAGS = \
	$(DBUS_CFLAGS) \
	-DPROG_NAME="\"maemo-launcher\"" \
	-DDEFENDER=$(DEFENDER) \
	-DHAVE_PRCTL_SET_NAME
maemo_launcher_LDADD = -ldl $(DBUS_LIBS) -L/usr/share/qt3/lib -L/usr/X11R6/lib -L/usr/lib/ -ldui -lX11 -lm -lpthread
# FIXME: Use rdynamic for now until the boosters do not use the report
#        functions.
maemo_launcher_LDFLAGS = -rdynamic

maemo_invoker_SOURCES = \
	invoker.c invokelib.c invokelib.h report.c report.h \
	search.c search.h
maemo_invoker_CPPFLAGS = -DPROG_NAME="\"maemo-invoker\""

maemo_summoner_SOURCES = \
	summoner.c report.c report.h prog.c prog.h \
	search.c search.h
maemo_summoner_CPPFLAGS = -DPROG_NAME="\"maemo-summoner\""
maemo_summoner_LDADD = -ldl

check_PROGRAMS = test-msg test-msg-perf

test_msg_SOURCES = test_lib.h test_lib.c test_msg.c comm_msg.c report.c
test_msg_CPPFLAGS = -DPROG_NAME="\"test-msg\""

test_msg_perf_SOURCES = test_msg_perf.c comm_msg.c report.c
test_msg_perf_CPPFLAGS = -DPROG_NAME="\"test-msg-perf\""

TESTS = $(check_PROGRAMS)

man_MANS = \
	maemo-launcher.1 \
	maemo-invoker.1 \
	maemo-summoner.1

EXTRA_DIST = \
	launcher.pc.in \
	launcher.1.in \
	invoker.1.in \
	summoner.1.in

