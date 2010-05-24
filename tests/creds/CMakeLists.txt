set(SRC creds_test.cpp)

link_libraries(${MEEGOTOUCH_LIBRARIES})

include(${QT_USE_FILE})
add_library(creds_test MODULE ${SRC})

install(PROGRAMS libcreds_test.so DESTINATION /usr/bin RENAME creds_test_max.launch)
install(PROGRAMS creds_test DESTINATION /usr/bin RENAME creds_test_max)

install(PROGRAMS libcreds_test.so DESTINATION /usr/bin RENAME creds_test_normal.launch)
install(PROGRAMS creds_test DESTINATION /usr/bin RENAME creds_test_normal)


