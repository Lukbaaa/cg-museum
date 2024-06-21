#ifndef LIGHT_H
#define LIGHT_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

#include "vector.h"

#include <stdlib.h>
#include <GL/glew.h>

typedef struct LightSource {
    Vec3 position;
    Vec4 ambient;
    Vec4 diffuse;
    Vec4 specular;
} LightSource;

LightSource* createLight(void) {
    return (LightSource*)malloc(sizeof(LightSource));
}

#endif