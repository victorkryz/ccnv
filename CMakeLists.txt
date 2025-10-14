cmake_minimum_required(VERSION 3.14)

set(app ccnv)
project(${app})

set(CMAKE_CXX_STANDARD 17)

set(CURL_INCLUDE_DIRS "")
set(CURL_LIBRARIES "")

file(GLOB source_files RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp src/*.h src/*.hpp src/*.c)

include(FetchContent)

# Fetch nlohmann_json
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)


if(WIN32)

    FetchContent_Declare(
        curl
        URL https://curl.se/windows/latest.cgi?p=win64-mingw.zip
        DOWNLOAD_EXTRACT_TIMESTAMP ON
    )

    FetchContent_MakeAvailable(curl)

    set(CURL_INCLUDE_DIRS "${FETCHCONTENT_BASE_DIR}/curl-src/include")
    set(CURL_LIBRARIES "${FETCHCONTENT_BASE_DIR}/curl-src/lib/libcurl.dll.a")

else()

    set(CURL_LIBRARIES curl)

endif()

add_executable(${app} ${source_files})

target_include_directories(${app}   PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/external 
                                    PRIVATE ${CURL_INCLUDE_DIRS})
target_link_libraries(${app} ${CURL_LIBRARIES} 
                              nlohmann_json::nlohmann_json)

if(WIN32)

    add_custom_command(TARGET ${app} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${FETCHCONTENT_BASE_DIR}/curl-src/bin/libcurl-x64.dll ${CMAKE_BINARY_DIR}
        COMMENT "Injecting curl dll into resulting build directory")


endif()

add_subdirectory(g.tests)

target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src) 