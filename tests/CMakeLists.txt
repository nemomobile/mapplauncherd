# Find libmeegotouch for the test programs
include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

# Sub build: helloworld
add_subdirectory(helloworld)

# Sub build: unit tests
add_subdirectory(unittests)

# Sub build: perftests
add_subdirectory(perftests)

# Sub build: functests
add_subdirectory(functests)



