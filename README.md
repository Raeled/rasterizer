# Software Rasterizer

Simple 3d software rasterizer that runs fully on the CPU without depending on
graphic libraries like `OpenGL` or `DirectX`.

## Getting started

This repository uses git submodules so run the following to download dependencies:
```
git submodule update --recursive --init
```

Preparing CMake for first use:
```
mkdir build
cd build
cmake ..
```

Build and run application using:
```
make && ./rasterizer
```
