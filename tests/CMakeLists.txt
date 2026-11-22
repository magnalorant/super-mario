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

function(mario_add_test_suite name label)
    add_executable(${name} ${ARGN})
    target_link_libraries(${name} PRIVATE mario::core GTest::gtest_main)
    target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_compile_definitions(${name} PRIVATE MARIO_ASSET_DIR="${PROJECT_SOURCE_DIR}/assets")
    if(NOT MSVC)
        target_compile_options(${name} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
    endif()
    gtest_discover_tests(${name}
        DISCOVERY_MODE PRE_TEST
        PROPERTIES LABELS ${label}
    )
endfunction()

mario_add_test_suite(mario_unit_tests unit
    unit/CameraTest.cpp
    unit/EditorModelTest.cpp
    unit/EventDispatcherTest.cpp
    unit/GeometryTest.cpp
    unit/LevelLoaderTest.cpp
    unit/PlatformTest.cpp
    unit/PlayerTest.cpp
    unit/TileMapTest.cpp
    unit/WorldInteractionTest.cpp
    unit/WorldPhysicsTest.cpp
    unit/WorldViewTest.cpp
)

