TARGET = invoker
target.path = /usr/bin
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

# Invoker originally had config.h (gen by autotools) with these defined
DEFINES += PROG_NAME=\\\"invoker\\\" \
    PACKAGE=\\\"maemo-launcher\\\" \
    VERSION=\\\"0.35\\\"
TEMPLATE = app
SOURCES += invokelib.c \
    invoker.c \
    report.c \
    search.c
HEADERS += invokelib.h \
    config.h \
    search.h \
    report.h \
    ../common/protocol.h
INSTALLS += target
