set(SRC ${CMAKE_HOME_DIRECTORY}/src/launcher)

# Set sources
set(ut_daemonSRC
ut_daemon.cpp
${SRC}/booster.cpp
${SRC}/daemon.cpp
${SRC}/connection.cpp
${SRC}/logger.cpp
${SRC}/mbooster.cpp
${SRC}/qtbooster.cpp
)

# Set moc headers
set(ut_daemon_MOC_HDRS
ut_daemon.h
#${SRC}/daemon.h
)

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Run moc
qt4_wrap_cpp( ut_daemon_MOC_SRC ${ut_daemon_MOC_HDRS} )

# Enable test library
set(QT_USE_QTTEST TRUE)

# Set include paths
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} ${SRC} )

link_libraries (
   ${QT_QTCORE_LIBRARY}
   ${QT_QTTEST_LIBRARY}
   ${MEEGOTOUCH_LIBRARIES}
)

# Enable Qt (may not be needed, because already defined on higher level)
include( ${QT_USE_FILE} )

add_executable ( ut_daemon ${ut_daemonSRC} ${ut_daemon_MOC_SRC} )

# Install
install(PROGRAMS ut_daemon DESTINATION /usr/share/applauncherd-tests/)

