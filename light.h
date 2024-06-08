#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

#include <stdlib.h>
#include <GL/glew.h>

typedef struct LightParams {
    float position[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
} LightSource;

LightSource* createLight(void) {
    return (LightSource*)malloc(sizeof(LightSource));
}

#endif