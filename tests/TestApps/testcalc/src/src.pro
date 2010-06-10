
BASICDIR = basiccalc
COREINTERFACEDIR = coreinterface
DOMDIR = dom
WIDGETSDIR = widgets
UTILITYDIR = utility

INCLUDEPATH += . $$BASICDIR $$COREINTERFACEDIR $$DOMDIR $$WIDGETSDIR $$UTILITYDIR \
    /usr/include/meegotouch

QMAKE_LIBDIR += /usr/lib/meegotouch

# BACKING OFF FAST LAUNCH
QMAKE_CXXFLAGS += -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
QMAKE_LFLAGS += -pie -rdynamic

LIBS += -lmeegotouchcore -lmeegotouchviews -lmeegotouchsettings -lmeegotouchextensions
DEPENDPATH += $$INCLUDEPATH
CONFIG += gui meegotouch
QT += svg \
    dbus \
    xml

TEMPLATE = app
TARGET = ../testcalc
# BACKING OFF FAST LAUNCH
# TARGET = essentials.launch
target.path = /usr/bin
OBJECTS_DIR = ./.obj
MOC_DIR = ./.moc

# BACKING OFF FAST LAUNCH
# launcher.files = essentials
# launcher.path = /usr/bin

MODEL_HEADERS += $$WIDGETSDIR/calculationhistoryitemmodel.h $$WIDGETSDIR/savedcalculationitemmodel.h $$WIDGETSDIR/calculationlinewidgetmodel.h
STYLE_HEADERS += $$WIDGETSDIR/calculationhistoryitemstyle.h $$WIDGETSDIR/savedcalculationitemstyle.h $$WIDGETSDIR/calculationlinewidgetstyle.h

SOURCES += $$BASICDIR/main.cpp \
    $$BASICDIR/basiccalc.cpp \
    $$BASICDIR/savedcalculations.cpp \
    $$BASICDIR/calculationhistory.cpp \
    $$BASICDIR/calcretranslator.cpp \
    $$COREINTERFACEDIR/coreinterface.cpp \
    $$COREINTERFACEDIR/calculationstack.cpp \
    $$DOMDIR/calcdomreader.cpp \
    $$DOMDIR/calcdomwriter.cpp \
    $$WIDGETSDIR/calculationhistoryitem.cpp \
    $$WIDGETSDIR/calculationhistoryitemview.cpp \
    $$WIDGETSDIR/savedcalculationitem.cpp \
    $$WIDGETSDIR/savedcalculationitemview.cpp \
    $$WIDGETSDIR/calculationlinewidget.cpp \
    $$WIDGETSDIR/calculationlinewidgetview.cpp \
    $$UTILITYDIR/calcutilities.cpp

HEADERS += $$BASICDIR/basiccalc.h \
    $$BASICDIR/savedcalculations.h \
    $$BASICDIR/calculationhistory.h \
    $$BASICDIR/calcretranslator.h \
    $$BASICDIR/calcconstants.h \
    $$COREINTERFACEDIR/coreinterface.h \
    $$COREINTERFACEDIR/calculationstack.h \
    $$DOMDIR/calcdomreader.h \
    $$DOMDIR/calcdomwriter.h \
    $$WIDGETSDIR/calculationhistoryitem.h \
    $$WIDGETSDIR/calculationhistoryitemview.h \
    $$WIDGETSDIR/calculationhistoryitemmodel.h \
    $$WIDGETSDIR/calculationhistoryitemview_p.h \
    $$WIDGETSDIR/calculationhistoryitemstyle.h \
    $$WIDGETSDIR/savedcalculationitem.h \
    $$WIDGETSDIR/savedcalculationitemview.h \
    $$WIDGETSDIR/savedcalculationitemmodel.h \
    $$WIDGETSDIR/savedcalculationitemview_p.h \
    $$WIDGETSDIR/savedcalculationitemstyle.h \
    $$WIDGETSDIR/calculationlinewidget.h \
    $$WIDGETSDIR/calculationlinewidgetview.h \
    $$WIDGETSDIR/calculationlinewidgetmodel.h \
    $$WIDGETSDIR/calculationlinewidgetview_p.h \
    $$WIDGETSDIR/calculationlinewidgetstyle.h \
    $$UTILITYDIR/calcutilities.h


# Install instructions
INSTALLS += \
	target 
	# BACKING OFF FAST LAUNCH
	# \
	# launcher


