# Set sources
set(SRC prestartapp.cpp)

link_libraries(${MEEGOTOUCH_LIBRARIES})

include(${QT_USE_FILE})
add_library(prestartapp MODULE ${SRC})

# Install
install(PROGRAMS libprestartapp.so DESTINATION /usr/bin RENAME prestartapp.launch)
install(FILES com.nokia.prestartapp.service DESTINATION /usr/share/dbus-1/services)

