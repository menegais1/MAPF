#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Backend/GraphicsLibrary.h"
#include "Backend/Shader.h"
#include "FileManagers/FileLoader.h"
#include "Backend/Texture2D.h"
#include <glm/glm.hpp>
#include "Callbacks.h"
#include "GlobalManager.h"
#include "FileManagers/ModelLoader.h"
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include <iostream>
#include <random>


#define PI        3.14159265358979323846    /* pi */
#define PI_2        2 * PI
int WIDTH = 500, HEIGHT = 500;

Shader defaultShader;
unsigned int VAO;
glm::vec2 mousePosition;

float mouseSensitivity = 1;
float cameraSpeed = 50;

struct Camera : public GameObject {
public:
    glm::mat4x4 V;
    glm::mat4x4 P;

    Camera(glm::vec3 eye, glm::vec3 forward, glm::vec3 up) : eye(eye), forward(forward), up(up) {
        center = eye + forward;
        right = glm::cross(forward, up);
        V = glm::lookAt(eye, center, up);
        P = glm::perspective(PI / 4.0, (double) WIDTH / HEIGHT, 0.1, 100.0);

        angle = glm::vec2(0, 0);
        updateCameraCenter();
    }

    void keyboard(int key, int scancode, int action, int mods) override {
        float moveSpeed = cameraSpeed * GlobalManager::getInstance()->deltaTime;

        if (key == GLFW_KEY_W) {
            eye = eye + moveSpeed * forward;
            center = center + moveSpeed * forward;
        } else if (key == GLFW_KEY_S) {
            eye = eye - moveSpeed * forward;
            center = center - moveSpeed * forward;
        } else if (key == GLFW_KEY_D) {
            eye = eye + moveSpeed * right;
            center = center + moveSpeed * right;
        } else if (key == GLFW_KEY_A) {
            eye = eye - moveSpeed * right;
            center = center - moveSpeed * right;
        } else if (key == GLFW_KEY_Q) {
            eye = eye + moveSpeed * up;
            center = center + moveSpeed * up;
        } else if (key == GLFW_KEY_E) {
            eye = eye - moveSpeed * up;
            center = center - moveSpeed * up;
        }
    }

    void mouseButton(int button, int action, int modifier) override {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
            isDragging = true;
        } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
            isDragging = false;
        }
    }

    void updateCameraCenter() {
        glm::vec3 direction;
        direction.x = cos(angle.y * PI / 180.0) * cos(angle.x * PI / 180.0);
        direction.y = sin(angle.x * PI / 180.0);
        direction.z = sin(angle.y * PI / 180.0) * cos(angle.x * PI / 180.0);

        center = eye + glm::normalize(direction);
    }

    void mouseMovement(double xpos, double ypos) override {
        if (isDragging) {

            float xDelta = (xpos - lastMousePosition.x);
            float yDelta = (ypos - lastMousePosition.y);
            float xOffset = xDelta * mouseSensitivity;
            float yOffset = yDelta * mouseSensitivity;

            angle.x += yOffset;
            angle.y += xOffset;

            if (angle.x >= 89) {
                angle.x = 89;
            } else if (angle.x <= -89) {
                angle.x = -89;
            }

            updateCameraCenter();
        }
        lastMousePosition = glm::vec2(xpos, ypos);

    }

    void update() override {
        forward = glm::normalize(center - eye);
        right = glm::normalize(glm::cross(forward, up));
        V = glm::lookAt(eye, center, up);
    }

private:
    bool isDragging = false;
    glm::vec3 center;
    glm::vec3 eye;
    glm::vec2 lastMousePosition;
    glm::vec2 angle;
    glm::vec3 up;
    glm::vec3 forward;
    glm::vec3 right;

};


Camera *camera;

