find_package(SDL2 2.0.18 REQUIRED)
find_package(SDL2_image QUIET)
find_package(SDL2_ttf QUIET)

if(NOT TARGET SDL2_image::SDL2_image OR NOT TARGET SDL2_ttf::SDL2_ttf)
    find_package(PkgConfig REQUIRED)
    if(NOT TARGET SDL2_image::SDL2_image)
        pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET SDL2_image)
        add_library(SDL2_image::SDL2_image ALIAS PkgConfig::SDL2_IMAGE)
    endif()
    if(NOT TARGET SDL2_ttf::SDL2_ttf)
        pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET SDL2_ttf)
        add_library(SDL2_ttf::SDL2_ttf ALIAS PkgConfig::SDL2_TTF)
    endif()
endif()

add_executable(super_mario)

target_sources(super_mario
    PRIVATE
        main.cpp
        Application.cpp
        SdlHandles.cpp
        SdlInput.cpp
        SdlRenderer.cpp
)

target_link_libraries(super_mario
    PRIVATE
        mario::core
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_ttf::SDL2_ttf
)

if(TARGET SDL2::SDL2main)
    target_link_libraries(super_mario PRIVATE SDL2::SDL2main)
endif()

mario_set_project_warnings(super_mario)

file(GLOB_RECURSE MARIO_ASSET_FILES CONFIGURE_DEPENDS
    RELATIVE ${PROJECT_SOURCE_DIR}/assets
    ${PROJECT_SOURCE_DIR}/assets/*
)
set(MARIO_STAGED_ASSETS "")
foreach(asset IN LISTS MARIO_ASSET_FILES)
    set(staged "${CMAKE_CURRENT_BINARY_DIR}/assets/${asset}")
    add_custom_command(
        OUTPUT ${staged}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/assets/${asset} ${staged}
        DEPENDS ${PROJECT_SOURCE_DIR}/assets/${asset}
        COMMENT "Staging asset ${asset}"
        VERBATIM
    )
    list(APPEND MARIO_STAGED_ASSETS ${staged})
endforeach()
add_custom_target(mario_assets DEPENDS ${MARIO_STAGED_ASSETS})
add_dependencies(super_mario mario_assets)

include(GNUInstallDirs)
install(TARGETS super_mario RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/assets DESTINATION ${CMAKE_INSTALL_BINDIR})

if(MARIO_BUILD_TESTS)
    add_test(NAME smoke.help COMMAND super_mario --help)
    add_test(NAME smoke.bad_option COMMAND super_mario --no-such-option)
    set_tests_properties(smoke.bad_option PROPERTIES WILL_FAIL TRUE)
    add_test(NAME smoke.run_frames COMMAND super_mario --frames 120 --verbose)
    set_tests_properties(smoke.help smoke.bad_option smoke.run_frames PROPERTIES LABELS smoke)
    set_tests_properties(smoke.run_frames PROPERTIES ENVIRONMENT "SDL_VIDEODRIVER=dummy;SDL_AUDIODRIVER=dummy")
endif()
