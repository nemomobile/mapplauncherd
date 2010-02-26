project(HelloWorld)

# Set sources
set(HelloWorldSRC helloworld.cpp)
# Set moc headers

# Find dui
find_library(dui_LIBRARY NAMES dui PATHS /usr/lib /usr/local/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} /usr/include/dui)

link_libraries(${dui_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})

include(${QT_USE_FILE})
add_executable(helloworld.launch ${HelloWorldSRC} )

# Install
install(PROGRAMS helloworld.launch DESTINATION /usr/bin/)
install(FILES scripts/helloworld DESTINATION /usr/bin/)


