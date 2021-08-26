//
// Created by menegais1 on 01/08/2020.
//

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "GraphicsLibrary.h"
#include "../FileManagers/FileLoader.h"
#include "Shader.h"
#include "../Callbacks.h"


void setupIMGui(GLFWwindow *window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(nullptr);

}

GLFWwindow *GraphicsLibrary::init(int width, int height, std::string title) {

    GLFWwindow *window;

    /* Initialize the library */
    if (!glfwInit())
        return nullptr;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (!gladLoadGL()) {
        std::cout << "Something went wrong!\n" << std::endl;
        exit(-1);
    }
    std::cout << "OpenGL " << GLVersion.major << "." << GLVersion.minor << std::endl;


    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSwapInterval(0);
    setupIMGui(window);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    return window;
}

void GraphicsLibrary::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void GraphicsLibrary::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    Callbacks::keyboard(key, scancode, action, mods);
}

void GraphicsLibrary::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    Callbacks::mouseMovement(xpos, ypos);
}

void GraphicsLibrary::mouseButtonCallback(GLFWwindow *window, int button, int action, int modifier) {
    Callbacks::mouseButton(button, action, modifier);
}

void GraphicsLibrary::render(GLFWwindow *window) {
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    Callbacks::render();

    ImGui::Render();

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}
//
//void GraphicsLibrary::screenSpaceLine(glm::vec2 p0, glm::vec2 p1, fvec4 color) {
//    unsigned int VBO, VAO;
//    fvec3 screenCoordinates = fvec3(GlobalManager::getInstance()->screenWidth,
//                                    GlobalManager::getInstance()->screenWidth, 1);
//    fvec3 screenP0 = fvec3(p0.x / screenCoordinates.x - 1, p0.y / screenCoordinates.y- 1, 0);
//    fvec3 screenP1 = fvec3(p1.x / screenCoordinates.x - 1, p1.y / screenCoordinates.y- 1, 0);
//    std::vector<fvec3> vertices = {screenP0, screenP1};
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(fvec3), vertices.data(), GL_STATIC_DRAW);
//    static Shader shader = Shader(
//            Shader::createVertexShader(FileLoader::getPath("Resources/Shaders/DefaultVertex.glsl")),
//            Shader::createFragmentShader(FileLoader::getPath("Resources/Shaders/DefaultFragment.glsl")));
//    shader.activateShader();
//    shader.setUniform("UNIFORM_MVP", fMatrix::identity(4));
//    shader.setUniform("UNIFORM_color", color);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), (void *) 0);
//    glEnableVertexAttribArray(0);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glDrawArrays(GL_LINE_STRIP, 0, 2);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    glDeleteBuffers(1, &VBO);
//    glDeleteVertexArrays(1, &VAO);
//
//}
//
//void GraphicsLibrary::line(fvec3 p0, fvec3 p1, fvec4 color) {
//    unsigned int VBO, VAO;
//    std::vector<fvec3> vertices = {p0, p1};
//    glGenVertexArrays(1, &VAO);
//    glBindVertexArray(VAO);
//    glGenBuffers(1, &VBO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(fvec3), vertices.data(), GL_STATIC_DRAW);
//    static Shader shader = Shader(
//            Shader::createVertexShader(FileLoader::getPath("Resources/Shaders/DefaultVertex.glsl")),
//            Shader::createFragmentShader(FileLoader::getPath("Resources/Shaders/DefaultFragment.glsl")));
//    shader.activateShader();
//    shader.setUniform("UNIFORM_MVP", Camera::getInstance()->Projection * Camera::getInstance()->View);
//    shader.setUniform("UNIFORM_color", color);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(fvec3), (void *) 0);
//    glEnableVertexAttribArray(0);
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//    glDrawArrays(GL_LINE_STRIP, 0, 2);
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//
//    glDeleteBuffers(1, &VBO);
//    glDeleteVertexArrays(1, &VAO);
//
//}
