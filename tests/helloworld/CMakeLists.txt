project(HelloWorld)

# Set sources
set(HelloWorldSRC helloworld.cpp)

link_libraries(${dui_LIBRARY})

include(${QT_USE_FILE})
add_library(helloworld.launch MODULE ${HelloWorldSRC})

# Install
install(PROGRAMS libhelloworld.launch.so DESTINATION /usr/bin RENAME helloworld.launch)
install(PROGRAMS scripts/helloworld DESTINATION /usr/bin/)
install(PROGRAMS scripts/qhelloworld DESTINATION /usr/bin/)

