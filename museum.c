#include "light.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "scene_graph.h"
#include "objectLoader.h"
#include "object.h"
  
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Camera* cam;
Object* scene;

LightSource light = 
  {
    {-12.0f, 2.0f, 2.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f}
  };

Object** objects;
int objCount = 0;

Object* createObject(const char* objFilePath) {
  objects = (Object**)realloc(objects, sizeof(Object*)*(objCount+1));
  objects[objCount] = createVAOwithObj(objFilePath);
  objCount++;
  return objects[objCount-1];
}

void init(void) {
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  cam = createCamera();
  cam->center[0] = cam->camPos[0] + cam->camFront[0];
  cam->center[1] = cam->camPos[1] + cam->camFront[1];
  cam->center[2] = cam->camPos[2] + cam->camFront[2];
  lookAt(cam->view, cam->camPos, cam->center, cam->camUp);
  perspective(cam->projection, 45.0f, 800/800, 0.1, 100);

  glClearColor((1/255.0f)*191, (1/255.0f)*217, (1/255.0f)*204, 1.0f);
  glViewport(0, 0, 800, 800);
}

void drawTextures(Object* obj) {
  for (int j = 0; j < obj->textureCount; j++) {
    char uniformName[15]; 
    sprintf(uniformName, "tex%d", j);
    glUniform1i(glGetUniformLocation(obj->shader->program, uniformName), j);
    glActiveTexture(GL_TEXTURE0+j);
    glBindTexture(GL_TEXTURE_2D, obj->textures[j]);
  } 
}

void drawScene(Object* obj) {
  if (!obj->shouldRender) {
      return;
  }
   
  changeView(cam);

  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawSphere(Object* obj) {
   
  changeView(cam);

  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, cam->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, cam->projection);

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void sunAnimation(Object* obj) {
  setObjectPosition(obj, 0, 0, 0);
  setObjectRotation(obj, 0, glfwGetTime()*10, 0);
}

void earthAnimation(Object* obj) {
  setObjectPosition(obj, 7,0,0);
  setObjectRotation(obj, 0, glfwGetTime()*100,0);
}

void moonAnimation(Object* obj) {
  setObjectPosition(obj, 2,0,0);
  setObjectRotation(obj, 0, glfwGetTime()*50,0);
  setObjectScale(obj, 0.3, 0.3, 0.3);
}

void createScene(void) {

  Object* root = createObject("objects/cube.obj");
  Object* sun = createObject("objects/sphere.obj");
  Object* earth = createObject("objects/sphere.obj");
  Object* moon = createObject("objects/sphere.obj");
  Object* window = createObject("objects/window.obj");
  Object* window2 = createObject("objects/window.obj");

  root->camera = cam;
  sun->camera = cam;
  earth->camera = cam;
  moon->camera = cam;
  window->camera = cam;
  window2->camera = cam;

  scAddChild(root, sun);
  scAddChild(sun, earth);
  scAddChild(earth, moon);
  scAddChild(root, window);
  scAddChild(root, window2);

  root->shouldRender = 0;

  sun->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  earth->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  moon->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  window->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  window2->shader = createShader("shaders/tex.vert", "shaders/tex.frag");

  loadTexture(sun, "textures/sun.png", 0);
  loadTexture(earth, "textures/earth_day.png", 0);
  loadTexture(moon, "textures/moon.png", 0);
  loadTexture(window, "textures/window_red.png", 0);
  loadTexture(window2, "textures/window_blue.png", 0);

  sun->draw = &drawSphere;
  earth->draw = &drawSphere;
  moon->draw = &drawSphere;
  window->draw = &drawSphere;
  window2->draw = &drawSphere;
  //setObjectPosition(window, 0, 0, 5);
  sun->animate = &sunAnimation;
  earth->animate = &earthAnimation;
  moon->animate = &moonAnimation;
  setObjectPosition(window, 3,0,0);
  setObjectPosition(window2, 5,0,0);
  
  window->isTransparent = 1;
  window2->isTransparent = 1;

  scene = root;
} 

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    changeView(cam);
    GLfloat model[16];
    identity(model);
    int toCount = 0;
    Object** transparentObjects = NULL;
    traverseDraw(scene, model, &transparentObjects, &toCount);
    drawTransparentObjects(transparentObjects, toCount);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  //glewExperimental = GL_TRUE;
  GLFWwindow *window =
      glfwCreateWindow(800, 800, "Computergrafik 1", NULL, NULL);
  if (!window) {
    printf("Failed to create window\n");
    glfwTerminate();
    return -1;
  }
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwWindowHint(GLFW_DEPTH_BITS, 24);

  //glfwSetCursorPosCallback(window, mouse_callback);
  //glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);
  glewInit();
  init();
  createScene();
    
  while (!glfwWindowShouldClose(window)) {
    processInput(window, cam);
    draw();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free(objects);
  glfwTerminate();
  return 0;
}
