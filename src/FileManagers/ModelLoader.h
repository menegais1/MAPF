//
// Created by Lorenzo on 2/11/12021.
//

#ifndef MAPF_OBJECTLOADER_H
#define MAPF_OBJECTLOADER_H


#include "../PnuVertexInput.h"

class ModelLoader {
public:

    static void loadPnuModel(const std::string &inputFile, std::vector<PnuVertexInput> &vertexInputs, std::vector<uint32_t> &indexVector);

    static void calculateTangents(std::vector<PnuVertexInput> &vertexInputs, const std::vector<uint32_t>& vertexIndices);
};


#endif //MAPF_OBJECTLOADER_H