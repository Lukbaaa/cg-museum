#ifndef CAMERA_H
#define CAMERA_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#else
#define M_PI 3.14159265359
#endif

#include <assert.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "vector.h"
#include "boundingbox.h"
#include "matrix_functions.h"

typedef struct Camera {
  GLfloat view[16];
  GLfloat projection[16];
  Vec3 position;
  Vec3 camFront;
  Vec3 camUp;
  Vec3 center;
  GLfloat yaw;	
  GLfloat pitch;
  GLfloat lastX;
  GLfloat lastY;
  GLfloat fov;
  BoundingBox boundingBox;
  int active;
} Camera;

Camera* camera;

/**
 * Initialisiert die Kamera
 * @param cam - Kamera
 */
void initCamera(Camera* cam) {
  Vec3 camPos = {0,1,3};
  cam->position = camPos;
  Vec3 camFront = {0.0f, 0.0f, -1.0};
  cam->camFront = camFront;
  Vec3 camUp = {0, 1, 0};
  cam->camUp = camUp;
  cam->yaw   = -90.0f;	
  cam->pitch =  0.0f;
  cam->lastX =  800.0f / 2.0;
  cam->lastY =  600.0 / 2.0;
  cam->fov   =  45.0f;
  cam->boundingBox = createBoundingBox(1,0.2,1);
}

/**
 * Erstellt eine Kamera
 * @return Kamera
 */
Camera* createCamera() {
  Camera* cam = (Camera*)malloc(sizeof(Camera));
  initCamera(cam);
  return cam;
}

/**
 * Berechnet die Look-At Matrix
 * @param out - Ausgabe Matrix
 * @param camPos - Kameraposition
 * @param center - Zentrum der Szene
 */
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

/**
 * Berechnet die Perspektive Matrix
 * @param out - Ausgabe Matrix
 * @param fovy - Field of View
 * @param aspect - Aspect Ratio
 * @param near - Near Plane
 * @param far - Far Plane
 */
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

/**
 * Bewegt die Kamera
 * @param cam - Kamera
 */
void changeView(Camera* cam) {
  cam->center.x = cam->position.x + cam->camFront.x;
  cam->center.y = cam->position.y + cam->camFront.y;
  cam->center.z = cam->position.z + cam->camFront.z;

  lookAt(cam->view, cam->position, cam->center, cam->camUp);
}

int firstMouse = 0;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;

/**
 * Listener für Mauseingabe
 * @param window - Fenster
 * @param xposIn - x-Position
 * @param yposIn - y-Position
 */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
  assert(window != NULL);

  float xpos = xposIn;
  float ypos = yposIn;

  if (firstMouse)
  {
      camera->lastX = xpos;
      camera->lastY = ypos;
      firstMouse = 0;
  }

  float xoffset = xpos - camera->lastX;
  float yoffset = camera->lastY - ypos; 
  camera->lastX = xpos;
  camera->lastY = ypos;

  float sensitivity = 0.5f; 
  xoffset *= sensitivity;
  yoffset *= sensitivity;

  camera->yaw += xoffset;
  camera->pitch += yoffset;

  if (camera->pitch > 89.0f)
      camera->pitch = 89.0f;
  if (camera->pitch < -89.0f)
      camera->pitch = -89.0f;

  camera->camFront.x = cos(camera->yaw*M_PI/180) * cos(camera->pitch*M_PI/180);
  camera->camFront.y = sin(camera->pitch*M_PI/180);
  camera->camFront.z = sin(camera->yaw*M_PI/180) * cos(camera->pitch*M_PI/180);
  normalize(&(camera->camFront));
}

/**
 * Listener für Maus-Scroller
 * @param window - Fenster
 * @param cam - Kamera
 * @param xoffset - x-Offset
 * @param yoffset - y-Offset
 */
void scroll_callback(GLFWwindow* window, Camera* cam, GLfloat xoffset, GLfloat yoffset)
{
  assert(window != NULL);

  cam->fov -= (float)yoffset;
  if (cam->fov < 1.0f)
      cam->fov = 1.0f;
  if (cam->fov > 45.0f)
      cam->fov = 45.0f;
}

/**
 * Berechnen des Abstands zwischen Kamera und Objekt
 * @param objPos - Position des Objekts
 * @param camPos - Position der Kamera
 */
GLfloat distToCamera(Vec3 objPos, Vec3 camPos) {
    GLfloat dx = objPos.x - camPos.x;
    GLfloat dy = objPos.y - camPos.y;
    GLfloat dz = objPos.z - camPos.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

#endif
