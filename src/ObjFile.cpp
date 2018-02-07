#include <ObjFile.hpp>

#include <fstream>

void loadObjFile(const std::string& filename, std::vector<mathfu::vec4>& positions, std::vector<int>& indexes) {
    std::ifstream stream(filename, std::ios::in);
    std::string command;
    std::string junk;
    while (stream) {
        command = ' ';
        std::getline(stream, command, ' ');

        if (command == "v") {
            mathfu::vec4 pos(0,0,0,1);
            stream >> pos.x;
            stream >> pos.y;
            stream >> pos.z;

            positions.push_back(pos);
        } else if (command == "f") {

            for (int i=0; i<3; i++) {
                int index, uvIndex, normalIndex;
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

                indexes.push_back(index - 1);
            }
        }

        std::getline(stream, junk);
    }
}
