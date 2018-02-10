#pragma once

#include <string>
#include <vector>
#include <mathfu/glsl_mappings.h>

struct Mesh {
    struct Vertex {
        mathfu::vec3 position;
        mathfu::vec3 normal;
        mathfu::vec2 uv;
    };

    std::vector<Vertex> vertexes;
    std::vector<int> indexes;
};

Mesh loadObjFile(const std::string& filename);
