project(ut_daemon)

set(SRC ${CMAKE_HOME_DIRECTORY}/src/launcher)

# Set sources
set(ut_daemonSRC
ut_daemon.cpp
${SRC}/daemon.cpp
)

# Set moc headers
set(ut_daemon_MOC_HDRS
ut_daemon.h
${SRC}/daemon.h
)

qt4_wrap_cpp( ut_daemon_MOC_SRC ${ut_daemon_MOC_HDRS} )

set(QT_USE_QTTEST TRUE)

include_directories ( 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${SRC}
)

link_libraries (
   ${QT_QTCORE_LIBRARY}
   ${QT_QTTEST_LIBRARY}
)

include( ${QT_USE_FILE} )
add_executable ( ut_daemon ${ut_daemonSRC} ${ut_daemon_MOC_SRC} )

# Install
install(PROGRAMS ut_daemon DESTINATION /usr/share/applauncherd-tests/)

