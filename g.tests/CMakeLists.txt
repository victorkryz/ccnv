set(CMAKE_VERBOSE_MAKEFILE on)

set(CMAKE_CXX_STANDARD 17)

aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src" tests_src_files)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/f8d7d77c06936315286eb55f8de22cd23c188571.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(tests ${tests_src_files} ${h_files})

target_link_libraries(tests  ${CLIENT_SERVER_LIB}
                             ${EXTRA_LIBS}
                             GTest::gtest_main
									           GTest::gmock_main)
add_test(ccnv_tests tests)
            

