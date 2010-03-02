
set(SRC ${CMAKE_HOME_DIRECTORY}/src/launcher)

# Set sources
set(ut_duiboosterSRC
ut_duibooster.cpp
${SRC}/booster.cpp
${SRC}/connection.cpp
${SRC}/logger.cpp
${SRC}/duibooster.cpp
)

# Set moc headers
set(MOC_HDRS
ut_duibooster.h
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

add_executable ( ut_duibooster ${ut_duiboosterSRC} ${MOC_SRC} )

# Install
install(PROGRAMS ut_duibooster DESTINATION /usr/share/applauncherd-tests/)

