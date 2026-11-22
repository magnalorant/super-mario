cmake_minimum_required(VERSION 3.20)

project(SuperMario
    VERSION 1.0.0
    DESCRIPTION "Side-scrolling platformer with a level editor, written in C++17 on top of SDL2"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

option(MARIO_BUILD_GAME "Build the SDL2 game executable" ON)
option(MARIO_BUILD_TESTS "Build the automated test suite" ON)
option(MARIO_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
option(MARIO_ENABLE_SANITIZERS "Instrument with AddressSanitizer and UndefinedBehaviorSanitizer" OFF)
option(MARIO_ENABLE_COVERAGE "Instrument for gcov/lcov code coverage" OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(ProjectWarnings)
include(Sanitizers)

add_subdirectory(src)

if(MARIO_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
