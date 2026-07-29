cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ANDROID_ARCH_ABI arm64-v8a)

project(amnesia_engine LANGUAGES CXX)

add_library(amnesia_engine SHARED
    cpp/engine/browser_engine.cpp
    cpp/anti_fp/anti_fingerprint.cpp
    cpp/tls/tls_spoof.cpp
    cpp/proxy/vless_proxy.cpp
    cpp/session/session_manager.cpp
    cpp/cleanup/cleanup.cpp
)

target_include_directories(amnesia_engine PRIVATE
    ${CMAKE_SOURCE_DIR}/amnesia-android/app/src/main/cpp
)

find_library(log-lib log)
find_library(android-lib android)
find_library(ssl-lib ssl)
find_library(crypto-lib crypto)

target_link_libraries(amnesia_engine
    ${log-lib}
    ${android-lib}
    ${ssl-lib}
    ${crypto-lib}
)
