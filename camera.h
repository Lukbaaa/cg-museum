#ifndef CAMERA_H
#define CAMERA_H

#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "matrix_functions.h"

int firstMouse = 0;
float yaw   = -90.0f;	
float pitch =  0.0f;
float lastX =  800.0f / 2.0;
float lastY =  600.0 / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

GLfloat camPos[3] = {0, 0, 5};
GLfloat camFront[3] = {0.0f, 0.0f, -1.0f};
GLfloat camUp[3] = {0, 1, 0};
GLfloat center[3];

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  assert(window != NULL);

  float xpos = xposIn;
  float ypos = yposIn;

  if (firstMouse)
  {
      lastX = xpos;
      lastY = ypos;
      firstMouse = 0;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; 
  lastX = xpos;
  lastY = ypos;

  float sensitivity = 0.5f; 
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  yaw += xoffset;
  pitch += yoffset;

  if (pitch > 89.0f)
      pitch = 89.0f;
  if (pitch < -89.0f)
      pitch = -89.0f;

  camFront[0] = cos(yaw*M_PI/180) * cos(pitch*M_PI/180);
  camFront[1] = sin(pitch*M_PI/180);
  camFront[2] = sin(yaw*M_PI/180) * cos(pitch*M_PI/180);
  normalize(camFront);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  assert(window != NULL);

  fov -= (float)yoffset;
  if (fov < 1.0f)
      fov = 1.0f;
  if (fov > 45.0f)
      fov = 45.0f;
}

void processInput(GLFWwindow *window)
{
  assert(window != NULL);
  
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);

  float cameraSpeed = 5 * deltaTime;
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    camPos[0] += cameraSpeed * camFront[0];
    camPos[1] += cameraSpeed * camFront[1];
    camPos[2] += cameraSpeed * camFront[2];
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    camPos[0] -= cameraSpeed * camFront[0];
    camPos[1] -= cameraSpeed * camFront[1];
    camPos[2] -= cameraSpeed * camFront[2];
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    GLfloat temp[3];
    crossProduct(temp, camFront, camUp);
    normalize(temp);
    camPos[0] -= temp[0] * cameraSpeed;
    camPos[1] -= temp[1] * cameraSpeed;
    camPos[2] -= temp[2] * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    GLfloat temp[3];
    crossProduct(temp, camFront, camUp);
    normalize(temp);
    camPos[0] += temp[0] * cameraSpeed;
    camPos[1] += temp[1] * cameraSpeed;
    camPos[2] += temp[2] * cameraSpeed;
  }

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    mouse_callback(window, lastX, lastY-1);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    mouse_callback(window, lastX, lastY+1);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    mouse_callback(window, lastX-1, lastY);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
    mouse_callback(window, lastX+1, lastY);
  }
}

#endif
