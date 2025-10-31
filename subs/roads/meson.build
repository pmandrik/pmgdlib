# P.~Mandrik, 2025, https://github.com/pmandrik/pmgdlib

if sdl2.found()
    test_exec = executable('rroads', 
        'main.cpp', 
        include_directories: core_deps + [sdl2_inc], 
        dependencies: [sdl2],
        link_with : core_links
    )
endif