struct Transform {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale) : position(position), rotation(rotation), scale(scale) {}

    glm::mat4x4 getModelMatrix() {
        if (std::abs(rotation.x) >= 360) rotation.x = (int) rotation.x % 360;
        if (std::abs(rotation.y) >= 360) rotation.y = (int) rotation.y % 360;
        if (std::abs(rotation.z) >= 360) rotation.z = (int) rotation.z % 360;
        auto S = glm::scale(glm::identity<glm::mat4x4>(), scale);
        auto T = glm::translate(glm::identity<glm::mat4x4>(), position);
        auto Rx = glm::rotate(glm::identity<glm::mat4x4>(), (float) (rotation.x * PI / 180.0f), glm::vec3(1, 0, 0));
        auto Ry = glm::rotate(glm::identity<glm::mat4x4>(), (float) (rotation.y * PI / 180.0f), glm::vec3(0, 1, 0));
        auto Rz = glm::rotate(glm::identity<glm::mat4x4>(), (float) (rotation.z * PI / 180.0f), glm::vec3(0, 0, 1));
        glm::mat4x4 M = T * Rz * Ry * Rx * S;
        return M;
    }
};

class Mesh {
public:
    std::vector<PnuVertexInput> vertices;
    std::vector<uint32_t> indices;
    glm::vec3 minBound;
    glm::vec3 maxBound;
    glm::vec3 size;

    Mesh(std::string fileName, Transform transform) {
        ModelLoader::loadPnuModel(FileLoader::getPath(fileName), vertices, indices);
        auto M = transform.getModelMatrix();

        for (auto &vertex: vertices) {
            vertex.position = M * glm::vec4(vertex.position, 1);
        }

        recalculateBounds();

    }

    void recalculateBounds() {
        minBound = glm::vec3(std::numeric_limits<float>::max());
        maxBound = glm::vec3(std::numeric_limits<float>::min());
        for (PnuVertexInput vertex: vertices) {
            if (vertex.position.z < minBound.z) minBound.z = vertex.position.z;
            else if (vertex.position.z > maxBound.z) maxBound.z = vertex.position.z;

            if (vertex.position.y < minBound.y) minBound.y = vertex.position.y;
            else if (vertex.position.y > maxBound.y) maxBound.y = vertex.position.y;

            if (vertex.position.x < minBound.x) minBound.x = vertex.position.x;
            else if (vertex.position.x > maxBound.x) maxBound.x = vertex.position.x;
        }

        size = maxBound - minBound;

        std::cout << "( " << size.x << "," << size.y << "," << size.z << ")" << std::endl;
        std::cout << "( " << minBound.x << "," << minBound.y << "," << minBound.z << ")" << std::endl;
        std::cout << "( " << maxBound.x << "," << maxBound.y << "," << maxBound.z << ")" << std::endl;
    }
};

class MeshRenderer : GameObject {
public:
    Transform transform;
    glm::mat4x4 M;
    Mesh *mesh;

    MeshRenderer(Transform transform, Mesh *mesh) : transform(transform), mesh(mesh) {

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(PnuVertexInput), mesh->vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(uint32_t), mesh->indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PnuVertexInput), (void *) offsetof(PnuVertexInput, position));
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }


    void render() {
        M = transform.getModelMatrix();

        glBindVertexArray(VAO);
        defaultShader.activateShader();
        defaultShader.setUniform("MVP", camera->P * camera->V * M);
        glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);

    }

private:
    unsigned int VBO, EBO, VAO;

};

glm::ivec3 getGridIndex(int index, glm::vec3 gridSize) {
    int indexZ = index / (int) (gridSize.x * gridSize.y);
    int indexY = index % (int) (gridSize.x * gridSize.y) / (int) gridSize.x;
    int indexX = index % (int) (gridSize.x * gridSize.y) % (int) gridSize.x;
    return glm::ivec3(indexX, indexY, indexZ);
}

int getLinearIndex(glm::ivec3 index, glm::vec3 gridSize) {
    int linearIndex = index.z * (gridSize.x * gridSize.y) + index.y * gridSize.y + index.x;
    if (linearIndex > gridSize.x * gridSize.y * gridSize.z) {
        std::cerr << "AN ERROR HAS OCCURED" << std::endl;
        exit(-1);
    }
    return linearIndex;
}

