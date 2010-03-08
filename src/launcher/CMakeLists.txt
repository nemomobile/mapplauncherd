# Set sources
set(LauncherSRC booster.cpp connection.cpp daemon.cpp duibooster.cpp logger.cpp main.cpp qtbooster.cpp)

if (EXISTS $ENV{DUI_INCLUDE_DIR})
  set(DUISRCINCLUDE $ENV{DUI_INCLUDE_DIR})
  message(STATUS "DUI included from " ${DUISRCINCLUDE})
else (EXISTS $ENV{DUI_INCLUDE_DIR})
  set(DUISRCINCLUDE /usr/include/dui)
  message(STATUS "DUI included from " ${DUISRCINCLUDE} ", use DUI_INCLUDE_DIR to override.")
endif (EXISTS $ENV{DUI_INCLUDE_DIR})

# Set include dirs
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${DUISRCINCLUDE})

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Check if it's possible to compile DuiBooster
if(EXISTS ${DUISRCINCLUDE}/duicomponentcache.h)
  if (EXISTS $ENV{DUI_LIBRARY_FILE})
    set(dui_LIBRARY $ENV{DUI_LIBRARY_FILE})
  else (EXISTS $ENV{DUI_LIBRARY_FILE})
    find_library(dui_LIBRARY NAMES dui PATHS /usr/lib /usr/local/lib)
  endif (EXISTS $ENV{DUI_LIBRARY_FILE})
  
  if (EXISTS ${dui_LIBRARY})
    message(STATUS "Linking to " ${dui_LIBRARY} ", use DUI_LIBRARY_FILE to override.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_DUI")
    message(STATUS "Building with DUI Booster")
  endif (EXISTS ${dui_LIBRARY})
else(EXISTS ${DUISRCINCLUDE}/duicomponentcache.h)
  message(STATUS "!! DUI Booster not available !!")
endif(EXISTS ${DUISRCINCLUDE}/duicomponentcache.h)

# Set libraries to be linked (dui_LIBRARY may be empty)
link_libraries(${QT_QTCORE_LIBRARY} ${dui_LIBRARY})

# Qt support
include(${QT_USE_FILE})

# Set executable
add_executable(applauncherd.bin ${LauncherSRC})

# Install rules
install(PROGRAMS applauncherd.bin scripts/applauncherd DESTINATION /usr/bin/)

