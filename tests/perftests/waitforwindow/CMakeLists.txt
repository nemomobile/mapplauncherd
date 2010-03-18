# Set sources
set(WaitForWindowSRC waitforwindow.c)

find_library(X11_LIBRARY NAMES X11 PATHS /usr/lib /usr/local/lib)

if(NOT X11_LIBRARY)
	message(FATAL_ERROR "Unable to find X11 library! Cannot build waitforwindow.")
endif(NOT X11_LIBRARY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} /usr/include/X11)

link_libraries(${X11_LIBRARY})
add_executable (wait_window ${WaitForWindowSRC})

# Install
install(PROGRAMS wait_window DESTINATION /usr/bin)


