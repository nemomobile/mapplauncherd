
set(SRC ${CMAKE_HOME_DIRECTORY}/src/launcher)

# Set sources
set(ut_boosterSRC
ut_booster.cpp
${SRC}/booster.cpp
${SRC}/connection.cpp
${SRC}/logger.cpp
)

# Set moc headers
set(MOC_HDRS
ut_booster.h
)

# Run moc
qt4_wrap_cpp( MOC_SRC ${MOC_HDRS} )

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

add_executable ( ut_booster ${ut_boosterSRC} ${MOC_SRC} )

# Install
install(PROGRAMS ut_booster DESTINATION /usr/share/applauncherd-tests/)

