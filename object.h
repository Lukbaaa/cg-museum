#ifndef OBJECT_H
#define OBJECT_H

#include <assert.h>
#include <GL/glew.h>

#include "shader.h"
#include "light.h"
#include "materials.h"

typedef struct Transform {
    GLfloat position[3];
    GLfloat scale[3];
    GLfloat rotation[3];
} Transform;

typedef struct Object {
    GLuint vao;
    int vertCount;
    Transform transform;
    Material material;
    GLuint* textures;
    int textureCount;
    Shader* shader;
    int shouldRender;
    Object** parents;
    int parentCount;
    Object** children;
    int childrenCount;
    LightSource* light;
    void (*draw)(Object*, GLfloat[16]);
    void (*animate)(Object*);
} Object;

void setPosition(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);

    obj->transform.position[0] = x;
    obj->transform.position[1] = y;
    obj->transform.position[2] = z;
}

void setScale(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    obj->transform.scale[0] = x;
    obj->transform.scale[1] = y;
    obj->transform.scale[2] = y;
}

void setRotation(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    obj->transform.rotation[0] = x;
    obj->transform.rotation[1] = y;
    obj->transform.rotation[2] = z;
}

void initObject(Object* obj) {
    assert(obj != NULL);
    obj->vao = 0;
    obj->vertCount = 0;
    setPosition(obj, 0,0,0);
    setRotation(obj, 0,0,0);
    setScale(obj, 1,1,1);
    obj->textures = NULL;
    obj->textureCount = 0;
    obj->shader = NULL;
    obj->shouldRender = 1;
    obj->parents = NULL;
    obj->children = NULL;
    obj->parentCount = 0;
    obj->childrenCount = 0;
    obj->light = NULL;
    obj->animate = NULL;
}
#endif