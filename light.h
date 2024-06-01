#ifndef LIGHT_H
#define LIGHT_H

#include <stdlib.h>
#include <GL/glew.h>

typedef struct LightParams {
    GLfloat position[4];
    GLfloat ambient[4];
    GLfloat diffuse[4];
    GLfloat specular[4];
} LightSource;

LightSource* createLight(void) {
    return (LightSource*)malloc(sizeof(LightSource));
}

#endif