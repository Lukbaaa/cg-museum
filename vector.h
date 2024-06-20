#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <GL/glew.h>

typedef struct Vec2 {
    GLfloat x;
    GLfloat y;
} Vec2;

typedef struct Vec3 {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} Vec3;

typedef struct Vec4 {
    GLfloat r;
    GLfloat g;
    GLfloat b;
    GLfloat a;
} Vec4;

void vec2ToArr(GLfloat out[2], Vec2 vec) {
    out[0] = vec.x;
    out[1] = vec.y;
}

void vec3ToArr(GLfloat out[3], Vec3 vec) {
    out[0] = vec.x;
    out[1] = vec.y;
    out[2] = vec.z;
}
void vec4ToArr(GLfloat out[4], Vec4 vec) {
    out[0] = vec.r;
    out[1] = vec.g;
    out[2] = vec.b;
    out[3] = vec.a;
}

void printVec3(Vec3 vec) {
  for (int i = 0; i < 3; i++) {
    printf("%f ", *(((float*)&(vec.x))+i));
  }
  printf("\n");
}

void printVec4(Vec4 vec) {
  for (int i = 0; i < 4; i++) {
    printf("%f ", *(((float*)&(vec.r))+i));
  }
  printf("\n");
}

GLfloat dotProduct(Vec3 vec1, Vec3 vec2) {
  return vec1.x*vec2.x+vec1.y*vec2.y+vec1.z*vec2.z;
}

Vec3 crossProduct(Vec3 v1, Vec3 v2) {
  Vec3 cross;
  cross.x = v1.y*v2.z - v1.z*v2.y;
  cross.y = v1.z*v2.x - v1.x*v2.z;
  cross.z = v1.x*v2.y - v1.y*v2.x;
  return cross;
}

float length2d(Vec2 vec) {
  return sqrt(vec.x*vec.x+vec.y*vec.y);
}

float length3d(Vec3 vec) {
  return sqrt(vec.x*vec.x+vec.y*vec.y+vec.z*vec.z);
}

float length2f(float x, float y) {
  return sqrt(x*x+y*y);
}

void normalize(Vec3* vec) {
  assert(vec != NULL);

  GLfloat length = length3d(*vec); 
  GLfloat invLen = 1/length;

  vec->x *= invLen;
  vec->y *= invLen;
  vec->z *= invLen;
}

#endif