INCLUDEPATH += .
DEPENDPATH += $$INCLUDEPATH
QT -= gui
TARGET = applauncherd
target.path = /usr/bin
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
DEFINES += PROG_NAME=\\\"launcher\\\"

# Check if needed m headers exist
MSRCINCLUDE = /usr/include/m
exists($$MSRCINCLUDE/mcomponentcache.h) { 
    message(Building with M Booster)
    DEFINES += HAVE_MCOMPONENTCACHE
}
else:message(!! MeeGo Touch Booster not available !!)
contains(DEFINES, HAVE_MCOMPONENTCACHE) { 
    INCLUDEPATH += $$MSRCINCLUDE
    LIBS += -lm
}
SOURCES += logger.cpp \
    main.cpp \
    daemon.cpp \
    connection.cpp \
    booster.cpp \
    qtbooster.cpp \
    mbooster.cpp
HEADERS += logger.h \
    daemon.h \
    connection.h \
    booster.h \
    qtbooster.h \
    mbooster.h \
    appdata.h \
    ../common/protocol.h
INSTALLS += target
scripts.path = /usr/bin
scripts.files = scripts/*
INSTALLS += scripts
