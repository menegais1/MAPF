///Manager responsible for forwarding the GLFW callbacks
///To every GameObject instantiated, every object has a uniqueId given
///in the beggining of its lifetime

#ifndef GLOBAL_MANAGER_H
#define GLOBAL_MANAGER_H

#include <vector>
#include <chrono>

#include "GameObject.h"
#include <glm/glm.hpp>
class GlobalManager {
public:
    void keyboard(int key, int scancode, int action, int mods);

    void mouseButton(int button, int action, int modifier);

    void mouseMovement(double xpos, double ypos);

    void render();

    int registerObject(GameObject *object);

    GameObject *unregisterObject(GameObject *object);

    static GlobalManager *getInstance();

    GameObject *deleteObject(GameObject *object);

    void changeObjectOrder(GameObject *object);

    void reshape(int width, int height);

    int screenWidth;
    int screenHeight;
    glm::vec2 mousePosition;
    float deltaTime;
    float time = 0;
    float fpsUpdateCycle = 0.25;
    float lastFpsUpdate = 0;
    int fps;
    int drawCallsPerFrame = 0;
private:

    float lastReshapeTime = 0;
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> lastTime = std::chrono::high_resolution_clock::now();
    std::chrono::time_point<std::chrono::_V2::system_clock, std::chrono::duration<long, std::ratio<1, 1000000000>>> currentTime = std::chrono::high_resolution_clock::now();

    int objectIdCounter;
    static GlobalManager *instance;
    //The order of the objects is descending, so what will be rendered first is last
    std::vector<GameObject *> objects;

    GlobalManager();

    void addObjectToList(GameObject *object);

    GameObject *cleanUpObjects();

};

#endif
