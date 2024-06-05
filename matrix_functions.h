#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <GL/glew.h>

#define M_PI 3.14159265358979323846

void copyMat(GLfloat* out, GLfloat* in, int n) {
  assert(out != NULL);
  assert(in != NULL);
  for (int i = 0; i < n; i++) { out[i] = in[i]; }
}

void printMat4(GLfloat* mat, int transpose) { 
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
  printf("\n");
}

void printVec3(GLfloat* vec) {
  assert(vec != NULL);
  for (int i = 0; i < 3; i++) {
    printf("%f ", vec[i]);
  }
  printf("\n");
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

void mat4Multiplication(GLfloat* out, GLfloat* in, GLfloat* v) {
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

void mat4VectorMultiplication(GLfloat* out, GLfloat* in, GLfloat* v) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  GLfloat temp[4];
  GLfloat v4[4];

  v4[0] = v[0];
  v4[1] = v[1];
  v4[2] = v[2];
  v4[1] = 1;

  for (int i = 0; i < 4; i++) {
      temp[i] = 0;
      for (int j = 0; j < 4; j++) {
          temp[i] += in[i*4 + j] * v4[j];
      }
  }
  copyMat(out, temp, 4);
}

GLfloat dotProduct(GLfloat* vec1, GLfloat* vec2) {
  assert(vec1 != NULL);
  assert(vec2 != NULL);

  return vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
}

void crossProduct(GLfloat* out, GLfloat* in, GLfloat* v) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  out[0] = in[1]*v[2] - in[2]*v[1];
  out[1] = in[2]*v[0] - in[0]*v[2];
  out[2] = in[0]*v[1] - in[1]*v[0];
}

void normalize(GLfloat* vec) {
  assert(vec != NULL);

  GLfloat length = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
  GLfloat invLen = 1/length;

  vec[0] *= invLen;
  vec[1] *= invLen;
  vec[2] *= invLen;
}

void translate(GLfloat* out, GLfloat* in, GLfloat* v) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  mat4Multiplication(out, in, v);
}

void createTransMatFP(GLfloat* out, GLfloat vec[3]) {
  assert(out != NULL);
  assert(vec != NULL);

  identity(out);
  out[12]  = vec[0];
  out[13]  = vec[1];
  out[14] = vec[2];
}

void createTransMat3f(GLfloat* out, GLfloat x, GLfloat y, GLfloat z) {
  assert(out != NULL);

  identity(out);
  out[12] = x;
  out[13] = y;
  out[14] = z;
}

void scale(GLfloat* out, GLfloat* in, GLfloat* v) {
  assert(out != NULL);
  assert(in != NULL);
  assert(v != NULL);

  mat4Multiplication(out, in, v);
}

void createScaleMatFP(GLfloat* out, GLfloat vec[3]) {
  assert(out != NULL);
  assert(vec != NULL);

  identity(out);
  out[0]  = vec[0];
  out[5]  = vec[1];
  out[10] = vec[2];
}

void createScaleMat3f(GLfloat* out, GLfloat x, GLfloat y, GLfloat z) {
  assert(out != NULL);

  identity(out);
  out[0]  = x;
  out[5]  = y;
  out[10] = z;
}

