#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <mathfu/glsl_mappings.h>
#include <mathfu/matrix.h>

#include <Bitmap.hpp>
#include <ObjFile.hpp>

struct DepthBuffer {
    int width;
    int height;
    std::vector<float> data;

    DepthBuffer(int width, int height) {
        this->width = width;
        this->height = height;
        this->data = std::vector<float>(width * height);
        this->clear();
    }

    void clear() {
        for (auto& value : data)
            value = 1000.0f;
    }

    bool test(int x, int y, float value) {
        if (x < 0 || x >= width || y < 0 || y >= height)
            return false;

        auto& currentValue = data[width * y + x];
        if (currentValue > value) {
            currentValue = value;
            return true;
        }

        return false;
    }
};

int main() {

    Bitmap testBitmap(800, 600);
    testBitmap.clear(Color(0,0,0,0));

    std::vector<mathfu::vec4> positions;
    std::vector<int> indexes;

    loadObjFile("monkey.obj", positions, indexes);
    for (auto& pos : positions) pos.y = -pos.y;
    for (int i=0; i<indexes.size(); i+= 3) {
        std::swap(indexes[i], indexes[i+1]);
    }

    auto modelRotationMatrix = mathfu::Quaternion<float>::FromAngleAxis(0.1f * M_PI / 180.f, mathfu::vec3(0, 1, 0)).ToMatrix4();

    auto modelMatrix = mathfu::mat4::FromTranslationVector({0,0,-3}) * modelRotationMatrix;
    auto viewMatrix = mathfu::mat4::Identity();
    auto projectionMatrix = mathfu::mat4::Perspective(60.0f * M_PI / 180.f, (float)testBitmap.width / (float)testBitmap.height, 0.1f, 50.0f);
    auto mvp = projectionMatrix * viewMatrix * modelMatrix;

    mathfu::vec2 halfSize(testBitmap.width * 0.5f, testBitmap.height * 0.5f);

    DepthBuffer depthBuffer(testBitmap.width, testBitmap.height);

    for (int i=0; i<indexes.size(); i+=3) {
        mathfu::vec4 transformed[3];
        mathfu::vec2 screenCoords[3];

        auto color = Color(255,255,255,0);

        mathfu::vec3 normal;
        {
            auto u = positions[indexes[i+1]] - positions[indexes[i+0]];
            auto v = positions[indexes[i+2]] - positions[indexes[i+0]];
            normal = mathfu::vec3::CrossProduct(u.xyz(), v.xyz()).Normalized();
        }

        // use normal as debug color
        color = Color(
            (normal.x * 0.5f + 0.5f) * 255,
            (normal.y * 0.5f + 0.5f) * 255,
            (normal.z * 0.5f + 0.5f) * 255,
            0
        );

        // calculate 2d screenCoords
        for (int j=0; j<3; j++) {
            transformed[j] = mvp * positions[indexes[i+j]];
            screenCoords[j] = transformed[j].xy() / transformed[j].w;
            screenCoords[j] = screenCoords[j] * halfSize + halfSize;
        }

        // backface culling
        auto u = screenCoords[1] - screenCoords[0];
        auto v = screenCoords[2] - screenCoords[0];
        auto cross = u.x * v.y - u.y * v.x;
        if (cross < 0) continue;

        // draw triangle
        fillTriangle(testBitmap,
            screenCoords[0].x, screenCoords[0].y,
            screenCoords[1].x, screenCoords[1].y,
            screenCoords[2].x, screenCoords[2].y,
            [color, &depthBuffer, transformed, screenCoords](int x, int y, bool& discard) -> Color {
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

                discard = !depthBuffer.test(x, y, depth);
                return color;
            });


        for (int j=0; j<3; j++)
            drawLine(testBitmap, screenCoords[j].x, screenCoords[j].y, screenCoords[(j+1) % 3].x, screenCoords[(j+1) % 3].y, color);
    }

    writeBitmap("test.bmp", testBitmap);

    return 0;
}
