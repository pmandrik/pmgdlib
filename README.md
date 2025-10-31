# pmgdlib

## External
### SDL
https://github.com/libsdl-org/SDL/releases/tag/release-2.30.3
```
sudo apt get install libx11-dev libxext-dev # SDL_X11
sudo apt-get install pkg-config cmake g++
sudo apt-get install libasound2-dev

# GL
sudo apt-get install libgl-dev libxext-dev libgl1-mesa-dev

# VULKAN
sudo apt-get install libvulkan-dev

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSDL_OPENGL=ON -DSDL_VULKAN=ON
```

### spdlog
https://github.com/gabime/spdlog/releases/tag/v1.15.1
Header-only implementation was found to be too heavy leading to unacceptable compile time.

## compiling tests
```
# basic
sudo apt-get install git

# add meson
sudo apt-get install python3 python3-pip python3-setuptools \
                     python3-wheel ninja-build meson

# add tests
sudo apt-get install libgtest-dev

# compile
git clone https://github.com/pmandrik/pmgdlib.git
cd pmgdlib && meson setup build && cd build
meson compile && meson test
```

one-liner
```
meson compile; meson test;
```

### TODO LIST
1. add LoadFrameDrawer
2. add LoadTextureDrawer
3. move confit.h -> config.cpp 
4. Pipeline should be more clear,  should give information about the source and target