std::vector<int> discretize(Mesh *mesh, glm::vec3 gridSize) {
    std::vector<int> validIndices;
    auto cellSize = mesh->size / gridSize;
    for (PnuVertexInput vertex: mesh->vertices) {
        auto position = vertex.position - mesh->minBound;
        glm::ivec3 index = position / cellSize;
        if (index.x >= gridSize.x)index.x = gridSize.x - 1;
        if (index.y >= gridSize.y)index.y = gridSize.y - 1;
        if (index.z >= gridSize.z)index.z = gridSize.z - 1;
        int linearIndex = getLinearIndex(index, gridSize);
        validIndices.push_back(linearIndex);
    }
    std::vector<int>::iterator it;
    it = std::unique(validIndices.begin(), validIndices.end());
    validIndices.resize(std::distance(validIndices.begin(), it));
    return validIndices;
}

std::vector<int> assignRandomGoals(int agentsNumber, std::vector<int> validGoals) {
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(validGoals.begin(), validGoals.end(), g);
    std::vector<int> goals;
    for (int i = 0; i < agentsNumber; i++) {
        goals.push_back(validGoals[i]);
    }
    return goals;
}

std::vector<int> assignClosestGoals(std::vector<int> startPositions, std::vector<int> validGoals, glm::vec3 gridSize) {
    std::vector<int> goals;

    for (int i = 0; i < startPositions.size(); i++) {
        glm::vec3 position = getGridIndex(startPositions[i], gridSize);
        float closestDist = std::numeric_limits<float>::max();
        int closestGoal = -1;
        for (int j = 0; j < validGoals.size(); ++j) {
            glm::vec3 goalPosition = getGridIndex(validGoals[j], gridSize);
            auto distance = glm::distance(position, goalPosition);
            if (distance < closestDist) {
                closestDist = distance;
                closestGoal = j;
            }
        }
        goals.push_back(validGoals[closestGoal]);
        validGoals.erase(validGoals.begin() + closestGoal);
    }
    return goals;
}

struct Node {
public:
    Node *parent;
    float g_cost;
    float h_cost;
    glm::ivec3 index;
    std::vector<Node *> neighbours;
    glm::vec3 position;

    Node(Node *parent, glm::ivec3 index, glm::vec3 position) : parent(parent), index(index), position(position) {
        reset();
    }

    float getFCost() {
        return g_cost + h_cost;
    }

    void reset() {
        parent = nullptr;
        g_cost = std::numeric_limits<float>::max();
        h_cost = std::numeric_limits<float>::max();
    }
};

std::vector<Node *> getNeighbours(std::unordered_map<glm::ivec3, Node *> &nodes, Node *node, glm::vec3 gridSize) {
    std::vector<Node *> neighbours;
    auto nodeIndex = node->index;
    std::vector<glm::ivec3> indices;
    indices.push_back(nodeIndex + glm::ivec3(-1, 0, 0));
    indices.push_back(nodeIndex + glm::ivec3(1, 0, 0));
    indices.push_back(nodeIndex + glm::ivec3(0, 1, 0));
    indices.push_back(nodeIndex + glm::ivec3(0, -1, 0));
    indices.push_back(nodeIndex + glm::ivec3(0, 0, 1));
    indices.push_back(nodeIndex + glm::ivec3(0, 0, -1));

    for (auto index: indices) {
        if (nodes.contains(index)) {
            neighbours.push_back(nodes[index]);
        }
    }

    return neighbours;
}

Node *getSmallestCostNode(std::vector<Node *> &nodes) {
    float leastCost = std::numeric_limits<float>::max();
    Node *leastCostNode;
    for (auto node: nodes) {
        if (node->getFCost() < leastCost) {
            leastCost = node->getFCost();
            leastCostNode = node;
        }
    }

    return leastCostNode;
}

std::vector<glm::ivec3> getAStarPath(Node *endNode, std::unordered_map<int, std::vector<glm::ivec3>> &collisionTable) {
    std::vector<glm::ivec3> indices;
    indices.push_back(endNode->index);
    Node *curNode = endNode->parent;
    while (curNode != nullptr) {
        indices.push_back(curNode->index);
        curNode = curNode->parent;
    }
    std::reverse(indices.begin(), indices.end());

    int round = 0;
    for (auto index: indices) {
        collisionTable[round].push_back(index);
        round++;
    }
    return indices;
}

