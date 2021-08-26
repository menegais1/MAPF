//
// Created by menegais1 on 02/08/2020.
//

#ifndef RENDERERENGINE_TEXTURE2D_H
#define RENDERERENGINE_TEXTURE2D_H


#include <glad/glad.h>

class Texture2D {
public:
    unsigned int texture;
    int width, height;
    void activateTexture(int textureUnit);
    Texture2D(GLenum glFormat, GLenum srcFormat, GLenum srcType, void *data, int width, int height);

    Texture2D();
};


#endif //RENDERERENGINE_TEXTURE2D_H
