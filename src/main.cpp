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
#include <iostream>


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
};

class MeshRenderer : GameObject {
public:
    Transform transform;
    glm::mat4x4 M;
    MeshRenderer(Transform transform, std::string fileName) : transform(transform) {
        ModelLoader::loadPnuModel(FileLoader::getPath(fileName), vertices, indices);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PnuVertexInput), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);

        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PnuVertexInput), (void *) offsetof(PnuVertexInput, position));
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
    }

    void update() {
        transform.rotation.z += 0.01;
    }

    void render() {
        if (std::abs(transform.rotation.x) >= 360) transform.rotation.x = (int) transform.rotation.x % 360;
        if (std::abs(transform.rotation.y) >= 360) transform.rotation.y = (int) transform.rotation.y % 360;
        if (std::abs(transform.rotation.z) >= 360) transform.rotation.z = (int) transform.rotation.z % 360;
        auto S = glm::scale(glm::identity<glm::mat4x4>(), transform.scale);
        auto T = glm::translate(glm::identity<glm::mat4x4>(), transform.position);
        auto Rx = glm::rotate(glm::identity<glm::mat4x4>(), (float) (transform.rotation.x * PI / 180.0f), glm::vec3(1, 0, 0));
        auto Ry = glm::rotate(glm::identity<glm::mat4x4>(), (float) (transform.rotation.y * PI / 180.0f), glm::vec3(0, 1, 0));
        auto Rz = glm::rotate(glm::identity<glm::mat4x4>(), (float) (transform.rotation.z * PI / 180.0f), glm::vec3(0, 0, 1));
        M = T * Rz * Ry * Rx * S;

        glBindVertexArray(VAO);
        defaultShader.activateShader();
        defaultShader.setUniform("MVP", camera->P * camera->V * M);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    }

private:
    unsigned int VBO, EBO, VAO;
    std::vector<PnuVertexInput> vertices;
    std::vector<uint32_t> indices;
};

void init() {

    unsigned int vertex = Shader::createVertexShader(FileLoader::getPath("Resources/Shaders/DefaultVertex.glsl"));
    unsigned int fragment = Shader::createFragmentShader(FileLoader::getPath("Resources/Shaders/DefaultFragment.glsl"));
    defaultShader = Shader(vertex, fragment);

    camera = new Camera(glm::vec3(0, 0, 1), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    MeshRenderer *mesh = new MeshRenderer(Transform({0,0,-2}, {0,0,0}, {1,1,1}), "Resources/Meshes/cylinder.obj");
    MeshRenderer *mesh1 = new MeshRenderer(Transform({3,0,-2}, {0,0,0}, {1.2,1,1}), "Resources/Meshes/cylinder.obj");

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
