
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <iostream>
#include <glm/glm.hpp>
#include <unordered_map>
#include "ModelLoader.h"
#include "../PnuVertexInput.h"

void ModelLoader::loadPnuModel(const std::string &inputFile, std::vector<PnuVertexInput> &vertices,
                                std::vector<uint32_t> &indices) {
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(inputFile))
    {
        if (!reader.Error().empty())
        {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty())
    {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto &attrib = reader.GetAttrib();
    auto &shapes = reader.GetShapes();
    auto &primaryMesh = shapes[0]; /* Fetch the first shape */
    std::unordered_map<PnuVertexInput, uint32_t> uniqueVertices{};
    std::cout << "Loading model: " << primaryMesh.name << std::endl;

    for (const auto &index : primaryMesh.mesh.indices)
    {

        glm::vec3 pos = {attrib.vertices[3 * index.vertex_index + 0],
                         attrib.vertices[3 * index.vertex_index + 1],
                         attrib.vertices[3 * index.vertex_index + 2]};
        glm::vec2 texCoord = glm::vec2(0,0);
        if(index.texcoord_index >= 0){
            glm::vec2 texCoord = {attrib.texcoords[2 * index.texcoord_index + 0],
                                  1.0 - attrib.texcoords[2 * index.texcoord_index + 1]};
        }


        glm::vec3 normal = {attrib.normals[3 * index.normal_index + 0],
                            attrib.normals[3 * index.normal_index + 1],
                            attrib.normals[3 * index.normal_index + 2]};

        PnuVertexInput vertex = PnuVertexInput(pos, normal, texCoord);

        if (uniqueVertices.count(vertex) == 0) {
            uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
        }

        indices.push_back(uniqueVertices[vertex]);
    }
    std::cout << primaryMesh.name << " loaded!" << std::endl;

}

//https://bgolus.medium.com/normal-mapping-for-a-triplanar-shader-10bf39dca05a#0576
//http://www.thetenthplanet.de/archives/1180
//http://www.aclockworkberry.com/shader-derivative-functions/
//https://learnopengl.com/Advanced-Lighting/Normal-Mapping
void ModelLoader::calculateTangents(std::vector<PnuVertexInput> &vertexInputs, const std::vector<uint32_t> &vertexIndices)
{
    // FOR EACH TRIANGLE, CALCULATE THE TANGENT AND STORE THE SUM OF ALL TANGENTS IN THE SAME VERTEX IN A TEMPORARY ARRAY
    for (int i = 0; i < vertexIndices.size(); i += 3)
    {
        PnuVertexInput p0 = vertexInputs[vertexIndices[i]];
        PnuVertexInput p1 = vertexInputs[vertexIndices[i + 1]];
        PnuVertexInput p2 = vertexInputs[vertexIndices[i + 2]];

        //Bellow there is two different forms for calculating the tangents, both should work
        glm::vec3 e1 = p1.position - p0.position;
        glm::vec3 e2 = p2.position - p0.position;
        glm::vec3 n = glm::cross(e1, e2);
        glm::mat3 A = glm::transpose(glm::mat3(e1, e2, n));
        glm::mat3 AI = glm::inverse(A);
        glm::vec3 tangent = AI * glm::vec3(p1.uv.x - p0.uv.x, p2.uv.x - p0.uv.x, 0);
        // glm::vec2 uv1 = p1.uv - p0.uv;
        // glm::vec2 uv2 = p2.uv - p0.uv;
        // float f = 1.0f / (uv1.x * uv2.y - uv2.x * uv1.y);
        // glm::vec3 tangent = f * glm::vec3(uv2.y * e1.x - uv1.y * e2.x, uv2.y * e1.y - uv1.y * e2.y, uv2.y * e1.z - uv1.y * e2.z);
        vertexInputs[vertexIndices[i]].tangent += tangent;
        vertexInputs[vertexIndices[i + 1]].tangent += tangent;
        vertexInputs[vertexIndices[i + 2]].tangent += tangent;
    }
    for (auto & vertexInput : vertexInputs)
    {
        vertexInput.tangent = glm::normalize(vertexInput.tangent);
    }
}