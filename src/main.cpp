#include <vector>
#include <iostream>
#include <fstream>

#include <mathfu/glsl_mappings.h>
#include <mathfu/matrix.h>

#include <Bitmap.hpp>
#include <ObjFile.hpp>

int main() {

    Bitmap testBitmap(800, 600);
    testBitmap.clear(Color(0,0,0,0));

    std::vector<mathfu::vec4> positions;
    std::vector<int> indexes;

    loadObjFile("monkey.obj", positions, indexes);
    

    auto modelRotationMatrix = mathfu::Quaternion<float>::FromAngleAxis(0.0f * M_PI / 180.f, mathfu::vec3(0, 1, 0)).ToMatrix4();

    auto modelMatrix = mathfu::mat4::FromTranslationVector({0,0,3}) * modelRotationMatrix;
    auto viewMatrix = mathfu::mat4::Identity();
    auto projectionMatrix = mathfu::mat4::Perspective(60.0f * M_PI / 180.f, (float)testBitmap.width / (float)testBitmap.height, 0.1f, 50.0f);
    auto mvp = projectionMatrix * viewMatrix * modelMatrix;

    mathfu::vec2 halfSize(testBitmap.width * 0.5f, testBitmap.height * 0.5f);

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
            transformed[j] = transformed[j] / transformed[j].w;
            screenCoords[j] = transformed[j].xy() * halfSize + halfSize;
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
            color);

        for (int j=0; j<3; j++)
            drawLine(testBitmap, screenCoords[j].x, screenCoords[j].y, screenCoords[(j+1) % 3].x, screenCoords[(j+1) % 3].y, color);
    }

    writeBitmap("test.bmp", testBitmap);

    return 0;
}
