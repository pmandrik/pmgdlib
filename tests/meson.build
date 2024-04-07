# P.~Mandrik, 2024, https://github.com/pmandrik/pmgdlib
#                   https://inspirehep.net/authors/1395721
#                   https://vk.com/pmandrik

test_incdir = include_directories('../')

gtest = dependency('gtest', main: true, required: false)
if not gtest.found()
  error('MESON_SKIP_TEST: gtest not installed.')
endif

all_testes = executable('all_tests', 'tests_main.cpp', include_directories: test_incdir, dependencies: gtest)
test('gtest test', all_testes)
