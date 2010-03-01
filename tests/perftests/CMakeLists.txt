project(PerfTests)

# Sub build: waitforwindow
add_subdirectory(waitforwindow)

#file(MAKE_DIRECTORY /usr/share/applauncherd-performance-tests)
install(FILES test-perf-launcher.py DESTINATION /usr/share/applauncherd-performance-tests/)
install(FILES tests.xml DESTINATION /usr/share/applauncherd-performance-tests/)



