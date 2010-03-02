project(Tests)

# Find dui for the test programs
find_library(dui_LIBRARY NAMES dui PATHS /usr/lib /usr/local/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} /usr/include/dui)

if(NOT dui_LIBRARY)
	message(FATAL_ERROR "DUI library not found! Unable to compile functional tests.")
endif(NOT dui_LIBRARY)

# Sub build: helloworld
add_subdirectory(helloworld)

# Sub build: unit tests
add_subdirectory(unittests)

# Sub build: perftests
add_subdirectory(perftests)

# Sub build: functests
add_subdirectory(functests)



