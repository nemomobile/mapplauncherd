# Set sources
set(SRC helloworld.cpp)

link_libraries(${MEEGOTOUCH_LIBRARIES})

include(${QT_USE_FILE})
add_library(helloworld MODULE ${SRC})

# Install
install(PROGRAMS libhelloworld.so DESTINATION /usr/bin RENAME helloworld.launch)
install(PROGRAMS scripts/helloworld DESTINATION /usr/bin/)
install(PROGRAMS scripts/qhelloworld DESTINATION /usr/bin/)

# Install symlinks so that /usr/bin/helloworld_X launches /usr/bin/helloworld_X.launch
install(SCRIPT scripts/create_links.cmake)

