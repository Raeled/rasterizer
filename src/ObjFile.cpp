#include <ObjFile.hpp>

#include <fstream>
#include <map>

namespace {
    struct Index {
        int position;
        int uv;
        int normal;
    };

    inline bool operator <(const Index& a, const Index& b) {
        if (a.position != b.position) return a.position < b.position;
        if (a.uv != b.uv) return a.uv < b.uv;
        return a.normal < b.normal;
    }

    mathfu::vec2 readVec2(std::istream& stream) {
        mathfu::vec2 result;
        stream >> result.x;
        stream >> result.y;
        return result;
    }

    mathfu::vec3 readVec3(std::istream& stream) {
        mathfu::vec3 result;
        stream >> result.x;
        stream >> result.y;
        stream >> result.z;
        return result;
    }

    Index readIndex(std::istream& stream) {
        int index = 0, uvIndex = 0, normalIndex = 0;
        stream >> index;

        if (stream.peek() == '/') {
            stream.ignore();

            if (stream.peek() != '/')
                stream >> uvIndex;
        }

        if (stream.peek() == '/') {
            stream.ignore();
            stream >> normalIndex;
        }

        return { index, uvIndex, normalIndex };
    }
}

Mesh loadObjFile(const std::string& filename) {

    std::vector<mathfu::vec3> positions;
    std::vector<mathfu::vec3> normals;
    std::vector<mathfu::vec2> uvs;

    positions.push_back({0,0,0});
    normals.push_back({0,0,0});
    uvs.push_back({0,0});

    std::vector<Index> indexes;

    std::ifstream stream(filename, std::ios::in);
    std::string command, junk;
    while (stream) {
        command = ' ';
        std::getline(stream, command, ' ');

        if (command == "v") {
            positions.push_back(readVec3(stream));
        } if (command == "vn") {
            normals.push_back(readVec3(stream));
        }if (command == "vt") {
            uvs.push_back(readVec2(stream));
        } else if (command == "f") {
            for (int i=0; i<3; i++)
                indexes.push_back(readIndex(stream));
        }

        std::getline(stream, junk);
    }
    stream.close();


    Mesh result;
    std::map<Index, int> indexMap;
    for (auto& index : indexes) {
        const auto x = indexMap.find(index);
        if (x != indexMap.end()) {
            result.indexes.push_back(x->second);
        } else {
            result.vertexes.push_back({
                positions[index.position],
                normals[index.normal],
                uvs[index.uv]});
            result.indexes.push_back(result.vertexes.size() - 1);
            indexMap[index] = result.vertexes.size() - 1;
        }
    }

    return result;
}
