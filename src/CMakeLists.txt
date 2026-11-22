add_library(mario_core STATIC)
add_library(mario::core ALIAS mario_core)

target_sources(mario_core
    PRIVATE
        core/Entities.cpp
        core/Platform.cpp
        core/TileMap.cpp
)

target_include_directories(mario_core
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_compile_features(mario_core PUBLIC cxx_std_17)
mario_set_project_warnings(mario_core)
mario_enable_instrumentation(mario_core)
