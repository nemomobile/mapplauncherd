
set(SRC ${CMAKE_HOME_DIRECTORY}/src/launcher)

# Set sources
set(ut_connectionSRC
ut_connection.cpp
${SRC}/booster.cpp
${SRC}/daemon.cpp
${SRC}/connection.cpp
${SRC}/logger.cpp
${SRC}/duibooster.cpp
${SRC}/qtbooster.cpp
)

# Set moc headers
set(ut_connection_MOC_HDRS
ut_connection.h
#${SRC}/connection.h
)

# Set the program name define used in daemon.cpp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROG_NAME=\\\"applauncherd\\\"")

# Run moc
qt4_wrap_cpp( ut_connection_MOC_SRC ${ut_connection_MOC_HDRS} )

# Enable test library
set(QT_USE_QTTEST TRUE)

# Set include paths
include_directories ( ${CMAKE_CURRENT_SOURCE_DIR} ${SRC} )

link_libraries (
   ${QT_QTCORE_LIBRARY}
   ${QT_QTTEST_LIBRARY}
)

# Enable Qt (may not be needed, because already defined on higher level)
include( ${QT_USE_FILE} )

add_executable ( ut_connection ${ut_connectionSRC} ${ut_connection_MOC_SRC} )

# Install
install(PROGRAMS ut_connection DESTINATION /usr/share/applauncherd-tests/)

