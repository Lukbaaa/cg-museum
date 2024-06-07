#ifndef LIGHT_H
#define LIGHT_H

#include "vector.h"

#include <stdlib.h>
#include <GL/glew.h>

typedef struct LightParams {
    Vec4 position;
    Vec4 ambient;
    Vec4 diffuse;
    Vec4 specular;
} LightSource;

LightSource* createLight(void) {
    return (LightSource*)malloc(sizeof(LightSource));
}

#endif