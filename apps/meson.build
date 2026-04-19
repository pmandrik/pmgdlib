# P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

accel_backend = 'GL'

test_inc = include_directories('../')
cpp = meson.get_compiler('cpp')

# cxxopts
cxxopts_inc = include_directories(cxxopts_include_path)

if gl.found()
    test_exec = executable('pmSceneRender', 
        'pmgdlib_scene_render.cpp', 
        include_directories: core_deps + [sdl2_inc, cxxopts_inc], 
        dependencies: [sdl2, gl],
        link_with : core_links
    )
endif