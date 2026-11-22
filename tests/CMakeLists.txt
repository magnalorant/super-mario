find_package(GTest 1.11 CONFIG QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

add_executable(mario_tests)

target_sources(mario_tests
    PRIVATE
        unit/EventDispatcherTest.cpp
        unit/GeometryTest.cpp
        unit/LevelLoaderTest.cpp
        unit/PlatformTest.cpp
        unit/PlayerTest.cpp
        unit/TileMapTest.cpp
)

target_link_libraries(mario_tests
    PRIVATE
        mario::core
        GTest::gtest_main
)

if(NOT MSVC)
    target_compile_options(mario_tests PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
endif()

gtest_discover_tests(mario_tests
    DISCOVERY_MODE PRE_TEST
    PROPERTIES LABELS unit
)
