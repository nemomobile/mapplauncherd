set(CMAKE_VERBOSE_MAKEFILE ON)

# Set sources
set(SRC main.cpp)
link_libraries(${MEEGOTOUCH_LIBRARIES})

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

set(CMAKE_CXX_FLAGS ${CMAKE_CXXFLAGS} "-fPIC -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_EXE_LINKER_FLAGS "-pie -rdynamic") 

# Enable Qt-support
include(${QT_USE_FILE})
add_executable(testapp ${SRC})

# Install
install(PROGRAMS testapp DESTINATION /usr/bin/)

