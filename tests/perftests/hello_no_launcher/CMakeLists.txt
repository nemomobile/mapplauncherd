# Set sources
set(HelloNoLaunchSRC main.cpp)

link_libraries (${dui_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})

# Enable Qt-support
include(${QT_USE_FILE})

add_executable(hello_no_launcher ${HelloNoLaunchSRC})

# Install
install(PROGRAMS hello_no_launcher DESTINATION /usr/bin/)
