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

