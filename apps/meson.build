# P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

accel_backend = 'GL'

test_inc = include_directories('../')
cpp = meson.get_compiler('cpp')

# sdl2 dependency
sdl2 = cpp.find_library('SDL2-2.0', dirs : sdl2_build_path)
sdl2_inc = include_directories(sdl2_include_path)

# gl dependency
gl = dependency('gl', main: true, required: true)

# cxxopts
cxxopts_inc = include_directories(cxxopts_include_path)

test_exec = executable('pmSceneRender', 
    'pmgdlib_scene_render.cpp', 
    include_directories: core_deps + [sdl2_inc, cxxopts_inc], 
    dependencies: [sdl2, gl],
    link_with : core_links
)
