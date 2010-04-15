# Set sources
set(SRC main.cpp)
link_libraries (${MEEGOTOUCH_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})

# Enable Qt-support
include(${QT_USE_FILE})

add_executable(hello_no_launcher ${SRC})

# Install
install(PROGRAMS hello_no_launcher DESTINATION /usr/bin/)
