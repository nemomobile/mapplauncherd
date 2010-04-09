project(Tests)

# Find dui for the test programs
include(FindPkgConfig) 
pkg_check_modules(DUI Dui>=0.19 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${DUI_INCLUDE_DIRS})

# Sub build: helloworld
add_subdirectory(helloworld)

# Sub build: unit tests
add_subdirectory(unittests)

# Sub build: perftests
add_subdirectory(perftests)

# Sub build: functests
add_subdirectory(functests)



