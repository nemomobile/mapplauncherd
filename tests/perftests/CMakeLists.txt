# Sub build: waitforwindow
add_subdirectory(waitforwindow)
add_subdirectory(hello_no_launcher)
add_subdirectory(startup-perftest)

#file(MAKE_DIRECTORY /usr/share/applauncherd-performance-tests)
install(FILES test-perf-launcher.py DESTINATION /usr/share/applauncherd-performance-tests/)
install(FILES test-perf-mbooster.py DESTINATION /usr/share/applauncherd-performance-tests/)
install(FILES tests.xml DESTINATION /usr/share/applauncherd-performance-tests/)
