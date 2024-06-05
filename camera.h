#ifndef CAMERA_H
#define CAMERA_H

#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "matrix_functions.h"

typedef struct Camera {
  GLfloat view[16];
  GLfloat projection[16];
  GLfloat camPos[3];
  GLfloat camFront[3];
  GLfloat camUp[3];
  GLfloat center[3];
  GLfloat yaw;	
  GLfloat pitch;
  GLfloat lastX;
  GLfloat lastY;
  GLfloat fov;
} Camera;

void initCamera(Camera* cam) {
  setVec33f(cam->camPos, 0, 0, 20);
  setVec33f(cam->camFront, 0.0f, 0.0f, -1.0f);
  setVec33f(cam->camUp, 0, 1, 0);
  cam->yaw   = -90.0f;	
  cam->pitch =  0.0f;
  cam->lastX =  800.0f / 2.0;
  cam->lastY =  600.0 / 2.0;
  cam->fov   =  45.0f;
}

Camera* createCamera() {
  Camera* cam = (Camera*)malloc(sizeof(Camera));
  initCamera(cam);
  return cam;
}

void lookAt(GLfloat*out,GLfloat*camPos,GLfloat*center,GLfloat*up) {
  GLfloat n[3] = {camPos[0]-center[0],camPos[1]-center[1],camPos[2]-center[2]};
  GLfloat u[3];
  GLfloat v[3];
  crossProduct(u, up, n);
  crossProduct(v, n, u);
  
  normalize(n);
  normalize(u);
  normalize(v);

  GLfloat tx = -dotProduct(u, camPos);
  GLfloat ty = -dotProduct(v, camPos);
  GLfloat tz = -dotProduct(n, camPos);

  GLfloat view[16] = 
    {
      u[0],v[0],n[0],0,
      u[1],v[1],n[1],0,
      u[2],v[2],n[2],0,
        tx,  ty,  tz,1
    };
  copyMat(out, view, 16);
}

void perspective(GLfloat* out, GLfloat fovy, GLfloat aspect, GLfloat near, GLfloat far) {

  fovy = fovy * M_PI/180;

  GLfloat t = near * tanf(fovy/2);
  GLfloat b = -t;
  GLfloat l = b * aspect;
  GLfloat r = t * aspect;

  GLfloat temp[16] = 
    {
      2/(r-l),       0,            (1/near)*((r+l)/(r-l)),                   0,
            0, 2/(t-b),            (1/near)*((t+b)/(t-b)),                   0,
            0,       0, (-1/near)*((far+near)/(far-near)), -(2*far)/(far-near),
            0,       0,                           -1/near,                   0
    };
  copyMat(out, temp, 16);
}

void changeView(Camera* cam) {
  cam->center[0] = cam->camPos[0] + cam->camFront[0];
  cam->center[1] = cam->camPos[1] + cam->camFront[1];
  cam->center[2] = cam->camPos[2] + cam->camFront[2];
  lookAt(cam->view, cam->camPos, cam->center, cam->camUp);
}

int firstMouse = 0;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, Camera* cam, double xposIn, double yposIn)
{
  assert(window != NULL);

  float xpos = xposIn;
  float ypos = yposIn;

  if (firstMouse)
  {
      cam->lastX = xpos;
      cam->lastY = ypos;
      firstMouse = 0;
  }

  float xoffset = xpos - cam->lastX;
  float yoffset = cam->lastY - ypos; 
  cam->lastX = xpos;
  cam->lastY = ypos;

  float sensitivity = 0.5f; 
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  cam->yaw += xoffset;
  cam->pitch += yoffset;

  if (cam->pitch > 89.0f)
      cam->pitch = 89.0f;
  if (cam->pitch < -89.0f)
      cam->pitch = -89.0f;

  cam->camFront[0] = cos(cam->yaw*M_PI/180) * cos(cam->pitch*M_PI/180);
  cam->camFront[1] = sin(cam->pitch*M_PI/180);
  cam->camFront[2] = sin(cam->yaw*M_PI/180) * cos(cam->pitch*M_PI/180);
  normalize(cam->camFront);
}

void scroll_callback(GLFWwindow* window, Camera* cam, GLfloat xoffset, GLfloat yoffset)
{
  assert(window != NULL);

  cam->fov -= (float)yoffset;
  if (cam->fov < 1.0f)
      cam->fov = 1.0f;
  if (cam->fov > 45.0f)
      cam->fov = 45.0f;
}

void processInput(GLFWwindow *window, Camera* cam)
{
  assert(window != NULL);
  
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  float cameraSpeed = 5 * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    cam->camPos[0] += cameraSpeed * cam->camFront[0];
    cam->camPos[1] += cameraSpeed * cam->camFront[1];
    cam->camPos[2] += cameraSpeed * cam->camFront[2];
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam->camPos[0] -= cameraSpeed * cam->camFront[0];
    cam->camPos[1] -= cameraSpeed * cam->camFront[1];
    cam->camPos[2] -= cameraSpeed * cam->camFront[2];
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    GLfloat temp[3];
    crossProduct(temp, cam->camFront, cam->camUp);
    normalize(temp);
    cam->camPos[0] -= temp[0] * cameraSpeed;
    cam->camPos[1] -= temp[1] * cameraSpeed;
    cam->camPos[2] -= temp[2] * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    GLfloat temp[3];
    crossProduct(temp, cam->camFront, cam->camUp);
    normalize(temp);
    cam->camPos[0] += temp[0] * cameraSpeed;
    cam->camPos[1] += temp[1] * cameraSpeed;
    cam->camPos[2] += temp[2] * cameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    mouse_callback(window, cam, cam->lastX, cam->lastY-1);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    mouse_callback(window, cam, cam->lastX, cam->lastY+1);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    mouse_callback(window, cam, cam->lastX-1, cam->lastY);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    mouse_callback(window, cam, cam->lastX+1, cam->lastY);
  }
}

GLfloat distToCamera(GLfloat objPos[3], GLfloat camPos[3]) {
    GLfloat dx = objPos[0] - camPos[0];
    GLfloat dy = objPos[1] - camPos[1];
    GLfloat dz = objPos[2] - camPos[2];
    return sqrt(dx*dx + dy*dy + dz*dz);
}

#endif
