# pmgdlib

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
