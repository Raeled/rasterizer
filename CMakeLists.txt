cmake_minimum_required (VERSION 2.8.11)
project (rasterizer)

set(CMAKE_CXX_STANDARD 14)

configure_file(monkey.obj monkey.obj COPYONLY)
configure_file(plane.obj plane.obj COPYONLY)

include_directories(
    src
    lib/mathfu/include
    lib/mathfu/dependencies/vectorial/include
    )

add_executable (rasterizer
    src/Bitmap.cpp
    src/ObjFile.cpp
    src/Rasterizer.cpp
    src/main.cpp)
