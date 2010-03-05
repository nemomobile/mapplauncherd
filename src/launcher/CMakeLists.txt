# Set sources
set(LauncherSRC booster.cpp connection.cpp daemon.cpp duibooster.cpp logger.cpp main.cpp qtbooster.cpp)

# Set include dirs
set(DUISRCINCLUDE /usr/include/dui)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${DUISRCINCLUDE})

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Check if it's possible to compile DuiBooster
if(EXISTS ${DUISRCINCLUDE}/duicomponentcache.h)
	message("Building with DUI Booster")
	find_library(dui_LIBRARY NAMES dui PATHS /usr/lib /usr/local/lib )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DHAVE_DUI")
else(EXISTS ${DUISRCINCLUDE}/duicomponentcache.h)
	message("!! DUI Booster not available !!")
endif(EXISTS ${DUISRCINCLUDE}/duicomponentcache.h)

# Set libraries to be linked (dui_LIBRARY may be empty)
link_libraries(${QT_QTCORE_LIBRARY} ${dui_LIBRARY})

# Qt support
include(${QT_USE_FILE})

# Set executable
add_executable(applauncherd ${LauncherSRC})

# Install
install(PROGRAMS applauncherd scripts/launcher DESTINATION /usr/bin/)


