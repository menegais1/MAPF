//
// Created by menegais on 25/08/2021.
//

#include "GameObject.h"
#include "GlobalManager.h"
#include <iostream>


void GameObject::setActive(bool isActive) {
    lastActiveState = this->isActive;
    this->isActive = isActive;
}

bool GameObject::getActive() {
    return this->isActive;
}

bool GameObject::checkIfCanExecuteCallback() {
    if (!isValid) return false;
    if (lastActiveState != isActive) {
        bool tmp = lastActiveState;
        lastActiveState = isActive;
        return tmp;
    }
    return this->isActive;
}

void GameObject::setObjectOrder(int zIndex) {
    this->objectOrder = zIndex;
    GlobalManager::getInstance()->changeObjectOrder(this);
}

int GameObject::getObjectOrder() {
    return objectOrder;
}

GameObject::GameObject() {
    objectOrder = 0;
    isValid = true;
    isActive = true;
    lastActiveState = false;
    objectId = GlobalManager::getInstance()->registerObject(this);
}

GameObject::~GameObject() {
    GlobalManager::getInstance()->unregisterObject(this);
}

void GameObject::keyboard(int key, int scancode, int action, int mods) {

}

void GameObject::mouseButton(int button, int action, int modifier) {

}

void GameObject::mouseMovement(double xpos, double ypos) {

}

void GameObject::render() {
}


void GameObject::update() {
}