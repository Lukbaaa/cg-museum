#ifndef TEXTURE_H
#define TEXTURE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

#include <GL/glew.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "object.h"

typedef GLuint Texture;

Texture loadTexture(Object* obj, const char* textureFilePath, int shaderSlot) {
    assert(obj != NULL);
    assert(shaderSlot >= 0);

    if (shaderSlot > obj->textureCount-1) {
        obj->textures = (GLuint*)realloc(obj->textures, sizeof(GLuint)*(shaderSlot+1));
        obj->textureCount = shaderSlot+1;
    }

    int width = 800, height = 800;
    stbi_set_flip_vertically_on_load(1);  
    int nrChannels;
    unsigned char* data = NULL;
    if (textureFilePath != NULL) {
        data = stbi_load(textureFilePath, &width, &height, &nrChannels, 4); 
        if (data == NULL) {
            printf("Could not load texture %s\n", textureFilePath);
        }
    } 
    glGenTextures(1, &(obj->textures[shaderSlot]));  
    glBindTexture(GL_TEXTURE_2D, obj->textures[shaderSlot]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data); 

    return obj->textures[shaderSlot];
}

#endif