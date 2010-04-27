TEMPLATE = app
TARGET = helloworld
CONFIG = qt meegotouch

QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_LFLAGS += -pie -rdynamic

SOURCES += helloworld.cpp
