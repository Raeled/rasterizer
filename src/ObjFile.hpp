#pragma once

#include <string>
#include <vector>
#include <mathfu/glsl_mappings.h>

void loadObjFile(const std::string& filename, std::vector<mathfu::vec4>& positions, std::vector<int>& indexes);
