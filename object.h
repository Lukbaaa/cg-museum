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

/**
 * Ein Objekt, das im Raum platziert werden kann
 */
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

/**
 * @param list - die Liste, in die das Objekt eingefügt werden soll
 * @param object - das Objekt, das eingefügt werden soll
 */
void objectHardListAdd(ObjectHardList* list, Object object) {
    list->objects = (Object*)realloc(list->objects, sizeof(Object)*((list->length)+1));
    (list->objects)[list->length] = object;
    (list->length)++;
}

/**
 * @param obj - das Objekt, das initialisiert werden soll
 */
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

/**
 * @param obj - das Objekt, von welchem die Position geändert werden soll
 * @param x - die x-Koordinate der neuen Position
 * @param y - die y-Koordinate der neuen Position
 * @param z - die z-Koordinate der neuen Position
 */
void setObjectPosition(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    setPosition(&obj->transform, x, y, z);
}

/**
 * @param obj - das Objekt, von welchem die Skalierung geändert werden soll
 * @param x - die x-Koordinate der neuen Skalierung
 * @param y - die y-Koordinate der neuen Skalierung
 * @param z - die z-Koordinate der neuen Skalierung
 
 */
void setObjectScale(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    setScale(&obj->transform, x, y, z);
}

/**
 * @param obj - das Objekt, von welchem die Rotation geändert werden soll
 * @param x - die x-Koordinate der neuen Rotation
 * @param y - die y-Koordinate der neuen Rotation
 * @param z - die z-Koordinate der neuen Rotation
 
 */
void setObjectRotation(Object* obj, GLfloat x, GLfloat y, GLfloat z) {
    assert(obj != NULL);
    setRotation(&obj->transform, x, y, z);
}

/**
 * @param in - die Matrix, die die Transformation enthält
 * @param v - der Vektor, der transformiert werden soll
 */
Vec3 getGlobalPosition(GLfloat in[16], Vec3 v) {
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

/**
 * @param obj - das Objekt, von welchem die globale Position berechnet werden soll
 * @param light - die Lichtquelle, von welcher das Objekt beleuchtet wird
 */
void addLightAffectedBy(Object* obj, LightSource* light) {
  lightSourceListAdd(&obj->lightsAffectedBy, light);
}

#endif