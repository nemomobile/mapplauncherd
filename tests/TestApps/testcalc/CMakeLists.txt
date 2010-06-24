set(CMAKE_VERBOSE_MAKEFILE ON)

# Set default C++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -g -O2")

cmake_minimum_required(VERSION 2.6)

# By default only QtCore and QtGui modules are enabled
#set(QT_USE_QTDBUS TRUE)

# Find Qt4
find_package(Qt4 4.6.0 REQUIRED)
include(${QT_USE_FILE})
if (${QTVERSION} VERSION_LESS 4.6.0)
    message(FATAL_ERROR "You need Qt4.6, found ${QTVERSION}.")
endif (${QTVERSION} VERSION_LESS 4.6.0)

# Find libmeegotouch for the test programs
include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

# Check if it's possible to compile MBooster with MComponentCache. If so,
# HAVE_MCOMPONENTCACHE is set.
message(STATUS "checking for mcomponentcache.h")
find_file(M_COMPONENT_CACHE NAMES mcomponentcache.h PATHS ${MEEGOTOUCH_INCLUDE_DIRS})
if (EXISTS ${M_COMPONENT_CACHE})
  message(STATUS "  found")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_MCOMPONENTCACHE")
else (EXISTS ${M_COMPONENT_CACHE})
  message(STATUS "  not found")
endif (EXISTS ${M_COMPONENT_CACHE})

add_subdirectory(src)
add_subdirectory(translations)

# Install themes
install(DIRECTORY themes/ DESTINATION /usr/share/themes/base/meegotouch/testcalc)