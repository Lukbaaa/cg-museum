#ifndef OBJECT_H
#define OBJECT_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

#include <assert.h>
#include <GL/glew.h>

#include "list.h"
#include "light.h"
#include "shader.h"
#include "camera.h"
#include "materials.h"
#include "transform.h"
#include "boundingbox.h"


typedef struct Object {
    GLuint vao;
    int vertCount;
    Transform transform;
    Vec3 globalPosition;
    BoundingBox boundingBox;
    GLuint boundingBoxVao;
    GLfloat model[16];
    Material material;
    GLuint* textures;
    int textureCount;
    Shader* shader;
    GLuint renderTarget;
    int shouldRender;
    int isTransparent;
    ObjectList parents;
    ObjectList children;
    LightSource* light;
    LightSourceList lightsAffectedBy;
    Camera* camera;
    void (*draw)(Object*);
    void (*animate)(Object*);
} Object;

typedef struct ObjectHardList {
    Object* objects;
    int length;
} ObjectHardList;

void initObjectHardList(ObjectHardList* list) {
    list->objects = NULL;
    list->length=0;
}

void objectHardListAdd(ObjectHardList* list, Object object) {
    list->objects = (Object*)realloc(list->objects, sizeof(Object)*((list->length)+1));
    (list->objects)[list->length] = object;
    (list->length)++;
}

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
    obj->renderTarget = 0;
    obj->shouldRender = 1;
    obj->isTransparent = 0;
    initObjectList(&obj->parents);
    initObjectList(&obj->children);
    obj->light = NULL;
    initLightSourceList(&obj->lightsAffectedBy);
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

Vec3 getGlobalPosition(GLfloat in[16], Vec3 vec) {
    assert(in != NULL);

    Vec3 globalPosition;
    
    GLfloat x = 0;//vec.x;
    GLfloat y = 0;//vec.y;
    GLfloat z = 0;//vec.z;
    GLfloat w = 1.0f;

    globalPosition.x = in[0] * x + in[4] * y + in[8] * z + in[12] * w;
    globalPosition.y = in[1] * x + in[5] * y + in[9] * z + in[13] * w;
    globalPosition.z = in[2] * x + in[6] * y + in[10] * z + in[14] * w;

    return globalPosition;
}

void addLightAffectedBy(Object* obj, LightSource* light) {
  lightSourceListAdd(&obj->lightsAffectedBy, light);
}

#endif