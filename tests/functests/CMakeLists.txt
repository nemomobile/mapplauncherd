project(FuncTests)
# Sub build: helloworld
add_subdirectory(helloworld)

install(FILES test-func-launcher.py DESTINATION /usr/share/applauncherd-functional-tests)
install(FILES tests.xml DESTINATION /usr/share/applauncherd-functional-tests)
install(PROGRAMS helloworld/helloworld.launch DESTINATION /usr/bin)
install(FILES helloworld/scripts/helloworld DESTINATION /usr/bin)
