#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "object.h"


void loadTexture(Object* obj, const char* textureFilePath, int shaderSlot) {
    assert(obj != NULL);
    assert(textureFilePath != NULL);
    assert(shaderSlot >= 0);

    if (shaderSlot > obj->textureCount-1) {
        obj->textures = (GLuint*)realloc(obj->textures, sizeof(GLuint)*(shaderSlot+1));
        obj->textureCount = shaderSlot+1;
    }

    stbi_set_flip_vertically_on_load(1);  
    int width, height, nrChannels;
    unsigned char *data = stbi_load(textureFilePath, &width, &height, &nrChannels, 4); 
    if (data == NULL) {
        printf("Could not load texture %s\n", textureFilePath);
    }
    glGenTextures(1, &(obj->textures[shaderSlot]));  
    glBindTexture(GL_TEXTURE_2D, obj->textures[shaderSlot]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data); 
}

#endif