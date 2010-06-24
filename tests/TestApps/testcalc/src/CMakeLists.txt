project(testcalc)

# see /usr/share/qt4/mkspecs/features/meegotouch*
macro(meego_mgen mode outvar headers)
    foreach(header ${headers})
      get_filename_component(basename ${header} NAME_WE)

      #message("mgen --${mode} ${header} (=> gen_${basename}data)")

      add_custom_command(OUTPUT "gen_${basename}data.cpp"
        COMMAND mgen ARGS "--${mode}" ${header}
        VERBATIM
        )

      list(APPEND ${outvar} gen_${basename}data.cpp)
    endforeach(header)
endmacro(meego_mgen)

# This macro makes model/style generation as easy as with qmake. Just define
# model_HEADERS and style_HEADERS and call this macro. Rules will be generated
# to generate necessary files.
#
# By default the names of the generated *.cpp files are appended to
# projectname_MGEN_SOURCES, but you can override that by calling the macro as
# meego_mgen_wrapper(outvar).
#
# Remember to include the projectname_MGEN_SOURCES list in your target!
macro(meego_mgen_wrapper)
  # outvar is optional, defaults to projectname_MGEN_SOURCES
  set(outvar ${ARGV0})

  if(NOT outvar)
    set(outvar ${PROJECT_NAME}_MGEN_SOURCES)
  endif(NOT outvar)

  if(DEFINED model_HEADERS)
    meego_mgen(model ${outvar} "${model_HEADERS}")
  endif(DEFINED model_HEADERS)

  if(DEFINED style_HEADERS)
    meego_mgen(style ${outvar} "${style_HEADERS}")
  endif(DEFINED style_HEADERS)
endmacro(meego_mgen_wrapper)

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

