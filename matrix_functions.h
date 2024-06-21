#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif
#include "vector.h"

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <GL/glew.h>

//#define M_PI 3.14159265358979323846

void copyMat(GLfloat* out, GLfloat* in, int n) {
  assert(out != NULL);
  assert(in != NULL);
  for (int i = 0; i < n; i++) { out[i] = in[i]; }
}

void printMat4(GLfloat mat[16], int transpose) { 
  assert(mat != NULL);
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (transpose) {
        printf("%f ",mat[j*4+i]);
      } else {
        printf("%f ",mat[i*4+j]);
      }
    }
    printf("\n");
  }
}

void identity(GLfloat* out) {
  assert(out != NULL);
  for (int i = 0; i < 16; i++) {
    if (i % 5 == 0) {
      out[i] = 1;
    } else {
      out[i] = 0;
    }
  }
}

void mat4Multiplication(GLfloat* out, GLfloat in[16], GLfloat v[16]) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  GLfloat temp[16];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i*4+j] = 0;
      for (int k = 0; k < 4; k++) {
        temp[i*4+j] += in[k*4+j] * v[i*4+k];
      }
    }
  }
  copyMat(out, temp, 16);
}

void translate(GLfloat* out, GLfloat in[16], GLfloat v[16]) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  mat4Multiplication(out, in, v);
}

void createTransMatVec3(GLfloat* out, Vec3 vec) {
  assert(out != NULL);

  identity(out);
  out[12] = vec.x;
  out[13] = vec.y;
  out[14] = vec.z;
}

void createTransMat3f(GLfloat* out, GLfloat x, GLfloat y, GLfloat z) {
  assert(out != NULL);

  identity(out);
  out[12] = x;
  out[13] = y;
  out[14] = z;
}

void scale(GLfloat* out, GLfloat in[16], GLfloat v[16]) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  mat4Multiplication(out, in, v);
}

void createScaleMatVec3(GLfloat* out, Vec3 vec) {
  assert(out != NULL);

  identity(out);
  out[0]  = vec.x;
  out[5]  = vec.y;
  out[10] = vec.z;
}

void createScaleMat3f(GLfloat* out, GLfloat x, GLfloat y, GLfloat z) {
  assert(out != NULL);

  identity(out);
  out[0]  = x;
  out[5]  = y;
  out[10] = z;
}

void rotatex(GLfloat* out, GLfloat in[16], double a) {
  assert(out != NULL);
  assert(in != NULL);

  a = a * M_PI/180;
  GLfloat rotMat[16] = {
    1,        0,       0, 0,
    0,  cosf(a), sinf(a), 0,
    0, -sinf(a), cosf(a), 0, 
    0,        0,       0, 1
  };
  mat4Multiplication(out, in, rotMat);
}

void rotatey(GLfloat* out, GLfloat in[16], double a) {
  assert(out != NULL);
  assert(in != NULL);

  a = a * M_PI/180;
  GLfloat rotMat[16] = {
    cosf(a), 0, -sinf(a), 0,
          0, 1,        0, 0,
    sinf(a), 0,  cosf(a), 0,
          0, 0,        0, 1
  };
  mat4Multiplication(out, in, rotMat);
}

void rotatez(GLfloat* out, GLfloat in[16], double a) {
  assert(out != NULL);
  assert(in != NULL);

  a = a * M_PI/180;
  GLfloat rotMat[16] = {
     cosf(a), sinf(a), 0, 0,
    -sinf(a), cosf(a), 0, 0,
          0,        0, 1, 0,
          0,        0, 0, 1
  };
  mat4Multiplication(out, in, rotMat);
}

void rotate(GLfloat* out, GLfloat in[16], Vec3 rotation) {
  rotatex(out, in, rotation.x);
  rotatey(out, in, rotation.y);
  rotatez(out, in, rotation.z);
}

void transpose4(GLfloat out[16], GLfloat in[16]) {
  assert(out != NULL);
  assert(in != NULL);

  GLfloat temp[16];
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[j * 4 + i] = in[i * 4 + j];
    }
  }
  copyMat(out, temp, 16);
}

