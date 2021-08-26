//
// Created by menegais on 14/11/2020.
//

#ifndef RENDERERENGINE_CALLBACKS_H
#define RENDERERENGINE_CALLBACKS_H

namespace Callbacks{

void render();
void mouseMovement(double x, double y);
void keyboard(int key, int scancode, int action, int mods);
void mouseButton(int button, int action, int modifier);

}
#endif //RENDERERENGINE_CALLBACKS_H
