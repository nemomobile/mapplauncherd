# Qt support
include(${QT_USE_FILE})

# Find Meego Touch libraries using pkg-config. 
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

# Set sources
set(SRC booster.cpp connection.cpp daemon.cpp mbooster.cpp logger.cpp main.cpp qtbooster.cpp)

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Set debug logging
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")

link_libraries(${QT_LIBRARIES} ${MEEGOTOUCH_LIBRARIES} creds)

# Set executable
add_executable(applauncherd.bin ${SRC})

# Install rules
install(PROGRAMS applauncherd.bin scripts/applauncherd DESTINATION /usr/bin/)

