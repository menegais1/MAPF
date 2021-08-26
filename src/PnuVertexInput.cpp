//
// Created by menegais on 24/08/2021.
//

#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include "PnuVertexInput.h"

bool PnuVertexInput::operator==(const PnuVertexInput &other) const
{
    return position == other.position && uv == other.uv && normal == other.normal;
}

PnuVertexInput::PnuVertexInput() : position(glm::vec3(0, 0, 0)), normal(glm::vec3(0, 0, 0)), uv(glm::vec3(0, 0, 0)), tangent(glm::vec3(0, 0, 0)) {}

PnuVertexInput::PnuVertexInput(const glm::vec3 &position, const glm::vec3 &normal, const glm::vec2 &uv) : position(position), normal(normal), uv(uv), tangent(glm::vec3(0, 0, 0)) {}