//
// Created by menegais1 on 02/08/2020.
//

#include "Texture2D.h"

Texture2D::Texture2D(GLenum glFormat, GLenum srcFormat, GLenum srcType, void *data, int width,
                     int height) {
    this->width = width;
    this->height = height;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, glFormat, width, height, 0, srcFormat, srcType, data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture2D::Texture2D() {

}

void Texture2D::activateTexture(int textureUnit) {
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D, texture);
}