void rotatex(GLfloat* out, GLfloat* in, double a) {
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

void rotatey(GLfloat* out, GLfloat* in, double a) {
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

void rotatez(GLfloat* out, GLfloat* in, double a) {
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

void rotate(GLfloat* out, GLfloat* in, GLfloat rotation[3]) {
  rotatex(out, in, rotation[0]);
  rotatey(out, in, rotation[1]);
  rotatez(out, in, rotation[2]);
}

void transpose4(GLfloat in[16], GLfloat out[16]) {
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

int inverse4(GLfloat m[16], GLfloat out[16]) {
  assert(m != NULL);
  assert(out != NULL);
  double inv[16], det;
  int i;

  inv[0] = m[5]  * m[10] * m[15] - 
            m[5]  * m[11] * m[14] - 
            m[9]  * m[6]  * m[15] + 
            m[9]  * m[7]  * m[14] +
            m[13] * m[6]  * m[11] - 
            m[13] * m[7]  * m[10];

  inv[4] = -m[4]  * m[10] * m[15] + 
            m[4]  * m[11] * m[14] + 
            m[8]  * m[6]  * m[15] - 
            m[8]  * m[7]  * m[14] - 
            m[12] * m[6]  * m[11] + 
            m[12] * m[7]  * m[10];

  inv[8] = m[4]  * m[9] * m[15] - 
            m[4]  * m[11] * m[13] - 
            m[8]  * m[5] * m[15] + 
            m[8]  * m[7] * m[13] + 
            m[12] * m[5] * m[11] - 
            m[12] * m[7] * m[9];

  inv[12] = -m[4]  * m[9] * m[14] + 
              m[4]  * m[10] * m[13] +
              m[8]  * m[5] * m[14] - 
              m[8]  * m[6] * m[13] - 
              m[12] * m[5] * m[10] + 
              m[12] * m[6] * m[9];

  inv[1] = -m[1]  * m[10] * m[15] + 
            m[1]  * m[11] * m[14] + 
            m[9]  * m[2] * m[15] - 
            m[9]  * m[3] * m[14] - 
            m[13] * m[2] * m[11] + 
            m[13] * m[3] * m[10];

  inv[5] = m[0]  * m[10] * m[15] - 
            m[0]  * m[11] * m[14] - 
            m[8]  * m[2] * m[15] + 
            m[8]  * m[3] * m[14] + 
            m[12] * m[2] * m[11] - 
            m[12] * m[3] * m[10];

  inv[9] = -m[0]  * m[9] * m[15] + 
            m[0]  * m[11] * m[13] + 
            m[8]  * m[1] * m[15] - 
            m[8]  * m[3] * m[13] - 
            m[12] * m[1] * m[11] + 
            m[12] * m[3] * m[9];

  inv[13] = m[0]  * m[9] * m[14] - 
            m[0]  * m[10] * m[13] - 
            m[8]  * m[1] * m[14] + 
            m[8]  * m[2] * m[13] + 
            m[12] * m[1] * m[10] - 
            m[12] * m[2] * m[9];

  inv[2] = m[1]  * m[6] * m[15] - 
            m[1]  * m[7] * m[14] - 
            m[5]  * m[2] * m[15] + 
            m[5]  * m[3] * m[14] + 
            m[13] * m[2] * m[7] - 
            m[13] * m[3] * m[6];

  inv[6] = -m[0]  * m[6] * m[15] + 
            m[0]  * m[7] * m[14] + 
            m[4]  * m[2] * m[15] - 
            m[4]  * m[3] * m[14] - 
            m[12] * m[2] * m[7] + 
            m[12] * m[3] * m[6];

  inv[10] = m[0]  * m[5] * m[15] - 
            m[0]  * m[7] * m[13] - 
            m[4]  * m[1] * m[15] + 
            m[4]  * m[3] * m[13] + 
            m[12] * m[1] * m[7] - 
            m[12] * m[3] * m[5];

  inv[14] = -m[0]  * m[5] * m[14] + 
              m[0]  * m[6] * m[13] + 
              m[4]  * m[1] * m[14] - 
              m[4]  * m[2] * m[13] - 
              m[12] * m[1] * m[6] + 
              m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] + 
            m[1] * m[7] * m[10] + 
            m[5] * m[2] * m[11] - 
            m[5] * m[3] * m[10] - 
            m[9] * m[2] * m[7] + 
            m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] - 
            m[0] * m[7] * m[10] - 
            m[4] * m[2] * m[11] + 
            m[4] * m[3] * m[10] + 
            m[8] * m[2] * m[7] - 
            m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] + 
              m[0] * m[7] * m[9] + 
              m[4] * m[1] * m[11] - 
              m[4] * m[3] * m[9] - 
              m[8] * m[1] * m[7] + 
              m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] - 
            m[0] * m[6] * m[9] - 
            m[4] * m[1] * m[10] + 
            m[4] * m[2] * m[9] + 
            m[8] * m[1] * m[6] - 
            m[8] * m[2] * m[5];

  det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (det == 0)
      return 0;

  det = 1.0 / det;

  for (i = 0; i < 16; i++)
      out[i] = inv[i] * det;

  return 1;
}


void setVec3FP(GLfloat out[3], GLfloat in[3]) {
  out[0] = in[0];
  out[1] = in[1];
  out[2] = in[2];
}
void setVec33f(GLfloat out[3], GLfloat x, GLfloat y, GLfloat z) {
  out[0] = x;
  out[1] = y;
  out[2] = z;
}

#endif