# P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
#                   https://inspirehep.net/authors/1395721
#                   https://vk.com/pmandrik

project('pmgdlib', 'cpp', default_options : ['c_std=c20', 'cpp_std=c++20'])
cpp_args_glob=['-Wfatal-errors', '-Wno-unused-variable', '-Wno-sign-compare', '-Wno-unused-but-set-variable']
cpp_args=[]
cpp = meson.get_compiler('cpp')

test_inc = include_directories('./lib')
pmgdlib_sources = [
  './lib/pmgdlib_math.cpp',
  './lib/pmgdlib_string.cpp',
  './lib/pmgdlib_msg.cpp',
  './lib/pmgdlib_graph.cpp',
  './lib/pmgdlib_core_render.cpp',
  './lib/pmgdlib_config.cpp',
  './lib/pmgdlib_storage.cpp',
  './lib/pmgdlib_factory.cpp',
  './lib/pmgdlib_sdl.cpp'
]
core_incs = [test_inc]
core_deps = []
core_links = []

# deps setup
USE_TINYXML2 = true
USE_STB = true
USE_SDL_IMAGE = false

TEST_SDL = true
TEST_GL = false
TEST_VULKAN = false

# sdl2
sdl2_build_path = '/home/petr/work/gd/SDL2-2.30.3/build'
sdl2_include_path = ['../../SDL2-2.30.3/build/include/SDL2/.', '../../SDL2-2.30.3/build/include-config-release/SDL2/.']

sdl2 = cpp.find_library('SDL2-2.0', dirs : sdl2_build_path)
sdl2_inc = include_directories(sdl2_include_path)

core_incs += [sdl2_inc]
core_deps += [sdl2]

if USE_SDL_IMAGE
  cpp_args += '-DUSE_SDL_IMAGE'
endif

# gl
gl = dependency('gl', main: true, required: false)

# cxxopts
cxxopts_include_path = '../external/cxxopts-3.2.0/include/.'
add_project_arguments(cpp_args_glob, language: ['cpp'])

# tinyxml2
tinyxml2_include_path = '../external/tinyxml2-11.0.0/.'
if USE_TINYXML2
  tinyxml2_inc = include_directories(tinyxml2_include_path)
  core_incs += [tinyxml2_inc]

  tinyxml_extra_args = ['-DUSE_TINYXML2']
  if cpp.get_argument_syntax() == 'msvc'
      tinyxml_extra_args += '-D_CRT_SECURE_NO_WARNINGS'
  endif

  if get_option('default_library') == 'shared'
      tinyxml_extra_args += '-DTINYXML2_EXPORT'
  endif

  if get_option('debug')
      tinyxml_extra_args += '-DTINYXML2_DEBUG'
  endif

  cpp_args += tinyxml_extra_args
  lib_tinyxml2 = library(
    'tinyxml2',
    ['./external/tinyxml2-11.0.0/tinyxml2.cpp'],
    cpp_args : tinyxml_extra_args,
    gnu_symbol_visibility : 'hidden',
    version : meson.project_version(),
    install : true,
    version : '11.0.0',
  )
  core_links += [lib_tinyxml2]
endif

# stb
stb_includes = '../external/stb/.'
if USE_STB
  cpp_args += '-DUSE_STB'
  core_incs += include_directories(stb_includes)
endif

# pmgdlib
pmgdlib = shared_library('pmgdlib', 
  pmgdlib_sources,
  cpp_args : cpp_args,
  include_directories: core_incs, 
  dependencies: core_deps,
  link_with: core_links
)
core_links += [pmgdlib]

subdir('tests')
subdir('apps')
subdir('subs')