int inverse4(GLfloat out[16], GLfloat in[16]) {
  assert(in != NULL);
  assert(out != NULL);
  double inv[16], det;
  int i;

  inv[0] = in[5]  * in[10] * in[15] - 
            in[5]  * in[11] * in[14] - 
            in[9]  * in[6]  * in[15] + 
            in[9]  * in[7]  * in[14] +
            in[13] * in[6]  * in[11] - 
            in[13] * in[7]  * in[10];

  inv[4] = -in[4]  * in[10] * in[15] + 
            in[4]  * in[11] * in[14] + 
            in[8]  * in[6]  * in[15] - 
            in[8]  * in[7]  * in[14] - 
            in[12] * in[6]  * in[11] + 
            in[12] * in[7]  * in[10];

  inv[8] = in[4]  * in[9] * in[15] - 
            in[4]  * in[11] * in[13] - 
            in[8]  * in[5] * in[15] + 
            in[8]  * in[7] * in[13] + 
            in[12] * in[5] * in[11] - 
            in[12] * in[7] * in[9];

  inv[12] = -in[4]  * in[9] * in[14] + 
              in[4]  * in[10] * in[13] +
              in[8]  * in[5] * in[14] - 
              in[8]  * in[6] * in[13] - 
              in[12] * in[5] * in[10] + 
              in[12] * in[6] * in[9];

  inv[1] = -in[1]  * in[10] * in[15] + 
            in[1]  * in[11] * in[14] + 
            in[9]  * in[2] * in[15] - 
            in[9]  * in[3] * in[14] - 
            in[13] * in[2] * in[11] + 
            in[13] * in[3] * in[10];

  inv[5] = in[0]  * in[10] * in[15] - 
            in[0]  * in[11] * in[14] - 
            in[8]  * in[2] * in[15] + 
            in[8]  * in[3] * in[14] + 
            in[12] * in[2] * in[11] - 
            in[12] * in[3] * in[10];

  inv[9] = -in[0]  * in[9] * in[15] + 
            in[0]  * in[11] * in[13] + 
            in[8]  * in[1] * in[15] - 
            in[8]  * in[3] * in[13] - 
            in[12] * in[1] * in[11] + 
            in[12] * in[3] * in[9];

  inv[13] = in[0]  * in[9] * in[14] - 
            in[0]  * in[10] * in[13] - 
            in[8]  * in[1] * in[14] + 
            in[8]  * in[2] * in[13] + 
            in[12] * in[1] * in[10] - 
            in[12] * in[2] * in[9];

  inv[2] = in[1]  * in[6] * in[15] - 
            in[1]  * in[7] * in[14] - 
            in[5]  * in[2] * in[15] + 
            in[5]  * in[3] * in[14] + 
            in[13] * in[2] * in[7] - 
            in[13] * in[3] * in[6];

  inv[6] = -in[0]  * in[6] * in[15] + 
            in[0]  * in[7] * in[14] + 
            in[4]  * in[2] * in[15] - 
            in[4]  * in[3] * in[14] - 
            in[12] * in[2] * in[7] + 
            in[12] * in[3] * in[6];

  inv[10] = in[0]  * in[5] * in[15] - 
            in[0]  * in[7] * in[13] - 
            in[4]  * in[1] * in[15] + 
            in[4]  * in[3] * in[13] + 
            in[12] * in[1] * in[7] - 
            in[12] * in[3] * in[5];

  inv[14] = -in[0]  * in[5] * in[14] + 
              in[0]  * in[6] * in[13] + 
              in[4]  * in[1] * in[14] - 
              in[4]  * in[2] * in[13] - 
              in[12] * in[1] * in[6] + 
              in[12] * in[2] * in[5];

  inv[3] = -in[1] * in[6] * in[11] + 
            in[1] * in[7] * in[10] + 
            in[5] * in[2] * in[11] - 
            in[5] * in[3] * in[10] - 
            in[9] * in[2] * in[7] + 
            in[9] * in[3] * in[6];

  inv[7] = in[0] * in[6] * in[11] - 
            in[0] * in[7] * in[10] - 
            in[4] * in[2] * in[11] + 
            in[4] * in[3] * in[10] + 
            in[8] * in[2] * in[7] - 
            in[8] * in[3] * in[6];

  inv[11] = -in[0] * in[5] * in[11] + 
              in[0] * in[7] * in[9] + 
              in[4] * in[1] * in[11] - 
              in[4] * in[3] * in[9] - 
              in[8] * in[1] * in[7] + 
              in[8] * in[3] * in[5];

  inv[15] = in[0] * in[5] * in[10] - 
            in[0] * in[6] * in[9] - 
            in[4] * in[1] * in[10] + 
            in[4] * in[2] * in[9] + 
            in[8] * in[1] * in[6] - 
            in[8] * in[2] * in[5];

  det = in[0] * inv[0] + in[1] * inv[4] + in[2] * inv[8] + in[3] * inv[12];

  if (det == 0)
      return 0;

  det = 1.0 / det;

  for (i = 0; i < 16; i++)
      out[i] = inv[i] * det;

  return 1;
}

#endif