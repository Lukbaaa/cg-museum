#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <assert.h>
#include <GL/glew.h>

typedef struct Transform {
    GLfloat position[3];
    GLfloat scale[3];
    GLfloat rotation[3];
} Transform;

void setPosition(Transform* transform, GLfloat x, GLfloat y, GLfloat z) {
    assert(transform != NULL);

    transform->position[0] = x;
    transform->position[1] = y;
    transform->position[2] = z;
}

void setScale(Transform* transform, GLfloat x, GLfloat y, GLfloat z) {
    assert(transform != NULL);
    transform->scale[0] = x;
    transform->scale[1] = y;
    transform->scale[2] = y;
}

void setRotation(Transform* transform, GLfloat x, GLfloat y, GLfloat z) {
    assert(transform != NULL);
    transform->rotation[0] = x;
    transform->rotation[1] = y;
    transform->rotation[2] = z;
}

void createModelFromTransform(GLfloat* model, Transform transform) {
  GLfloat t[16];
  GLfloat s[16];

  createTransMatFP(t, transform.position);
  createScaleMatFP(s, transform.scale);
  identity(model);
  translate(model, model, t);
  rotate(model, model, transform.rotation);
  scale(model, model, s);
}
#endif