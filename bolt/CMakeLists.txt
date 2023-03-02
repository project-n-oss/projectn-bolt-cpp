cmake_minimum_required(VERSION 3.21)

# where to find our CMake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(OWN_VERSION 1.0.0)
if(DEFINED CMAKE_PROJECT_VERSION AND NOT "${CMAKE_PROJECT_VERSION}" STREQUAL "")
    set(OWN_VERSION ${CMAKE_PROJECT_VERSION})
else
    message( FATAL_ERROR "Cannot find cmake project version" )
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
        src/bolt_s3_config.cpp
        src/bolt_s3_signer.cpp
        src/bolt_s3_curl_client.cpp
        src/bolt_s3_http_client_factory.cpp
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
    include/bolt_s3_config.h
    include/bolt_s3_client.h
    include/bolt_s3_client_model.h
    include/bolt_s3_http_client_factory.h
    include/bolt_s3_curl_client.h
)

#Set the location of where Windows can find the installed libraries of the SDK.
if(MSVC)
    string(REPLACE ";" "/aws-cpp-sdk-all;" SYSTEM_MODULE_PATH "${CMAKE_SYSTEM_PREFIX_PATH}/aws-cpp-sdk-all")
    list(APPEND CMAKE_PREFIX_PATH ${SYSTEM_MODULE_PATH})
endif()
find_package(AWSSDK REQUIRED COMPONENTS s3)
target_link_libraries(${PROJECT_NAME} PRIVATE ${AWSSDK_LINK_LIBRARIES})

include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG a2d35a1cb9f3f7e2f1469d6a189751331dc99f96) 
FetchContent_MakeAvailable(cpr)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr::cpr)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

set(EXTRA_TARGETS cpr nlohmann_json)


# not for MSVC
if(CMAKE_COMPILER_IS_GNUCXX)
    # compile options for this target only
    # treat warnings as errors
    target_compile_options(${PROJECT_NAME} PRIVATE -Werror)
endif()

include(ResolvePlatform)

# Http client control
if(NOT NO_HTTP_CLIENT)
    message(STATUS ${PLATFORM_LINUX} ${PLATFORM_APPLE} "platform")
    if(PLATFORM_WINDOWS)
        if(FORCE_CURL)
            set(ENABLE_CURL_CLIENT 1)
        else()
            set(ENABLE_WINDOWS_CLIENT 1)
        endif()
    elseif(PLATFORM_LINUX OR PLATFORM_APPLE OR PLATFORM_ANDROID)
        set(ENABLE_CURL_CLIENT 1)
    endif()

    if(ENABLE_CURL_CLIENT)
        add_definitions(-DENABLE_CURL_CLIENT)
        message(STATUS "Http client: Curl")
      
        include(FindCURL)
        if(NOT CURL_FOUND)
            message(FATAL_ERROR "Could not find curl")
        endif()
    elseif(ENABLE_WINDOWS_CLIENT)
        add_definitions(-DENABLE_WINDOWS_CLIENT)
        if(USE_IXML_HTTP_REQUEST_2)
            add_definitions(-DENABLE_WINDOWS_IXML_HTTP_REQUEST_2_CLIENT)
            message(STATUS "Http client: IXmlHttpRequest2")
            if(BYPASS_DEFAULT_PROXY)
                add_definitions(-DBYPASS_DEFAULT_PROXY)
                message(STATUS "Proxy bypass is enabled via WinHttp")
            endif()
        else()
            message(STATUS "Http client: WinHttp")
        endif()
    else()
        message(FATAL_ERROR "No http client available for target platform and client injection not enabled (-DNO_HTTP_CLIENT=ON)")
    endif()
else()
    message(STATUS "You will need to inject an http client implementation before making any http requests!")
endif()

include(InstallingGeneral)
include(InstallingConfigs)
