## Process this file with automake to produce Makefile.in

version_script = app.Version

EXTRA_DIST = $(version_script)

bin_PROGRAMS = app-console

app_console_CPPFLAGS = $(PIC)
app_console_LDFLAGS = \
	-rdynamic -Wl,--version-script=$(srcdir)/$(version_script) \
	-shared $(PIC)

if BUILD_APP_HILDON
bin_PROGRAMS += app-hildon

app_hildon_SOURCES = app-hildon.c
app_hildon_CPPFLAGS = $(APP_HILDON_CFLAGS) $(PIC)
app_hildon_LDFLAGS = \
	-rdynamic -Wl,--version-script=$(srcdir)/$(version_script) \
	-shared $(PIC)
app_hildon_LDADD = $(APP_HILDON_LIBS)
endif

