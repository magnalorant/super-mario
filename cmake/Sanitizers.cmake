include_guard(GLOBAL)

function(mario_enable_instrumentation target)
    if(MSVC)
        return()
    endif()

    if(MARIO_ENABLE_SANITIZERS)
        target_compile_options(${target} PUBLIC -fsanitize=address,undefined -fno-omit-frame-pointer -fno-sanitize-recover=all)
        target_link_options(${target} PUBLIC -fsanitize=address,undefined)
    endif()

    if(MARIO_ENABLE_COVERAGE)
        target_compile_options(${target} PUBLIC --coverage -O0 -g)
        target_link_options(${target} PUBLIC --coverage)
    endif()
endfunction()
