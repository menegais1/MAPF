
#include "GlobalManager.h"
#include <vector>
#include "GameObject.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>

using namespace std::chrono;

GlobalManager::GlobalManager() {
    screenWidth = 500;
    screenHeight = 500;
}

GlobalManager *GlobalManager::getInstance() {
    static GlobalManager *instance = new GlobalManager();
    return instance;
}

void GlobalManager::changeObjectOrder(GameObject *object) {
    auto iterator = std::find(objects.begin(), objects.end(), object);
    if (iterator != objects.cend()) {
        objects.erase(iterator);
        addObjectToList(object);
    }
}


void GlobalManager::render() {
    currentTime = high_resolution_clock::now();
    duration<double> time_span = duration_cast<duration<double>>(currentTime - lastTime);
    deltaTime = time_span.count();
    time += deltaTime;
    if (time > lastFpsUpdate + fpsUpdateCycle) {
        fps = 1.0 / deltaTime;
        std::cout<< "FPS:" << fps << std::endl;

        lastFpsUpdate = time;
    }
    std::vector<GameObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback())
            continue;
        callbackCaller[i]->update();
        callbackCaller[i]->render();
        drawCallsPerFrame++;
    }

    std::cout<< "DrawCalls:" << drawCallsPerFrame << std::endl;
    drawCallsPerFrame = 0;
    cleanUpObjects();

//    if (time - lastReshapeTime >= deltaTime) {
//        bool lastRendererState = Renderer::getInstance()->isActive;
//        Renderer::getInstance()->isActive = false;
//        lastReshapeTime = 10000000;
//        Renderer::getInstance()->setScreenProportions(this->screenWidth, this->screenHeight);
//        Renderer::getInstance()->isActive = lastRendererState;
//    }

    lastTime = currentTime;
}

int GlobalManager::registerObject(GameObject *object) {
    addObjectToList(object);
    return objectIdCounter++;
}

void GlobalManager::addObjectToList(GameObject *object) {
    int size = objects.size();
    if (size == 0) {
        objects.push_back(object);
    } else {
        for (int i = 0; i < size; i++) {
            if (objects[i]->getObjectOrder() <= object->getObjectOrder()) {
                objects.insert(objects.begin() + i, object);
                break;
            }
        }
        if (size == objects.size())
            objects.push_back(object);
    }
}

GameObject *GlobalManager::unregisterObject(GameObject *object) {
    auto iterator = std::find(objects.begin(), objects.end(), object);
    if (iterator != objects.cend()) {
        objects.erase(iterator);
        return object;
    }
    return nullptr;
}

GameObject *GlobalManager::deleteObject(GameObject *object) {
    object->isValid = false;
    return nullptr;
}

GameObject *GlobalManager::cleanUpObjects() {
    for (int i = 0; i < objects.size(); ++i) {
        if (!objects[i]->isValid) {
            auto object = objects[i];
            unregisterObject(object);
            delete object;
            i--;
        }
    }
}

void GlobalManager::reshape(int width, int height) {
    this->screenWidth = width;
    this->screenHeight = height;

    float aspectRatio = width / (float) height;
//    Camera::getInstance()->setViewport(width, height, 0, 0);
//    if (Camera::getInstance()->cameraType == CameraType::Orthographic) {
//        Camera::getInstance()->generateOrtographicProjectionMatrix(Camera::getInstance()->getWidth(),
//                                                                   Camera::getInstance()->getHeight(), aspectRatio,
//                                                                   Camera::getInstance()->getNear(),
//                                                                   Camera::getInstance()->getFar());
//    } else {
//        Camera::getInstance()->generateProjectionMatrix(Camera::getInstance()->getFov(), aspectRatio,
//                                                        Camera::getInstance()->getNear(),
//                                                        Camera::getInstance()->getFar());
//    }
    lastReshapeTime = time;
}

void GlobalManager::keyboard(int key, int scancode, int action, int mods) {
    std::vector<GameObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback())
            continue;
        callbackCaller[i]->keyboard(key,scancode,action,mods);
    }
}

void GlobalManager::mouseButton(int button, int action, int modifier) {
    std::vector<GameObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback())
            continue;
        callbackCaller[i]->mouseButton(button, action, modifier);
    }
}

void GlobalManager::mouseMovement(double xpos, double ypos) {
    mousePosition = glm::vec2(xpos, screenHeight - ypos);
    std::vector<GameObject *> callbackCaller = objects;
    for (int i = callbackCaller.size() - 1; i >= 0; i--) {
        if (!callbackCaller[i]->checkIfCanExecuteCallback())
            continue;
        callbackCaller[i]->mouseMovement(mousePosition.x, mousePosition.y);
    }
}
