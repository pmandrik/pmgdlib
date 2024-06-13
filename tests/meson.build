# P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
#                   https://inspirehep.net/authors/1395721
#                   https://vk.com/pmandrik

TEST_SDL = true

test_incdir = include_directories('../')
cpp = meson.get_compiler('cpp')

gtest = dependency('gtest', main: true, required: false)
if not gtest.found()
  error('MESON_SKIP_TEST: gtest not installed.')
endif

all_testes = executable('all_tests', 'tests_main.cpp', include_directories: test_incdir, dependencies: gtest)
test('gtest test', all_testes)

if TEST_SDL
  #sdl2_dep = dependency('sdl2')
  sdl2_build_path = '/home/i/work/pmgdlib_external/SDL2-2.30.3/build'
  sdl2_cfg_path = '/home/i/work/pmgdlib_external/SDL2-2.30.3_build'
  sdl2_cfg = join_paths(sdl2_cfg_path, 'sdl2-config')

  sdl2_ldflags = run_command(sdl2_cfg, '--libs', check: true).stdout().strip()
  sdl2_cflags = run_command(sdl2_cfg, '--cflags', check: true).stdout().strip()
  message('sdl ldflags:', sdl2_ldflags)
  message('sdl cflags:', sdl2_cflags)
  sdl2_inc = include_directories('../../pmgdlib_external/SDL2-2.30.3/build/include/SDL2/.',
                                 '../../pmgdlib_external/SDL2-2.30.3/build/include-config-release/SDL2/.')

  sdl2 = cpp.find_library('SDL2-2.0', dirs : sdl2_build_path)

  gl = dependency('gl', main: true, required: false)

  sdl_tests = ['test_sdl_native',
               'test_sdl_core']
  foreach sdl_test : sdl_tests
    test_exec = executable(sdl_test, sdl_test + '.cpp', include_directories: [test_incdir, sdl2_inc], dependencies: [sdl2, gtest, gl])
    test(sdl_test, test_exec)
  endforeach
endif
