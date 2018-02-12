#pragma once

#include <vector>
#include <Bitmap.hpp>
#include <mathfu/glsl_mappings.h>

struct DepthBuffer {
    int width;
    int height;
    std::vector<float> data;

    DepthBuffer(int width, int height);
    void clear();
    bool test(int x, int y, float value);
};

class Rasterizer {
public:
    typedef std::function<mathfu::vec4 (int, mathfu::vec3[])> VertexFunction;
    typedef std::function<Color (mathfu::vec3[])> PixelFunction;

    Bitmap* target = nullptr;
    DepthBuffer* depthBuffer = nullptr;
    unsigned char varyingCount = 0;
    VertexFunction vertexFunction;
    PixelFunction pixelFunction;

    void clear(Color clearColor);
    void drawIndexed(std::vector<unsigned int> indexes);
};
