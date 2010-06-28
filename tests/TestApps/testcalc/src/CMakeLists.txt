project(testcalc)

include(meegotouch.cmake)

include_directories(basiccalc coreinterface dom widgets utility)

set(testcalc_SRCS
  ./basiccalc/main.cpp
  ./basiccalc/basiccalc.cpp
  ./basiccalc/savedcalculations.cpp
  ./basiccalc/calculationhistory.cpp
  ./basiccalc/calcretranslator.cpp

  ./coreinterface/coreinterface.cpp
  ./coreinterface/calculationstack.cpp

  ./dom/calcdomreader.cpp
  ./dom/calcdomwriter.cpp

  ./widgets/calculationhistoryitem.cpp
  ./widgets/calculationhistoryitemview.cpp

  ./widgets/savedcalculationitem.cpp
  ./widgets/savedcalculationitemview.cpp

  ./widgets/calculationlinewidget.cpp
  ./widgets/calculationlinewidgetview.cpp

  ./utility/calcutilities.cpp
)

# moc should be run on these headers
set(testcalc_MOC_HDRS
  ./basiccalc/basiccalc.h
  ./basiccalc/savedcalculations.h
  ./basiccalc/calculationhistory.h
  ./basiccalc/calcretranslator.h

  ./coreinterface/coreinterface.h

  ./widgets/calculationhistoryitem.h
  ./widgets/calculationhistoryitemview.h
  ./widgets/calculationhistoryitemview_p.h
  ./widgets/calculationhistoryitemstyle.h
  ./widgets/calculationhistoryitemmodel.h

  ./widgets/savedcalculationitem.h
  ./widgets/savedcalculationitemview.h
  ./widgets/savedcalculationitemview_p.h
  ./widgets/savedcalculationitemmodel.h
  ./widgets/savedcalculationitemstyle.h

  ./widgets/calculationlinewidget.h
  ./widgets/calculationlinewidgetview.h
  ./widgets/calculationlinewidgetview_p.h
  ./widgets/calculationlinewidgetmodel.h
  ./widgets/calculationlinewidgetstyle.h

  ./utility/calcutilities.h
)

# mgen --model for these
set(model_HEADERS
  ./widgets/calculationhistoryitemmodel.h  
  ./widgets/savedcalculationitemmodel.h
  ./widgets/calculationlinewidgetmodel.h
)

# mgen --style for these
set(style_HEADERS
  ./widgets/calculationhistoryitemstyle.h
  ./widgets/savedcalculationitemstyle.h
  ./widgets/calculationlinewidgetstyle.h
)

# generate moc_* files
qt4_wrap_cpp(testcalc_MOC_SRCS ${testcalc_MOC_HDRS})

# generate model/style files
meego_mgen_wrapper()

link_libraries(${MEEGOTOUCH_LIBRARIES})

# use QtXml, QtSvg and QtDBus
set(QT_USE_QTXML TRUE)
set(QT_USE_QTSVG TRUE)
set(QT_USE_QTDBUS TRUE)
include(${QT_USE_FILE})

set(APPLICATION_NAME "fala_calc")

add_executable(${APPLICATION_NAME} ${testcalc_SRCS} ${testcalc_MOC_SRCS} ${testcalc_MGEN_SOURCES})

set_target_properties(${APPLICATION_NAME} PROPERTIES
  COMPILE_FLAGS "-fPIC -fvisibility=hidden -fvisibility-inlines-hidden"
  LINK_FLAGS "-pie -rdynamic"
  COMPILE_DEFINITIONS M_APPLICATION_NAME="${APPLICATION_NAME}")

install(PROGRAMS ${APPLICATION_NAME} DESTINATION /usr/bin)

