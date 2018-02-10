#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <mathfu/glsl_mappings.h>
#include <mathfu/matrix.h>

#include <Bitmap.hpp>
#include <ObjFile.hpp>
#include <Rasterizer.hpp>

Bitmap bitmapFromDepthBuffer(const DepthBuffer& depthBuffer) {
    Bitmap zBufferBitmap(depthBuffer.width, depthBuffer.height);

    float minDepth = 900.0f;
    float maxDepth = -900.0f;

    for (auto& point : depthBuffer.data) {
        if (point > 950.0f) continue;

        minDepth = std::min(point, minDepth);
        maxDepth = std::max(point, maxDepth);
    }
    auto range = maxDepth - minDepth;

    for (int i=0; i<depthBuffer.data.size(); i++) {
        auto point = -std::min(1.0f, (depthBuffer.data[i] - minDepth) / range) + 1.0f;
        zBufferBitmap.colorData[i] = Color(point * 255, point * 255, point * 255, 0);
    }

    return zBufferBitmap;
}

std::ostream& operator<< (std::ostream& stream, const mathfu::vec4 val) {
    stream << val.x << ", " << val.y << ", " << val.z << ", " << val.w;
    return stream;
}

Color makeColor(mathfu::vec2 color) {
    return Color(color.x * 255, color.y * 255, 0, 0);
}

Color makeColor(mathfu::vec3 color) {
    return Color(color.x * 255, color.y * 255, color.z * 255, 0);
}

int main() {

    Bitmap testBitmap(800, 600);
    DepthBuffer depthBuffer(testBitmap.width, testBitmap.height);

    auto monkeyMesh = loadObjFile("monkey.obj");
    auto planeMesh = loadObjFile("plane.obj");

    auto modelRotationMatrix = mathfu::Quaternion<float>::FromAngleAxis(45.f * M_PI / 180.f, mathfu::vec3(0, 1, 0)).ToMatrix4();

    auto modelMatrix = mathfu::mat4::FromTranslationVector({0,0,-2.5}) * modelRotationMatrix;
    auto viewMatrix = mathfu::mat4::Identity();
    auto projectionMatrix = mathfu::mat4::Perspective(60.0f * M_PI / 180.f, (float)testBitmap.width / (float)testBitmap.height, 0.1f, 50.0f);
    auto mvp = projectionMatrix * viewMatrix * modelMatrix;

    struct State {
        Mesh* mesh;
        mathfu::mat4 mvpMatrix;
    } state;

    state.mesh = &monkeyMesh;
    state.mvpMatrix = mvp;

    Rasterizer rasterizer;
    rasterizer.target = &testBitmap;
    rasterizer.depthBuffer = &depthBuffer;
    rasterizer.varyingCount = 2;
    rasterizer.vertexFunction = [&](int ndx, mathfu::vec3 varying[]) -> mathfu::vec4 {
        varying[0] = mathfu::vec3(state.mesh->vertexes[ndx].uv, 0);
        varying[1] = state.mesh->vertexes[ndx].normal;

        return state.mvpMatrix * mathfu::vec4(state.mesh->vertexes[ndx].position, 1);
    };
    rasterizer.pixelFunction = [](mathfu::vec3 varying[]) -> Color {
        return makeColor(varying[1] * 0.5f + 0.5f);
    };

    rasterizer.clear(Color(0,0,0,0));
    state.mesh = &monkeyMesh;
    rasterizer.drawIndexed(monkeyMesh.indexes);

    state.mesh = &planeMesh;
    rasterizer.drawIndexed(planeMesh.indexes);


    writeBitmap("test.bmp", testBitmap);
    writeBitmap("depth.bmp", bitmapFromDepthBuffer(depthBuffer));

    return 0;
}
