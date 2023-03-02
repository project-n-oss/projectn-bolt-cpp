if(DEFINED ENV{VCPKG_DEFAULT_TRIPLET} AND NOT DEFINED VCPKG_TARGET_TRIPLET)
  set(VCPKG_TARGET_TRIPLET "$ENV{VCPKG_DEFAULT_TRIPLET}" CACHE STRING "The vcpkg triplet")
endif()

cmake_minimum_required(VERSION 3.21)

# Same as aws-sdk-cpp
set(OWN_VERSION 1.11.0)

set(namespace "projectn")
project(
    "projectn"
    VERSION ${OWN_VERSION}
    DESCRIPTION "projectn sdks for c++"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
message(STATUS "CMAKE_MODULE_PATYH: ${CMAKE_MODULE_PATH}")

add_subdirectory(bolt)
add_subdirectory(example)

include(Packing)