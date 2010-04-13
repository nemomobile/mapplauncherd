# Find Dui libraries using pkg-config. 
include(FindPkgConfig) 
pkg_check_modules(DUI Dui>=0.19 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${DUI_INCLUDE_DIRS})

# Check if it's possible to compile DuiBooster with DuiComponentCache. If so,
# HAVE_DUI is set.
message(STATUS "checking for duicomponentcache.h")
find_file(DUI_COMPONENT_CACHE NAMES duicomponentcache.h PATHS ${DUI_INCLUDE_DIRS})
if (EXISTS ${DUI_COMPONENT_CACHE})
  message(STATUS "  found")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_DUI")
else (EXISTS ${DUI_COMPONENT_CACHE})
  message(STATUS "  not found")
endif (EXISTS ${DUI_COMPONENT_CACHE})

# Set sources
set(SRC booster.cpp connection.cpp daemon.cpp duibooster.cpp logger.cpp main.cpp qtbooster.cpp)

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Set debug logging
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")

link_libraries(${QT_QTCORE_LIBRARY} ${DUI_LIBRARIES})

# Qt support
include(${QT_USE_FILE})

# Set executable
add_executable(applauncherd.bin ${SRC})

# Install rules
install(PROGRAMS applauncherd.bin scripts/applauncherd DESTINATION /usr/bin/)

