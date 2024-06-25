#ifndef OBJECTSTRUCT_H
#define OBJECTSTRUCT_H

#include <GL/glew.h>

#include "transform.h"
#include "vector.h"
#include "materials.h"
#include "list.h"
#include "shader.h"
#include "camera.h"

typedef struct BoundingBox {
    Vec3 topleftfront;
    Vec3 toprightfront;
    Vec3 bottomleftfront;
    Vec3 bottomrightfront;
    Vec3 topleftback;
    Vec3 toprightback;
    Vec3 bottomleftback;
    Vec3 bottomrightback;
} BoundingBox;

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
    ObjectPointerList parents;
    ObjectPointerList children;
    LightSource* light;
    LightSourceList lightsAffectedBy;
    Camera* camera;
    void (*draw)(Object*);
    void (*animate)(Object*);
} Object;


#endif