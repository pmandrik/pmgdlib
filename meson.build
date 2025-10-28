# P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
#                   https://inspirehep.net/authors/1395721
#                   https://vk.com/pmandrik

project('pmgdlib', 'cpp', default_options : ['c_std=c20', 'cpp_std=c++20'])
cpp_args=['-Wfatal-errors']
cpp = meson.get_compiler('cpp')

test_inc = include_directories('./')
core_deps = [test_inc]
core_links = []

# deps setup
USE_TINYXML2 = false
USE_STB = false

TEST_SDL = true
TEST_GL = false
TEST_VULKAN = false

# sdl2
sdl2_build_path = '/home/i/work/pmgdlib_external/SDL2-2.30.3/build'
sdl2_include_path = ['../../pmgdlib_external/SDL2-2.30.3/build/include/SDL2/.', '../../pmgdlib_external/SDL2-2.30.3/build/include-config-release/SDL2/.']

sdl2_build_path = '/home/petr/work/gd/SDL2-2.30.3/build'
sdl2_include_path = ['../../SDL2-2.30.3/build/include/SDL2/.', '../../SDL2-2.30.3/build/include-config-release/SDL2/.']

# gl
gl = dependency('gl', main: true, required: false)

# cxxopts
cxxopts_include_path = '../external/cxxopts-3.2.0/include/.'

# tinyxml2
tinyxml2_include_path = '../external/tinyxml2-11.0.0/.'
if USE_TINYXML2
  tinyxml2_inc = include_directories(tinyxml2_include_path)
  core_deps += [tinyxml2_inc]

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
endif

add_project_arguments(cpp_args, language: ['cpp'])
subdir('tests')
subdir('apps')
subdir('subs')
