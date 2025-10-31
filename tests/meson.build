# P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
#                   https://inspirehep.net/authors/1395721
#                   https://vk.com/pmandrik

gtest = dependency('gtest', main: true, required: false)
if not gtest.found()
  error('MESON_SKIP_TEST: gtest not installed.')
endif

# use spdlog
if false
  spdlog_inc = include_directories('../external')
  core_deps += [spdlog_inc]
endif

# use tinyxml2
if USE_TINYXML2
  core_deps += [tinyxml2_inc]
endif

if USE_STB
  core_deps += [stb_includes]
endif

all_testes = executable('all_tests', 
  'tests_main.cpp',
  include_directories: core_deps, 
  dependencies: gtest, 
  link_with : core_links
)

test('gtest test', all_testes)

if TEST_SDL
  # sdl2 dependency via dependency - X
  if false
    # sdl2_dep = dependency('sdl2')
  endif

  # sdl2 dependency via sdl2-config - X
  if false
    sdl2_cfg_path = '/home/i/work/pmgdlib_external/SDL2-2.30.3_build'
    sdl2_cfg = join_paths(sdl2_cfg_path, 'sdl2-config')

    sdl2_ldflags = run_command(sdl2_cfg, '--libs', check: true).stdout().strip()
    sdl2_cflags = run_command(sdl2_cfg, '--cflags', check: true).stdout().strip()
    message('sdl ldflags:', sdl2_ldflags)
    message('sdl cflags:', sdl2_cflags)
  endif

  # sdl2 dependency - V
  sdl2 = cpp.find_library('SDL2-2.0', dirs : sdl2_build_path)
  sdl2_inc = include_directories(sdl2_include_path)

  sdl_tests = ['test_sdl_core']
  foreach sdl_test : sdl_tests
    test_exec = executable(sdl_test, sdl_test + '.cpp', 
      include_directories: core_deps + [sdl2_inc], 
      dependencies: [sdl2, gtest], 
      link_with : core_links
    )
    test(sdl_test, test_exec)
  endforeach

  # gl tests
  if TEST_GL and gl.found()
    sdl_tests = ['test_sdl_gl', 'test_sdl_gl_pmgdlib', 'test_gl_factory']
    foreach sdl_test : sdl_tests
      test_exec = executable(sdl_test, sdl_test + '.cpp', 
        include_directories: core_deps + [sdl2_inc], 
        dependencies: [sdl2, gtest, gl], 
        link_with : core_links
      )
      test(sdl_test, test_exec)
    endforeach
  endif

  # vulkan tests
  if TEST_VULKAN
    vulkan = dependency('vulkan', main: true, required: true)
    sdl_tests = ['test_sdl_vulkan']
    foreach sdl_test : sdl_tests
      test_exec = executable(sdl_test, 
        sdl_test + '.cpp', 
        include_directories: core_deps + [sdl2_inc], 
        dependencies: [sdl2, gtest, vulkan],
        link_with : core_links
      )
      test(sdl_test, test_exec)
    endforeach
  endif
endif
