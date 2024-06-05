#ifndef OBJECT_H
#define OBJECT_H

#include <assert.h>
#include <GL/glew.h>

#include "light.h"
#include "shader.h"
#include "camera.h"
#include "materials.h"
#include "transform.h"

typedef struct Object {
    GLuint vao;
    int vertCount;
    Transform transform;
    GLfloat globalPosition[4];
    GLfloat model[16];
    Material material;
    GLuint* textures;
    int textureCount;
    Shader* shader;
    int shouldRender;
    int isTransparent;
    Object** parents;
    int parentCount;
    Object** children;
    int childrenCount;
    LightSource* light;
    Camera* camera;
    void (*draw)(Object*);
    void (*animate)(Object*);
} Object;

void initObject(Object* obj) {
    assert(obj != NULL);
    obj->vao = 0;
    obj->vertCount = 0;
    setPosition(&obj->transform, 0,0,0);
    setRotation(&obj->transform, 0,0,0);
    setScale(&obj->transform, 1,1,1);
    identity(obj->model);
    obj->textures = NULL;
    obj->textureCount = 0;
    obj->shader = NULL;
    obj->shouldRender = 1;
    obj->isTransparent = 0;
    obj->parents = NULL;
    obj->children = NULL;
    obj->parentCount = 0;
    obj->childrenCount = 0;
    obj->light = NULL;
    obj->camera = NULL;
    obj->draw = NULL;
    obj->animate = NULL;
}

void setObjectPosition(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    setPosition(&obj->transform, x, y, z);
}

void setObjectScale(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    setScale(&obj->transform, x, y, z);
}

void setObjectRotation(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    setRotation(&obj->transform, x, y, z);
}
#endif