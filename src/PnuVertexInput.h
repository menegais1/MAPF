//
// Created by menegais on 24/08/2021.
//

#ifndef MAPF_PNUVERTEXINPUT_H
#define MAPF_PNUVERTEXINPUT_H

#include <glm/gtx/hash.hpp>


/* Position + Normal + UV Vertex */
struct PnuVertexInput {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
    glm::vec3 tangent{};

    PnuVertexInput(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv);

    PnuVertexInput();

    bool operator==(const PnuVertexInput &other) const;
};


namespace std
{
    template<> struct hash<PnuVertexInput>
    {
        size_t operator()(PnuVertexInput const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^
                     (hash<glm::vec2>()(vertex.uv) << 1)) >> 1) ^
                   (hash<glm::vec3>()(vertex.normal) << 1);
        }
    };
}


#endif //MAPF_PNUVERTEXINPUT_H
