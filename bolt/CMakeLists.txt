cmake_minimum_required(VERSION 3.21)

set(OWN_VERSION 1.0.1)
if(DEFINED CMAKE_PROJECT_VERSION AND NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
    set(OWN_VERSION ${CMAKE_PROJECT_VERSION})
endif()

project(
    "bolt"
    VERSION ${OWN_VERSION}
    DESCRIPTION "bolt sdk"
)

message(STATUS "This project version: ${PROJECT_VERSION}")
message(STATUS "This project major version: ${PROJECT_VERSION_MAJOR}")

if (NOT CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    message(STATUS "This project has a top-level one called [${CMAKE_PROJECT_NAME}]")
else()
    message(STATUS "This project is a top-level one")
endif()


if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    message(STATUS "C++ standard has been already set to: ${CMAKE_CXX_STANDARD}")
endif()

add_library(${PROJECT_NAME})

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        src/bolt_s3_client.cpp

)

target_include_directories(${PROJECT_NAME}
    PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
    PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/>
)

# without it public headers won't get installed
set(
    public_headers
    include/bolt_s3_client.h
)

#Set the location of where Windows can find the installed libraries of the SDK.
if(MSVC)
    string(REPLACE ";" "/aws-cpp-sdk-all;" SYSTEM_MODULE_PATH "${CMAKE_SYSTEM_PREFIX_PATH}/aws-cpp-sdk-all")
    list(APPEND CMAKE_PREFIX_PATH ${SYSTEM_MODULE_PATH})
endif()
find_package(AWSSDK REQUIRED COMPONENTS s3)
target_link_libraries(${PROJECT_NAME} PRIVATE ${AWSSDK_LINK_LIBRARIES})


# not for MSVC
if(CMAKE_COMPILER_IS_GNUCXX)
    # compile options for this target only
    # treat warnings as errors
    target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
endif()

# where to find our CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(InstallingGeneral)
include(InstallingConfigs)
