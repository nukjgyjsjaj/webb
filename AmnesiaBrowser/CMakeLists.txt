cmake_minimum_required(VERSION 3.20)
project(AmnesiaBrowser C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -D_DEBUG")

# Security hardening
add_compile_options(
  $<$<CONFIG:Release>:-O3>
  $<$<CONFIG:Release>:-DNDEBUG>
  $<$<CONFIG:Debug>:-g>
  $<$<CONFIG:Debug>:-O0>
)

# Position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src)
include_directories(${CMAKE_SOURCE_DIR}/AmnesiaBrowser/third_party)
include_directories(${CMAKE_SOURCE_DIR}/AmnesiaBrowser/profiles)

# Find required packages
find_package(Threads REQUIRED)
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# =============================================================================
# Core Libraries (these are the actual browser core, not wrappers)
# =============================================================================

# Session Manager Library
add_library(amnesia_session STATIC
    AmnesiaBrowser/src/session/session_manager.cc
    AmnesiaBrowser/src/session/csprng.cc
)
set_target_properties(amnesia_session PROPERTIES
    OUTPUT_NAME "amnesia_session"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_session PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_session PUBLIC
    Threads::Threads
    OpenSSL::Crypto
    ZLIB::ZLIB
)

# Fingerprint Generator Library
add_library(amnesia_fingerprint STATIC
    AmnesiaBrowser/src/fingerprint/fingerprint_generator.cc
)
set_target_properties(amnesia_fingerprint PROPERTIES
    OUTPUT_NAME "amnesia_fingerprint"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_fingerprint PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_fingerprint PUBLIC
    amnesia_session
    Threads::Threads
)

# Ephemeral Storage Library
add_library(amnesia_storage STATIC
    AmnesiaBrowser/src/storage/ephemeral_storage.cc
    AmnesiaBrowser/src/storage/encrypted_dir.cc
)
set_target_properties(amnesia_storage PROPERTIES
    OUTPUT_NAME "amnesia_storage"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_storage PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_storage PUBLIC
    amnesia_session
    Threads::Threads
    OpenSSL::Crypto
)

# Cleanup System Library
add_library(amnesia_cleanup STATIC
    AmnesiaBrowser/src/cleanup/cleanup_system.cc
    AmnesiaBrowser/src/cleanup/crash_handler.cc
    AmnesiaBrowser/src/cleanup/memory_zeroizer.cc
)
set_target_properties(amnesia_cleanup PROPERTIES
    OUTPUT_NAME "amnesia_cleanup"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_cleanup PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_cleanup PUBLIC
    amnesia_session
    amnesia_storage
    Threads::Threads
)

# TLS Manager Library
add_library(amnesia_tls STATIC
    AmnesiaBrowser/src/tls/tls_manager.cc
    AmnesiaBrowser/src/tls/browser_profiles.cc
    AmnesiaBrowser/src/tls/tls_connection.cc
)
set_target_properties(amnesia_tls PROPERTIES
    OUTPUT_NAME "amnesia_tls"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_tls PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/third_party/utls/include
)
target_link_libraries(amnesia_tls PUBLIC
    amnesia_session
    amnesia_fingerprint
    OpenSSL::SSL
    OpenSSL::Crypto
)

# VLESS Manager Library
add_library(amnesia_vless STATIC
    AmnesiaBrowser/src/vless/vless_manager.cc
    AmnesiaBrowser/src/vless/vless_collector.cc
    AmnesiaBrowser/src/vless/vless_link_tester.cc
    AmnesiaBrowser/src/vless/vless_link_parser.cc
)
set_target_properties(amnesia_vless PROPERTIES
    OUTPUT_NAME "amnesia_vless"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_vless PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_vless PUBLIC
    amnesia_session
    CURL::libcurl
    OpenSSL::Crypto
    Threads::Threads
)

# Xray Integration Library
add_library(amnesia_xray STATIC
    AmnesiaBrowser/src/xray_integration/xray_client.cc
    AmnesiaBrowser/src/xray_integration/socks5_proxy.cc
    AmnesiaBrowser/src/xray_integration/proxy_router.cc
)
set_target_properties(amnesia_xray PROPERTIES
    OUTPUT_NAME "amnesia_xray"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_xray PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_xray PUBLIC
    amnesia_vless
    Threads::Threads
    OpenSSL::Crypto
)

# GeoIP Library
add_library(amnesia_geoip STATIC
    AmnesiaBrowser/src/geoip/geoip_database.cc
    AmnesiaBrowser/src/geoip/mmdb_reader.cc
)
set_target_properties(amnesia_geoip PROPERTIES
    OUTPUT_NAME "amnesia_geoip"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_geoip PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_geoip PUBLIC
    amnesia_session
    Threads::Threads
)

# Consistency Engine Library
add_library(amnesia_consistency STATIC
    AmnesiaBrowser/src/test/consistency/consistency_engine.cc
    AmnesiaBrowser/src/test/consistency/rule_engine.cc
    AmnesiaBrowser/src/test/consistency/consistency_validator.cc
    AmnesiaBrowser/src/test/consistency/creepjs_simulator.cc
    AmnesiaBrowser/src/test/consistency/device_profiles_db.cc
    AmnesiaBrowser/src/test/consistency/browser_profiles_db.cc
)
set_target_properties(amnesia_consistency PROPERTIES
    OUTPUT_NAME "amnesia_consistency"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_consistency PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_consistency PUBLIC
    amnesia_fingerprint
    amnesia_geoip
    Threads::Threads
)

# =============================================================================
# Main Browser Executable
# =============================================================================
add_executable(amnesia_browser
    AmnesiaBrowser/src/main.cc
    AmnesiaBrowser/src/amnesia_init.cc
)
set_target_properties(amnesia_browser PROPERTIES
    OUTPUT_NAME "amnesia_browser"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_browser PRIVATE
    amnesia_session
    amnesia_fingerprint
    amnesia_storage
    amnesia_cleanup
    amnesia_tls
    amnesia_vless
    amnesia_xray
    amnesia_geoip
    amnesia_consistency
    Threads::Threads
    OpenSSL::Crypto
    CURL::libcurl
)

# =============================================================================
# Unit Tests
# =============================================================================
enable_testing()

add_executable(amnesia_session_unittests
    AmnesiaBrowser/src/test/unit/session_manager_test.cc
    AmnesiaBrowser/src/test/unit/csprng_test.cc
)
set_target_properties(amnesia_session_unittests PROPERTIES
    OUTPUT_NAME "amnesia_session_unittests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_session_unittests PRIVATE
    amnesia_session
    gtest_main
    Threads::Threads
)

add_executable(amnesia_fingerprint_unittests
    AmnesiaBrowser/src/test/unit/fingerprint_generator_test.cc
)
set_target_properties(amnesia_fingerprint_unittests PROPERTIES
    OUTPUT_NAME "amnesia_fingerprint_unittests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_fingerprint_unittests PRIVATE
    amnesia_fingerprint
    amnesia_session
    gtest_main
    Threads::Threads
)

add_executable(amnesia_consistency_unittests
    AmnesiaBrowser/src/test/unit/consistency_engine_test.cc
)
set_target_properties(amnesia_consistency_unittests PROPERTIES
    OUTPUT_NAME "amnesia_consistency_unittests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_consistency_unittests PRIVATE
    amnesia_consistency
    amnesia_fingerprint
    gtest_main
    Threads::Threads
)

add_executable(amnesia_vless_unittests
    AmnesiaBrowser/src/test/unit/vless_manager_test.cc
)
set_target_properties(amnesia_vless_unittests PROPERTIES
    OUTPUT_NAME "amnesia_vless_unittests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_vless_unittests PRIVATE
    amnesia_vless
    amnesia_session
    gtest_main
    Threads::Threads
)

add_executable(amnesia_storage_unittests
    AmnesiaBrowser/src/test/unit/ephemeral_storage_test.cc
)
set_target_properties(amnesia_storage_unittests PROPERTIES
    OUTPUT_NAME "amnesia_storage_unittests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_storage_unittests PRIVATE
    amnesia_storage
    amnesia_cleanup
    gtest_main
    Threads::Threads
)

add_executable(amnesia_cleanup_unittests
    AmnesiaBrowser/src/test/unit/cleanup_system_test.cc
)
set_target_properties(amnesia_cleanup_unittests PROPERTIES
    OUTPUT_NAME "amnesia_cleanup_unittests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_cleanup_unittests PRIVATE
    amnesia_cleanup
    amnesia_storage
    gtest_main
    Threads::Threads
)

# =============================================================================
# Integration Tests
# =============================================================================
add_executable(amnesia_browser_integration_tests
    AmnesiaBrowser/src/test/integration/session_lifecycle_test.cc
    AmnesiaBrowser/src/test/integration/fingerprint_engine_integration_test.cc
    AmnesiaBrowser/src/test/integration/vless_proxy_test.cc
    AmnesiaBrowser/src/test/integration/tls_spoofing_test.cc
    AmnesiaBrowser/src/test/integration/cleanup_verification_test.cc
)
set_target_properties(amnesia_browser_integration_tests PROPERTIES
    OUTPUT_NAME "amnesia_browser_integration_tests"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(amnesia_browser_integration_tests PRIVATE
    amnesia_session
    amnesia_fingerprint
    amnesia_storage
    amnesia_cleanup
    amnesia_tls
    amnesia_vless
    amnesia_xray
    amnesia_consistency
    gtest_main
    Threads::Threads
    OpenSSL::Crypto
    CURL::libcurl
)

# =============================================================================
# Consistency Validator (standalone)
# =============================================================================
add_executable(consistency_validator
    AmnesiaBrowser/src/test/consistency/standalone_validator.cc
)
set_target_properties(consistency_validator PROPERTIES
    OUTPUT_NAME "consistency_validator"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
target_link_libraries(consistency_validator PRIVATE
    amnesia_consistency
    amnesia_fingerprint
    amnesia_geoip
    Threads::Threads
)

# =============================================================================
# Custom Targets for Convenience
# =============================================================================
add_custom_target(run_all_tests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_session_unittests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_fingerprint_unittests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_consistency_unittests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_vless_unittests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_storage_unittests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_cleanup_unittests
    COMMAND ${CMAKE_BINARY_DIR}/bin/amnesia_browser_integration_tests
    COMMENT "Running all Amnesia Browser tests"
)

add_custom_target(validate_consistency
    COMMAND ${CMAKE_BINARY_DIR}/bin/consistency_validator
    COMMENT "Running standalone consistency validator"
)

add_custom_target(build_and_test
    DEPENDS amnesia_browser run_all_tests validate_consistency
    COMMENT "Building and testing Amnesia Browser"
)

# =============================================================================
# Install Targets
# =============================================================================
install(TARGETS amnesia_browser
    RUNTIME DESTINATION bin
)
install(TARGETS amnesia_session amnesia_fingerprint amnesia_storage
                   amnesia_cleanup amnesia_tls amnesia_vless amnesia_xray
                   amnesia_geoip amnesia_consistency
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(DIRECTORY AmnesiaBrowser/src/ DESTINATION include/amnesia/browser/src
    FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY AmnesiaBrowser/resources/ DESTINATION share/amnesia/resources)
install(DIRECTORY AmnesiaBrowser/profiles/ DESTINATION share/amnesia/profiles)
