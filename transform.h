#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <assert.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

typedef struct Transform {
    Vec3 position;
    Vec3 scale;
    Vec3 rotation;
} Transform;

void setPosition(Transform* transform, GLfloat x, GLfloat y, GLfloat z) {
    assert(transform != NULL);

    transform->position.x = x;
    transform->position.y = y;
    transform->position.z = z;
}

void setScale(Transform* transform, GLfloat x, GLfloat y, GLfloat z) {
    assert(transform != NULL);
    transform->scale.x = x;
    transform->scale.y = y;
    transform->scale.z = y;
}

void setRotation(Transform* transform, GLfloat x, GLfloat y, GLfloat z) {
    assert(transform != NULL);
    transform->rotation.x = x;
    transform->rotation.y = y;
    transform->rotation.z = z;
}

void createModelFromTransform(GLfloat* model, Transform transform) {
  GLfloat t[16];
  GLfloat s[16];

  createTransMatVec3(t, transform.position);
  createScaleMatVec3(s, transform.scale);
  identity(model);
  translate(model, model, t);
  rotate(model, model, transform.rotation);
  scale(model, model, s);
}
#endif