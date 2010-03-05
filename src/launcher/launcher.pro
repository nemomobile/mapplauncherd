INCLUDEPATH += . 

DEPENDPATH += $$INCLUDEPATH
QT         -= gui
TARGET      = applauncherd
target.path = /usr/bin
CONFIG     += console
CONFIG     -= app_bundle
TEMPLATE    = app
DEFINES    += PROG_NAME=\\\"launcher\\\"

# Check if needed dui headers exist
DUISRCINCLUDE = /usr/include/dui

exists($$DUISRCINCLUDE/duicomponentcache.h) {
    message(Building with DUI Booster)
    DEFINES += HAVE_DUI
} else {
    message(!! DUI Booster not available !!)
}

contains(DEFINES, HAVE_DUI) {
    INCLUDEPATH += $$DUISRCINCLUDE 
    LIBS        += -ldui
}

SOURCES += logger.cpp \
    main.cpp \
    daemon.cpp \
    connection.cpp \
    booster.cpp \
    qtbooster.cpp \
    duibooster.cpp

HEADERS += logger.h \
    daemon.h \
    connection.h \
    booster.h \
    qtbooster.h \
    duibooster.h \
    appdata.h

INSTALLS += target 

scripts.path = /usr/bin
scripts.files = scripts/*

INSTALLS += scripts

