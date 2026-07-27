cmake_minimum_required(VERSION 3.20)
project(AmnesiaBrowser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Global compile definitions
add_definitions(-DNAMESIA_BUILD)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/third_party)

# Core libraries
add_subdirectory(src/session)
add_subdirectory(src/fingerprint)
add_subdirectory(src/storage)
add_subdirectory(src/cleanup)
add_subdirectory(src/tls)
add_subdirectory(src/vless)
add_subdirectory(src/xray_integration)
add_subdirectory(src/geoip)
add_subdirectory(src/test)

# UI (if needed)
add_subdirectory(src/ui)

# Resources
add_subdirectory(resources)

# Documentation
add_subdirectory(docs)
