# Top-level CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(AmnesiaBrowser LANGUAGES C CXX)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type: Debug or Release" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -flto=auto")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -D_DEBUG -fno-omit-frame-pointer")

# Static linking for core browser libs to reduce external dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_LINK_WHAT_YOU_USE ON)

# Include subdirectories
add_subdirectory(AmnesiaBrowser)

# =============================================================================
# Top-level convenience targets
# =============================================================================
add_custom_target(amnesia-ci
    DEPENDS amnesia_browser
            amnesia_session_unittests
            amnesia_fingerprint_unittests
            amnesia_consistency_unittests
            amnesia_vless_unittests
            amnesia_storage_unittests
            amnesia_cleanup_unittests
            amnesia_browser_integration_tests
            consistency_validator
    COMMENT "Running full Amnesia Browser CI pipeline"
)

add_custom_target(amnesia-quick-test
    DEPENDS amnesia_session_unittests
            amnesia_fingerprint_unittests
            consistency_validator
    COMMENT "Running quick smoke tests"
)

# Print build summary
add_custom_target(amnesia-summary ALL
    COMMAND ${CMAKE_COMMAND} -E echo "========================================"
    COMMAND ${CMAKE_COMMAND} -E echo "  Amnesia Browser Build Summary"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================"
    COMMAND ${CMAKE_COMMAND} -E echo "  Build type:      ${CMAKE_BUILD_TYPE}"
    COMMAND ${CMAKE_COMMAND} -E echo "  C++ standard:    ${CMAKE_CXX_STANDARD}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Install prefix:  ${CMAKE_INSTALL_PREFIX}"
    COMMAND ${CMAKE_COMMAND} -E echo "  Binary output:   ${CMAKE_BINARY_DIR}/bin"
    COMMAND ${CMAKE_COMMAND} -E echo "  Library output:  ${CMAKE_BINARY_DIR}/lib"
    COMMAND ${CMAKE_COMMAND} -E echo "========================================"
)
