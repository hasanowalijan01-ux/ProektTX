cmake_minimum_required(VERSION 3.16)
project(pd_crypto_suite LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

option(BUILD_CLIENT "Build Qt GUI client" ON)
option(BUILD_SERVER "Build TCP server" ON)
option(BUILD_TESTS  "Build unit tests"   ON)

# Core lib (no Qt) - pure algorithms
add_subdirectory(core)

if (BUILD_SERVER)
    find_package(Qt6 COMPONENTS Core Network Sql REQUIRED)
    add_subdirectory(server)
endif()

if (BUILD_CLIENT)
    find_package(Qt6 COMPONENTS Core Widgets Network Gui REQUIRED)
    add_subdirectory(client)
endif()

if (BUILD_TESTS)
    enable_testing()
    find_package(Qt6 COMPONENTS Test REQUIRED)
    add_subdirectory(tests)
endif()
