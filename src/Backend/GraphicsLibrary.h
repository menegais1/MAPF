//
// Created by menegais1 on 01/08/2020.
//

#ifndef RENDERERENGINE_GRAPHICSLIBRARY_H
#define RENDERERENGINE_GRAPHICSLIBRARY_H


#include <string>
#include <GLFW/glfw3.h>

class GraphicsLibrary {
public:

    static GLFWwindow *init(int width, int height, std::string title);

    static void render(GLFWwindow *window);

//    static void screenSpaceLine(glm::vec2 p0, glm::vec2 p1, fvec4 color = fvec4(0, 0, 0, 1));
//
//    static void line(fvec3 p0, fvec3 p1, fvec4 color = fvec4(0, 0, 0, 1));

private:
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int modifier);

    static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);

    static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height);

};


#endif //RENDERERENGINE_GRAPHICSLIBRARY_H
