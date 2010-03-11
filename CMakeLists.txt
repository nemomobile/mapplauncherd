project(AppLauncherD)

set(CMAKE_VERBOSE_MAKEFILE OFF)

# Set default C++ flags
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -g -Wno-long-long -O2 -Wextra -Wpointer-arith -Wwrite-strings -Wold-style-cast -Woverloaded-virtual -Wundef")
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

# Sub build: applauncherd
add_subdirectory(src)

# Build tests if BUILD_TESTS environment variable is set
if ($ENV{BUILD_TESTS})
	# Sub build: tests
	# This will build the HelloWorld test library, unit tests, functional tests
	# and performance tests
	add_subdirectory(tests)
endif ($ENV{BUILD_TESTS})

# Target for documentation (make doc)
find_program(Doxygen NAMES doxygen)
if (EXISTS ${Doxygen})
	add_custom_target(doc COMMAND ${Doxygen} doc/Doxyfile)
else (EXISTS ${Doxygen})
	message(STATUS "Doxygen not found: you're not able to build documentation.")
endif (EXISTS ${Doxygen})


