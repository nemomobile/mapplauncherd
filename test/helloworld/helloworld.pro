DUISRCINCLUDE = /usr/include/dui

INCLUDEPATH += . \
    $$DUISRCINCLUDE \

LIBS += -ldui

scripts.path = /usr/bin
scripts.files = scripts/*

INSTALLS += scripts

QMAKE_CXXFLAGS += -shared -fPIC
QMAKE_LFLAGS += -shared -fPIC -export-dynamic

target.path = /usr/bin/
qINSTALLS += target

TEMPLATE = app
TARGET = helloworld.launch
DEPENDPATH += $$INCLUDEPATH

SOURCES += \
    helloworld.cpp 