float l1_norm(Node *current, Node *last) {
    return glm::l1Norm(current->position, last->position);
}

void resetNodes(std::unordered_map<glm::ivec3, Node *> &nodes) {
    for (auto item: nodes) {
        item.second->reset();
    }
}

std::vector<glm::ivec3> pathFindingAStar(Node *startNode, Node *endNode, std::unordered_map<int, std::vector<glm::ivec3>> &collisionTable) {
    std::vector<Node *> open;
    std::vector<Node *> closed;
    int round = -1;
    startNode->h_cost = l1_norm(startNode, endNode);
    startNode->g_cost = 0;
    open.push_back(startNode);
    Node *current;
    while (open.size() > 0) {
        round++;
        current = getSmallestCostNode(open);

        if (current == endNode) return getAStarPath(current, collisionTable);
        auto it = std::remove(open.begin(), open.end(), current);
        open.resize(std::distance(open.begin(), it));
        closed.push_back(current);
        for (auto neighbour: current->neighbours) {
            if (std::find(closed.begin(), closed.end(), neighbour) != closed.end()) continue;
            float cost = glm::l1Norm(neighbour->position, current->position) + current->g_cost;
            bool notInOpen = std::find(open.begin(), open.end(), neighbour) == open.end();
            bool collision = std::find(collisionTable[round].begin(), collisionTable[round].end(), neighbour->index) != collisionTable[round].end();
            if (collision) {
                std::cerr << "UMA COLISÃO FOI ENCONTRADA" << std::endl;
            }
            if ((cost < neighbour->g_cost || notInOpen) && !collision) {
                neighbour->g_cost = cost;
                neighbour->h_cost = l1_norm(neighbour, endNode);
                neighbour->parent = current;

                if (notInOpen) {
                    open.push_back(neighbour);
                }
            }
        }

    }

    return getAStarPath(current, collisionTable);
}

class MAPF : public GameObject {
public:
    std::vector<MeshRenderer *> agents;
    std::vector<std::vector<glm::ivec3>> paths;
    int step = 0;
    Mesh *mesh;
    glm::vec3 cellSize;
    int finalStep = -1;

    MAPF(const std::vector<MeshRenderer *> &agents, const std::vector<std::vector<glm::ivec3>> &paths, Mesh *mesh, glm::vec3 cellSize) : agents(agents), paths(paths),
                                                                                                                                         mesh(mesh), cellSize(cellSize) {
        for (const auto &path: paths) {
            if (int(path.size() - 1) > finalStep) {
                finalStep = int(path.size()) - 1;
            }
        }

    }

    void keyboard(int key, int scancode, int action, int mods) override {
        if (key == GLFW_KEY_RIGHT && action == GLFW_RELEASE)step++;
        if (key == GLFW_KEY_LEFT && action == GLFW_RELEASE) step--;

        if (step < 0) step = 0;
        if (step > finalStep) step = finalStep;
        for (int i = 0; i < agents.size(); ++i) {
            auto path = paths[i];
            if (step >= path.size()) continue;
            auto gridIndex = path[step];
            agents[i]->transform.position = glm::vec3(mesh->minBound.x + (gridIndex.x * cellSize.x) + (cellSize.x / 2),
                                                      mesh->minBound.y + (gridIndex.y * cellSize.y) + (cellSize.y / 2),
                                                      mesh->minBound.z + (gridIndex.z * cellSize.z) + (cellSize.z / 2));
        }
    }
};

