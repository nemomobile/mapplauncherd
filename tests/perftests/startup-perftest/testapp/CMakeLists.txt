set(CMAKE_VERBOSE_MAKEFILE ON)

# Set sources
set(SRC main.cpp)
link_libraries (${MEEGOTOUCH_LIBRARIES})

set(CMAKE_CXX_FLAGS ${CMAKE_CXXFLAGS} "-fPIC -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_EXE_LINKER_FLAGS "-pie -rdynamic") 

# Enable Qt-support
include(${QT_USE_FILE})
add_executable(testapp ${SRC})

# Install
install(PROGRAMS testapp DESTINATION /usr/bin/)

