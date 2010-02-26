project(PerfTests)

# Sub build: helloworld
add_subdirectory(helloworld)

# Set sources
set(WaitForWindowSRC
waitforwindow.c
)

find_library(X11_LIBRARY
  NAMES X11
  PATHS /usr/lib /usr/local/lib
  )

# Set moc headers
#set(WaitForWindow_MOC_HDRS
#)

# And finally this will run moc:
#qt4_wrap_cpp( WaitForWindow_MOC_SRC ${WaitForWindow_MOC_HDRS} )

include_directories ( 
  ${CMAKE_CURRENT_SOURCE_DIR}
  /usr/include/X11
)

link_libraries (
   ${m_LIBRARY}
   ${X11_LIBRARY}
)

add_executable ( waitforwindow ${WaitForWindowSRC} )

# Install
install(PROGRAMS waitforwindow DESTINATION /usr/bin/)
file(MAKE_DIRECTORY /usr/share/applauncherd-performance-tests)
install(FILES test-perf-launcher.py DESTINATION /usr/share/applauncherd-performance-tests/)
install(FILES tests.xml DESTINATION /usr/share/applauncherd-performance-tests/)
install(PROGRAMS helloworld/helloworld.launch DESTINATION /usr/bin)
install(FILES helloworld/scripts/helloworld DESTINATION /usr/bin)


