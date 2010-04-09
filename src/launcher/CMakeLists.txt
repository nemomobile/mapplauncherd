# Find Dui libraries using pkg-config. 
include(FindPkgConfig) 
pkg_check_modules(DUI Dui>=0.19 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${DUI_INCLUDE_DIRS})

# Check for DuiComponentCache header
include(CheckIncludeFile)
set(CMAKE_REQUIRED_INCLUDES ${DUI_INCLUDE_DIRS})
check_include_file(duicomponentcache.h DUI_COMPONENT_CACHE)

# Set sources
set(SRC booster.cpp connection.cpp daemon.cpp duibooster.cpp logger.cpp main.cpp qtbooster.cpp)

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Set debug logging
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")

# Check if it's possible to compile DuiBooster with DuiComponentCache. If so,
# HAVE_DUI is set.
if (${DUI_COMPONENT_CACHE})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_DUI")
endif(${DUI_COMPONENT_CACHE})

link_libraries(${QT_QTCORE_LIBRARY} ${DUI_LIBRARIES})

# Qt support
include(${QT_USE_FILE})

# Set executable
add_executable(applauncherd.bin ${SRC})

# Install rules
install(PROGRAMS applauncherd.bin scripts/applauncherd DESTINATION /usr/bin/)

