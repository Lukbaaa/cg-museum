#ifndef TRANSFORMATIONS_H
#define TRANSFORMATIONS_H

#include <math.h>
#include <stdio.h>
#include <GL/glew.h>

#define M_PI 3.14159265358979323846

void copyMat(GLfloat* mat1, GLfloat* mat2) {
  for (int i = 0; i < 16; i++) { mat1[i] = mat2[i]; }
}

void printMat4(GLfloat* mat, int transpose) {
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
  for (int i = 0; i < 3; i++) {
    printf("%f ", vec[i]);
  }
  printf("\n");
}

void identity(GLfloat* out) {
  for (int i = 0; i < 16; i++) {
    if (i % 5 == 0) {
      out[i] = 1;
    } else {
      out[i] = 0;
    }
  }
}

void mat4Multiplication(GLfloat* out, GLfloat* in, GLfloat* v) {
  GLfloat temp[16];

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      temp[i*4+j] = 0;
      for (int k = 0; k < 4; k++) {
        temp[i*4+j] += in[k*4+j] * v[i*4+k];
      }
    }
  }
  copyMat(out, temp);
}

GLfloat dotProduct(GLfloat* vec1, GLfloat* vec2) {
  return vec1[0]*vec2[0]+vec1[1]*vec2[1]+vec1[2]*vec2[2];
}

void crossProduct(GLfloat* out, GLfloat* in, GLfloat* v) {
  out[0] = in[1]*v[2] - in[2]*v[1];
  out[1] = in[2]*v[0] - in[0]*v[2];
  out[2] = in[0]*v[1] - in[1]*v[0];
}

void normalize(GLfloat* vec) {
  GLfloat length = sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
  GLfloat invLen = 1/length;

  vec[0] *= invLen;
  vec[1] *= invLen;
  vec[2] *= invLen;
}

void translate(GLfloat* out, GLfloat* in, GLfloat* v) {
  mat4Multiplication(out, in, v);
}

void createTransMatFP(GLfloat* out, GLfloat vec[3]) {
  GLfloat temp[16];
  identity(temp);
  temp[12]  = vec[0];
  temp[13]  = vec[1];
  temp[14] = vec[2];
  copyMat(out, temp);
}

void createTransMat3f(GLfloat* out, GLfloat x, GLfloat y, GLfloat z) {
  GLfloat temp[16];
  identity(temp);
  temp[12] = x;
  temp[13] = y;
  temp[14] = z;
  copyMat(out, temp);
}

void scale(GLfloat* out, GLfloat* in, GLfloat* v) {
  mat4Multiplication(out, in, v);
}

void createScaleMatFP(GLfloat* out, GLfloat vec[3]) {
  GLfloat temp[16];
  identity(temp);
  temp[0]  = vec[0];
  temp[5]  = vec[1];
  temp[10] = vec[2];
  copyMat(out, temp);
}

void createScaleMat3f(GLfloat* out, GLfloat x, GLfloat y, GLfloat z) {
  GLfloat temp[16];
  identity(temp);
  temp[0]  = x;
  temp[5]  = y;
  temp[10] = z;
  copyMat(out, temp);
}

void rotatex(GLfloat* out, GLfloat* in, double a) {
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
  a = a * M_PI/180;
  GLfloat rotMat[16] = {
     cosf(a), sinf(a), 0, 0,
    -sinf(a), cosf(a), 0, 0,
          0,        0, 1, 0,
          0,        0, 0, 1
  };
  mat4Multiplication(out, in, rotMat);
}

void transpose4(GLfloat in[16], GLfloat out[16]) {
  GLfloat temp[16];
  for (int i = 0; i < 4; ++i) {
    for (int j = 0; j < 4; ++j) {
      temp[j * 4 + i] = in[i * 4 + j];
    }
  }
  copyMat(out, temp);
}

int inverse4(GLfloat m[16], GLfloat out[16])
{
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

#endif