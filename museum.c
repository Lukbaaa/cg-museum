#include "light.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "scene_graph.h"
#include "objectLoader.h"
#include "matrix_functions.h"
  
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

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
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  center[0] = camPos[0] + camFront[0];
  center[1] = camPos[1] + camFront[1];
  center[2] = camPos[2] + camFront[2];
  lookAt(view, camPos, center, camUp);
  perspective(projection, 45.0f, 800/800, 0.1, 100);

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
   
  changeView();

  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawCube(Object* obj, GLfloat model[16]) {
   
  changeView();

  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection);
  //printMat4(model, 1);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void cube1Animation(Object* obj) {
  setPosition(obj, 0, glfwGetTime()-5, 0);
  setRotation(obj, 0, glfwGetTime()*100, 0);
}

void cube2Animation(Object* obj) {
  setPosition(obj, 5,0,0);
  setRotation(obj, glfwGetTime()*500, 0,0);
}

void createScene(void) {
  Object* root = createObject("objects/cube.obj");
  Object* cube1 = createObject("objects/cube.obj");
  Object* cube2 = createObject("objects/cube.obj");

  scAddChild(root, cube1);
  scAddChild(cube1, cube2);

  root->shouldRender = 0;

  cube1->shader = createShader("shaders/crate.vert", "shaders/crate.frag");
  cube2->shader = createShader("shaders/crate.vert", "shaders/crate.frag");

  loadTexture(cube1, "textures/crate.png", 0);
  loadTexture(cube2, "textures/crate.png", 0);

  cube1->draw = &drawCube;
  cube2->draw = &drawCube;

  cube1->animate = &cube1Animation;
  cube2->animate = &cube2Animation;

  scene = root;
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
    processInput(window);
    changeView();
    GLfloat model[16];
    identity(model);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    traverseDraw(scene, model);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free(objects);
  glfwTerminate();
  return 0;
}
