#include <Rasterizer.hpp>

#include <mathfu/matrix.h>

DepthBuffer::DepthBuffer(int width, int height) {
    this->width = width;
    this->height = height;
    this->data = std::vector<float>(width * height);
    this->clear();
}

void DepthBuffer::clear() {
    for (auto& value : data)
        value = 1000.0f;
}

bool DepthBuffer::test(int x, int y, float value) {
    if (x < 0 || x >= width || y < 0 || y >= height)
        return false;

    auto& currentValue = data[width * y + x];
    if (currentValue > value) {
        currentValue = value;
        return true;
    }

    return false;
}

void Rasterizer::clear(Color clearColor) {
    if (target) target->clear(clearColor);
    if (depthBuffer) depthBuffer->clear();
}

void Rasterizer::drawIndexed(std::vector<int> indexes) {
    mathfu::vec2 halfSize(target->width * 0.5f, target->height * 0.5f);

    mathfu::vec4 transformed[3];
    mathfu::vec2 screenCoords[3];
    mathfu::vec3* varying[3];

    for (int j=0; j<3; j++)
        varying[j] = new mathfu::vec3[varyingCount];

    for (int i=0; i<indexes.size(); i+=3) {

        // calculate 2d screenCoords
        for (int j=0; j<3; j++) {
            transformed[j] = this->vertexFunction(indexes[i+j], varying[j]);

            screenCoords[j] = transformed[j].xy() / transformed[j].w;
            screenCoords[j].y = - screenCoords[j].y;
            screenCoords[j] = screenCoords[j] * halfSize + halfSize;
        }

        // backface culling
        auto u = screenCoords[1] - screenCoords[0];
        auto v = screenCoords[2] - screenCoords[0];
        auto cross = u.x * v.y - u.y * v.x;
        if (cross > 0) continue;

        // draw triangle
        fillTriangle(*target,
            screenCoords[0].x, screenCoords[0].y,
            screenCoords[1].x, screenCoords[1].y,
            screenCoords[2].x, screenCoords[2].y,
            [this, transformed, screenCoords, varying](int x, int y, bool& discard) {
                auto a = mathfu::vec3(screenCoords[0], 0);
                auto b = mathfu::vec3(screenCoords[1], 0);
                auto c = mathfu::vec3(screenCoords[2], 0);

                mathfu::vec3 p(x, y, 0);

                auto normal = mathfu::vec3::CrossProduct(b-a, c-a);

                auto areaABC = mathfu::vec3::DotProduct(normal, mathfu::vec3::CrossProduct(b-a, c-a));
                auto areaPBC = mathfu::vec3::DotProduct(normal, mathfu::vec3::CrossProduct(b-p, c-p));
                auto areaPCA = mathfu::vec3::DotProduct(normal, mathfu::vec3::CrossProduct(c-p, a-p));

                auto weightA = areaPBC / areaABC;
                auto weightB = areaPCA / areaABC;
                auto weightC = 1.0f - weightA - weightB;

                auto depth = weightA * transformed[0].z + weightB * transformed[1].z + weightC * transformed[2].z;

                if (depthBuffer)
                    discard = !depthBuffer->test(x, y, depth);
                if (discard) { return Color(0,0,0,0); }

                mathfu::vec3* pixelVarying = new mathfu::vec3[varyingCount];
                for (int j=0; j<varyingCount; j++) {
                    pixelVarying[j] = weightA * varying[0][j] + weightB * varying[1][j] + weightC * varying[2][j];
                }
                auto result = this->pixelFunction(pixelVarying);
                delete[] pixelVarying;
                return result;
            });
    }

    for (int j=0; j<3; j++)
        delete[] varying[j];
}
