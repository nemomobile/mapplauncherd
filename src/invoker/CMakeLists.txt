set(InvokerSRC invokelib.c invoker.c report.c search.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W -Wall -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DPROG_NAME=\\\"invoker\\\" -DPACKAGE=\\\"applauncherd\\\" -DVERSION=\\\"0.1\\\"")

add_executable(invoker ${InvokerSRC})
install(PROGRAMS invoker DESTINATION /usr/bin/)


