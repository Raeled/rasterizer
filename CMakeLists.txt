cmake_minimum_required (VERSION 2.8.11)
project (rasterizer)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    src
    lib/mathfu/include
    lib/mathfu/dependencies/vectorial/include
    )

add_executable (rasterizer
    src/Bitmap.cpp
    src/main.cpp)