void init() {

    int agentsNumber = 1000;
    glm::vec3 gridSize = glm::vec3(100, 100, 100);

    unsigned int vertex = Shader::createVertexShader(FileLoader::getPath("Resources/Shaders/DefaultVertex.glsl"));
    unsigned int fragment = Shader::createFragmentShader(FileLoader::getPath("Resources/Shaders/DefaultFragment.glsl"));
    defaultShader = Shader(vertex, fragment);

    camera = new Camera(glm::vec3(0, 0, 1), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    Mesh *mesh = new Mesh("Resources/Meshes/ufsm.obj", Transform({0, 0, 0}, {180, 0, 0}, {1, 1, 1}));
    Mesh *cube = new Mesh("Resources/Meshes/Cube.obj", Transform({0, 0, 0}, {0, 0, 0}, {1, 1, 1}));
//    MeshRenderer *meshRenderer = new MeshRenderer(Transform({0, 0, 0}, {0, 0, 0}, {1, 1, 1}), mesh);
    glm::vec3 cellSize = mesh->size / gridSize;


    //Create graph structure and add neighbours
    std::cout << "Generating Nodes in Graph" << std::endl;
    std::unordered_map<glm::ivec3, Node *> nodes;
    for (int x = 0; x < gridSize.x; ++x) {
        for (int y = 0; y < gridSize.y; ++y) {
            for (int z = 0; z < gridSize.z; ++z) {
                glm::ivec3 index = glm::ivec3(x, y, z);
                glm::vec3 position = glm::vec3(mesh->minBound.x + (index.x * cellSize.x) + (cellSize.x / 2),
                                               mesh->minBound.y + (index.y * cellSize.y) + (cellSize.y / 2),
                                               mesh->minBound.z + (index.z * cellSize.z) + (cellSize.z / 2));
                nodes[index] = new Node(nullptr, index, position);
            }
        }
    }
    std::cout << "Setting neighbours in Graph" << std::endl;
    for (auto item: nodes) {
        Node *node = item.second;
        node->neighbours = getNeighbours(nodes, node, gridSize);
    }
    //End creation


    auto validIndices = discretize(mesh, gridSize);

    std::vector<int> start;
    for (int i = 0; i < agentsNumber; ++i) {
        start.push_back(rand() % (int) (gridSize.x * gridSize.y * gridSize.z));
    }

//    auto goals = assignRandomGoals(agentsNumber, validIndices);
    auto goals = assignClosestGoals(start, validIndices, gridSize);

    std::vector<std::vector<glm::ivec3>> paths;
    std::unordered_map<int, std::vector<glm::ivec3>> collisionTable;

    for (int i = 0; i < agentsNumber; ++i) {
        auto startIndex = getGridIndex(start[i], gridSize);
        auto endIndex = getGridIndex(goals[i], gridSize);
        auto path = pathFindingAStar(nodes[startIndex], nodes[endIndex], collisionTable);
        paths.push_back(path);
        std::cout << "Agent: " << i << " has " << path.size() << std::endl;
        resetNodes(nodes);
    }

    std::vector<MeshRenderer *> agents;
    for (auto index: start) {
        glm::ivec3 gridIndex = getGridIndex(index, gridSize);
        agents.push_back(new MeshRenderer(Transform({mesh->minBound.x + (gridIndex.x * cellSize.x) + (cellSize.x / 2),
                                                     mesh->minBound.y + (gridIndex.y * cellSize.y) + (cellSize.y / 2),
                                                     mesh->minBound.z + (gridIndex.z * cellSize.z) + (cellSize.z / 2)},
                                                    {0, 0, 0}, {0.01, 0.01, 0.01}), cube));
    }
    MAPF *mapf = new MAPF(agents, paths, mesh, cellSize);
}

void Callbacks::mouseMovement(double x, double y) {
    GlobalManager::getInstance()->mouseMovement(x, y);

}

void Callbacks::keyboard(int key, int scancode, int action, int mods) {
    GlobalManager::getInstance()->keyboard(key, scancode, action, mods);
}

void Callbacks::mouseButton(int button, int action, int modifier) {
    GlobalManager::getInstance()->mouseButton(button, action, modifier);

}

void Callbacks::render() {
    GlobalManager::getInstance()->render();
    ImGui::Begin("Debug", nullptr);
    ImGui::LabelText("Mouse Position x", std::to_string(GlobalManager::getInstance()->mousePosition.x).c_str());
    ImGui::LabelText("Mouse Position y", std::to_string(GlobalManager::getInstance()->mousePosition.y).c_str());
    ImGui::End();
}

int main(void) {
    auto window = GraphicsLibrary::init(WIDTH, HEIGHT, "OpenGlBase");
    init();
    while (!glfwWindowShouldClose(window)) {
        GraphicsLibrary::render(window);
    }

    glfwTerminate();
    return 0;
}
