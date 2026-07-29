cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(amnesia_engine LANGUAGES CXX)

add_library(amnesia_engine SHARED
    ../app/src/main/cpp/engine/browser_engine.cpp
    ../app/src/main/cpp/anti_fp/anti_fingerprint.cpp
    ../app/src/main/cpp/tls/tls_spoof.cpp
    ../app/src/main/cpp/proxy/vless_proxy.cpp
    ../app/src/main/cpp/session/session_manager.cpp
    ../app/src/main/cpp/cleanup/cleanup.cpp
)

target_include_directories(amnesia_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/../app/src/main/cpp
)

find_library(log-lib log)
find_library(android-lib android)

target_link_libraries(amnesia_engine
    ${log-lib}
    ${android-lib}
)
