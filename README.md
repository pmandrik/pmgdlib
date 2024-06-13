# pmgdlib

## External
### SDL
https://github.com/libsdl-org/SDL/releases/tag/release-2.30.3
```
sudo apt-get install pkg-config cmake
sudo apt-get install libasound2-dev
sudo apt-get install libgl-dev libxext-dev libgl1-mesa-dev

mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DSDL_OPENGL=ON
```

## compiling tests
```
# add meson
sudo apt-get install python3 python3-pip python3-setuptools \
                     python3-wheel ninja-build meson

# add tests
sudo apt-get install libgtest-dev

# compile
meson setup build && cd build
meson compile && meson test
```

one-liner
```
meson compile; meson test;
```
