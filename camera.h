#ifndef CAMERA_H
#define CAMERA_H

#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vector.h"
#include "matrix_functions.h"

typedef struct Camera {
  GLfloat view[16];
  GLfloat projection[16];
  Vec3 camPos;
  Vec3 camFront;
  Vec3 camUp;
  Vec3 center;
  GLfloat yaw;	
  GLfloat pitch;
  GLfloat lastX;
  GLfloat lastY;
  GLfloat fov;
} Camera;

void initCamera(Camera* cam) {
  Vec3 camPos = {0,0,0};
  cam->camPos = camPos;
  Vec3 camFront = {0.0f, 0.0f, -1.0};
  cam->camFront = camFront;
  Vec3 camUp = {0, 1, 0};
  cam->camUp = camUp;
  cam->yaw   = -90.0f;	
  cam->pitch =  0.0f;
  cam->lastX =  800.0f / 2.0;
  cam->lastY =  600.0 / 2.0;
  cam->fov   =  45.0f;
}

Camera* createCamera(void) {
  Camera* cam = (Camera*)malloc(sizeof(Camera));
  initCamera(cam);
  return cam;
}

void lookAt(GLfloat*out, Vec3 camPos, Vec3 center, Vec3 up) {
  Vec3 n = {camPos.x-center.x, camPos.y-center.y, camPos.z-center.z};
  Vec3 u = crossProduct(up, n);
  Vec3 v = crossProduct(n, u);
    
  normalize(&n);
  normalize(&u);
  normalize(&v);

  GLfloat tx = -dotProduct(u, camPos);
  GLfloat ty = -dotProduct(v, camPos);
  GLfloat tz = -dotProduct(n, camPos);

  GLfloat view[16] = 
    {
      u.x,v.x,n.x,0,
      u.y,v.y,n.y,0,
      u.z,v.z,n.z,0,
       tx, ty, tz,1
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
  cam->center.x = cam->camPos.x + cam->camFront.x;
  cam->center.y = cam->camPos.y + cam->camFront.y;
  cam->center.z = cam->camPos.z + cam->camFront.z;
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

  cam->camFront.x = cos(cam->yaw*M_PI/180) * cos(cam->pitch*M_PI/180);
  cam->camFront.y = sin(cam->pitch*M_PI/180);
  cam->camFront.z = sin(cam->yaw*M_PI/180) * cos(cam->pitch*M_PI/180);
  normalize(&(cam->camFront));
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
    cam->camPos.x += cameraSpeed * cam->camFront.x;
    cam->camPos.y += cameraSpeed * cam->camFront.y;
    cam->camPos.z += cameraSpeed * cam->camFront.z;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam->camPos.x -= cameraSpeed * cam->camFront.x;
    cam->camPos.y -= cameraSpeed * cam->camFront.y;
    cam->camPos.z -= cameraSpeed * cam->camFront.z;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    Vec3 temp = crossProduct(cam->camFront, cam->camUp);
    normalize(&temp);
    cam->camPos.x -= temp.x * cameraSpeed;
    cam->camPos.y -= temp.y * cameraSpeed;
    cam->camPos.z -= temp.z * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    Vec3 temp = crossProduct(cam->camFront, cam->camUp);
    normalize(&temp);
    cam->camPos.x += temp.x * cameraSpeed;
    cam->camPos.y += temp.y * cameraSpeed;
    cam->camPos.z += temp.z * cameraSpeed;
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

GLfloat distToCamera(Vec3 objPos, Vec3 camPos) {
    GLfloat dx = objPos.x - camPos.x;
    GLfloat dy = objPos.y - camPos.y;
    GLfloat dz = objPos.z - camPos.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

#endif
