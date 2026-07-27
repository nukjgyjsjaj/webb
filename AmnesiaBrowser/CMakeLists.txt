cmake_minimum_required(VERSION 3.16)
project(amnesia_browser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)
find_package(CURL REQUIRED)

# =============================================================================
# Library: amnesia_session
# =============================================================================
add_library(amnesia_session STATIC
    src/session/session_manager.cc
    src/session/csprng.cc
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
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_fingerprint
# =============================================================================
add_library(amnesia_fingerprint STATIC
    src/fingerprint/fingerprint_generator.cc
    src/fingerprint/fingerprint.cc
    src/fingerprint/device_profiles.cc
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
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_storage
# =============================================================================
add_library(amnesia_storage STATIC
    src/storage/ephemeral_storage.cc
    src/storage/encrypted_dir.cc
)
set_target_properties(amnesia_storage PROPERTIES
    OUTPUT_NAME "amnesia_storage"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_storage PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_storage PUBLIC
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_cleanup
# =============================================================================
add_library(amnesia_cleanup STATIC
    src/cleanup/cleanup_system.cc
    src/cleanup/crash_handler.cc
    src/cleanup/memory_zeroizer.cc
)
set_target_properties(amnesia_cleanup PROPERTIES
    OUTPUT_NAME "amnesia_cleanup"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_cleanup PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_cleanup PUBLIC
    amnesia_storage
    amnesia_session
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_tls
# =============================================================================
add_library(amnesia_tls STATIC
    src/tls/browser_profiles.cc
    src/tls/tls_connection.cc
    src/tls/tls_manager.cc
)
set_target_properties(amnesia_tls PROPERTIES
    OUTPUT_NAME "amnesia_tls"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_tls PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_tls PUBLIC
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_vless
# =============================================================================
add_library(amnesia_vless STATIC
    src/vless/vless_manager.cc
    src/vless/vless_link_parser.cc
    src/vless/vless_collector.cc
    src/vless/vless_link_tester.cc
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
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_xray
# =============================================================================
add_library(amnesia_xray STATIC
    src/xray_integration/xray_client.cc
    src/xray_integration/socks5_proxy.cc
    src/xray_integration/proxy_router.cc
)
set_target_properties(amnesia_xray PROPERTIES
    OUTPUT_NAME "amnesia_xray"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_xray PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_xray PUBLIC
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_geoip
# =============================================================================
add_library(amnesia_geoip STATIC
    src/geoip/geoip_database.cc
    src/geoip/geoip_mmdb.cc
    src/geoip/mmdb_reader.cc
)
set_target_properties(amnesia_geoip PROPERTIES
    OUTPUT_NAME "amnesia_geoip"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_geoip PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_geoip PUBLIC
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_network
# =============================================================================
add_library(amnesia_network STATIC
    src/network/webrtc_leak_prevention.cc
)
set_target_properties(amnesia_network PROPERTIES
    OUTPUT_NAME "amnesia_network"
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)
target_include_directories(amnesia_network PUBLIC
    ${CMAKE_SOURCE_DIR}/AmnesiaBrowser/src
)
target_link_libraries(amnesia_network PUBLIC
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Library: amnesia_consistency
# =============================================================================
add_library(amnesia_consistency STATIC
    src/test/consistency/consistency_engine.cc
    src/test/consistency/rule_engine.cc
    src/test/consistency/consistency_validator.cc
    src/test/consistency/creepjs_simulator.cc
    src/test/consistency/device_profiles_db.cc
    src/test/consistency/browser_profiles_db.cc
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
    CURL::libcurl
)

# =============================================================================
# Main Browser Executable
# =============================================================================
add_executable(amnesia_browser
    src/main.cc
    src/amnesia_init.cc
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
    amnesia_network
    amnesia_consistency
    Threads::Threads
    CURL::libcurl
)

# =============================================================================
# Install Targets
# =============================================================================
install(TARGETS amnesia_browser
    RUNTIME DESTINATION bin
)
install(TARGETS amnesia_session amnesia_fingerprint amnesia_storage
                   amnesia_cleanup amnesia_tls amnesia_vless amnesia_xray
                   amnesia_geoip amnesia_network amnesia_consistency
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
