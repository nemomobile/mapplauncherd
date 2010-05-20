# Qt support
include(${QT_USE_FILE})

# Find Meego Touch libraries using pkg-config. 
include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_HOME_DIRECTORY}/src/common ${MEEGOTOUCH_INCLUDE_DIRS})

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

if ($ENV{DISABLE_VERIFICATION})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDISABLE_VERIFICATION")
endif ($ENV{DISABLE_VERIFICATION})

# Set sources
set(SRC appdata.cpp booster.cpp connection.cpp daemon.cpp mbooster.cpp logger.cpp main.cpp qtbooster.cpp)

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Find creds.h (and libcreds if the header is found)
message(STATUS "checking for creds.h")
find_file(CREDS_H NAMES creds.h PATHS /usr/include/sys)
if (EXISTS ${CREDS_H})
    message(STATUS "  found: " ${CREDS_H})
    message(STATUS "checking for libcreds")
    find_library(LIBCREDS NAMES creds)
    if (EXISTS ${LIBCREDS})
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_CREDS")
        link_libraries(${LIBCREDS})
	    message(STATUS "  found: " ${LIBCREDS})
    else (EXISTS ${LIBCREDS})
    	message(STATUS "  not found: Platform Security is disabled.")
    endif (EXISTS ${LIBCREDS})
else (EXISTS ${CREDS_H})
  	message(STATUS "  not found: Platform Security is disabled.")
endif (EXISTS ${CREDS_H})

# Find libdl
find_library(LIBDL NAMES dl)

link_libraries(${QT_LIBRARIES} ${MEEGOTOUCH_LIBRARIES} ${LIBDL})

# Set executable
add_executable(applauncherd.bin ${SRC})

# Install rules
install(PROGRAMS applauncherd.bin scripts/applauncherd DESTINATION /usr/bin/)

