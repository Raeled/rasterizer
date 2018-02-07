#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <mathfu/glsl_mappings.h>
#include <mathfu/matrix.h>

#include <Bitmap.hpp>

int main() {

    Bitmap testBitmap(640, 480);
    testBitmap.clear(Color(0,0,0,0));

    mathfu::vec4 drawList[] = {
        mathfu::vec4(-1, -1,  1, 1), mathfu::vec4( 1, -1,  1, 1),
        mathfu::vec4( 1, -1,  1, 1), mathfu::vec4( 1,  1,  1, 1),
        mathfu::vec4( 1,  1,  1, 1), mathfu::vec4(-1,  1,  1, 1),
        mathfu::vec4(-1,  1,  1, 1), mathfu::vec4(-1, -1,  1, 1),

        mathfu::vec4(-1, -1, -1, 1), mathfu::vec4( 1, -1, -1, 1),
        mathfu::vec4( 1, -1, -1, 1), mathfu::vec4( 1,  1, -1, 1),
        mathfu::vec4( 1,  1, -1, 1), mathfu::vec4(-1,  1, -1, 1),
        mathfu::vec4(-1,  1, -1, 1), mathfu::vec4(-1, -1, -1, 1),

        mathfu::vec4(-1, -1,  1, 1), mathfu::vec4(-1, -1, -1, 1),
        mathfu::vec4( 1, -1,  1, 1), mathfu::vec4( 1, -1, -1, 1),
        mathfu::vec4( 1,  1,  1, 1), mathfu::vec4( 1,  1, -1, 1),
        mathfu::vec4(-1,  1,  1, 1), mathfu::vec4(-1,  1, -1, 1),
    };

    auto rotation = mathfu::Quaternion<float>::FromAngleAxis(30.f * M_PI / 180.f, mathfu::vec3(0, 1, 0)).ToMatrix4();

    auto modelMatrix = mathfu::mat4::FromTranslationVector({0,0,3}) * rotation;
    auto projectionMatrix = mathfu::mat4::Perspective(60.0f * M_PI / 180.f, (float)testBitmap.width / (float)testBitmap.height, 0.1f, 50.0f);

    int i = 0;
    mathfu::vec4 oldPos;
    for (auto& item : drawList) {
        auto transformed = projectionMatrix * modelMatrix * item;
        transformed = transformed / transformed.w;

        if (i % 2 == 1) {
            mathfu::vec2 halfSize(testBitmap.width * 0.5f, testBitmap.height * 0.5f);

            drawLine(testBitmap,
                oldPos.x * halfSize.x + halfSize.x,
                oldPos.y * halfSize.y + halfSize.y,
                transformed.x * halfSize.x + halfSize.x,
                transformed.y * halfSize.y + halfSize.y,
                Color(255,255,255,0));
        }

        oldPos = transformed;
        i++;
    }

    writeBitmap("test.bmp", testBitmap);

    return 0;
}
