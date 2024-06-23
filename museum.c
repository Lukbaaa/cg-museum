#include "light.h"
#include "shader.h"
#include "camera.h"
#include "object.h"
#include "texture.h"
#include "framebuffer.h"
#include "scene_graph.h"
#include "objectLoader.h"
  
#include <GL/glew.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow *window, Camera* cam);

Object* scene;

double timeAtDraw = 0;

Object** objects;
int objCount = 0;   
Object* createObject(const char* objFilePath) {
  objects = (Object**)realloc(objects, sizeof(Object*)*(objCount+1));
  objects[objCount] = createVAOwithObj(objFilePath);
  objCount++;
  return objects[objCount-1];
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

void drawWater(Object* obj) {

  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glUniform1f(glGetUniformLocation(program, "time"), timeAtDraw);

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawBoat(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);
  
  float displacement = (sin(timeAtDraw)+sin(timeAtDraw))*0.2;
  Vec3 rot = {
    -cos(timeAtDraw)*10,
    0,
    cos(timeAtDraw)*10
  };
  rotate(obj->model, obj->model, rot);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glUniform1f(glGetUniformLocation(program, "yDisplacement"), displacement);

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawBjarne(Object* obj) {

  int program = obj->shader->program;
  glUseProgram(program);
  glBindVertexArray(obj->vao);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glUniformMatrix4fv(glGetUniformLocation(program, "viewPos"), 1, GL_FALSE, (float*)&(camera->position));
  glUniform4fv(glGetUniformLocation(program, "material.emissive"), 1, obj->material.emissive);
  glUniform4fv(glGetUniformLocation(program, "material.ambient"), 1, obj->material.ambient);
  glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, obj->material.diffuse);
  glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, obj->material.specular);
  glUniform1f(glGetUniformLocation(program, "material.shininess"), obj->material.shininess);
  for(int i = 0; i < obj->lightsAffectedBy.length; i++) {
    glUniform3fv(glGetUniformLocation(program, "light.position"), 1, (float*)&(lightSourceListGet(&obj->lightsAffectedBy, i)->position));
    glUniform4fv(glGetUniformLocation(program, "light.ambient"), 1, (float*)&(lightSourceListGet(&obj->lightsAffectedBy, i)->ambient));
    glUniform4fv(glGetUniformLocation(program, "light.diffuse"), 1, (float*)&(lightSourceListGet(&obj->lightsAffectedBy, i)->diffuse));
    glUniform4fv(glGetUniformLocation(program, "light.specular"), 1, (float*)&(lightSourceListGet(&obj->lightsAffectedBy, i)->specular));
  }

  GLfloat normalMatrix[16];
  inverse4(normalMatrix, obj->model);
  transpose4(normalMatrix, normalMatrix);
  glUniformMatrix4fv(glGetUniformLocation(program, "normalMatrix"), 1, GL_FALSE, normalMatrix);

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);

}

void drawBjarneLight(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);

  glBindVertexArray(obj->vao);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glUniform4fv(glGetUniformLocation(program, "ambient"), 1, (float*)&(obj->light->ambient.r));
  glUniform4fv(glGetUniformLocation(program, "diffuse"), 1, (float*)&(obj->light->diffuse.r));
  glUniform4fv(glGetUniformLocation(program, "specular"), 1, (float*)&(obj->light->specular.r));

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawRMRenderer(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);

  glBindVertexArray(obj->vao);

  glBindFramebuffer(GL_FRAMEBUFFER, obj->renderTarget);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void drawWithTexture(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);

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

void bjarneLightAnimation(Object* obj) {
  setObjectPosition(obj, sin(glfwGetTime())*3,0,cos(glfwGetTime())*3);
  obj->light->position = obj->transform.position;
}

void boatAnimation(Object* obj) {
  Vec3 rot = {
    -cos(timeAtDraw)*10,
    0,
    cos(timeAtDraw)*10
  };
  rotate(obj->model, obj->model, rot);
}

void createScene(void) {
  
  Object* root = createObject("objects/cube.obj");
  Object* sun = createObject("objects/sphere.obj");
  Object* earth = createObject("objects/sphere.obj");
  Object* moon = createObject("objects/sphere.obj");
  Object* window = createObject("objects/window.obj");
  Object* window2 = createObject("objects/window.obj");
  Object* water = createObject("objects/plane.obj");
  Object* bjarne = createObject("objects/bjarne.obj");
  Object* bjarneLight = createObject("objects/sphere.obj");
  Object* boat = createObject("objects/boat.obj");
  Object* rmRenderer = createObject("objects/cube.obj");
  Object* rmDisplay = createObject("objects/cube.obj");

  rmRenderer->renderTarget = createRenderTarget();

  sgAddChild(root, rmRenderer);
  sgAddChild(root, rmDisplay);
  sgAddChild(root, sun);
  sgAddChild(sun, earth);
  sgAddChild(earth, moon);
  sgAddChild(root, window);
  sgAddChild(root, window2);
  sgAddChild(root, water);
  sgAddChild(root, bjarne);
  sgAddChild(bjarne, bjarneLight);
  sgAddChild(water, boat);

  root->shouldRender = 0;
  rmDisplay->shouldRender = 1;

  sun->shader = createShader("shaders/texture.vert", "shaders/texture.frag");
  earth->shader = createShader("shaders/texture.vert", "shaders/texture.frag");
  moon->shader = createShader("shaders/texture.vert", "shaders/texture.frag");
  window->shader = createShader("shaders/texture.vert", "shaders/texture.frag");
  window2->shader = createShader("shaders/texture.vert", "shaders/texture.frag");
  water->shader = createShader("shaders/water.vert", "shaders/water.frag");
  bjarne->shader = createShader("shaders/bjarne.vert", "shaders/bjarne.frag");
  bjarneLight->shader = createShader("shaders/lightsource.vert", "shaders/lightsource.frag");
  boat->shader = createShader("shaders/boat.vert", "shaders/boat.frag");
  rmRenderer->shader = createShader("shaders/raymarching.vert", "shaders/raymarching.frag");
  rmDisplay->shader = createShader("shaders/texture.vert", "shaders/rmDisplay.frag");

  loadTexture(sun, "textures/sun.png", 0);
  loadTexture(earth, "textures/earth_day.png", 0);
  loadTexture(moon, "textures/moon.png", 0);
  loadTexture(window, "textures/window_red.png", 0);
  loadTexture(window2, "textures/window_blue.png", 0);
  loadTexture(boat, "textures/boat.png", 0);
  loadTexture(water, "textures/water.png", 0);
  Texture rmTexture = loadTexture(rmDisplay, NULL, 0);
  attachRenderTexture(rmRenderer->renderTarget, rmTexture);

  sun->draw = &drawWithTexture;
  earth->draw = &drawWithTexture;
  moon->draw = &drawWithTexture;
  window->draw = &drawWithTexture;
  window2->draw = &drawWithTexture;
  water->draw = &drawWater;
  bjarne->draw = &drawBjarne;
  bjarneLight->draw = &drawBjarneLight;
  boat->draw = &drawBoat;
  rmDisplay->draw = &drawWithTexture;
  rmRenderer->draw = &drawRMRenderer;
  
  sun->animate = &sunAnimation;
  earth->animate = &earthAnimation;
  moon->animate = &moonAnimation;
  bjarneLight->animate = &bjarneLightAnimation;
  boat->animate = &boatAnimation;

  bjarne->material = rubin;

  setObjectPosition(window, 3,0,0);
  setObjectPosition(window2, 5,0,0);
  setObjectPosition(water, 0, 0, 20);
  setObjectPosition(bjarne, 10, 0, 0);
  setObjectPosition(rmRenderer, -15, 0, 0);
  setObjectPosition(rmDisplay, -10, 0, 0);
  setObjectScale(bjarneLight, 0.3,0.3,0.3);
  setObjectScale(boat, 0.5, 0.5, 0.5);
  setObjectScale(water, 0.1, 0.1, 0.1);

  LightSource* light = createLight();
  Vec4 ambient = {1,1,1,1};
  Vec4 diffuse = {1,1,1,1};
  Vec4 specular = {1,1,1,1};
  light->ambient = ambient;
  light->diffuse = diffuse;
  light->specular = specular;

  bjarneLight->light=light;

  addLightAffectedBy(bjarne, light);

  window->isTransparent = 1;
  window2->isTransparent = 1;

  scene = root;
} 

void init(void) {
  glEnable(GL_DEPTH_TEST);
  //glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  camera = createCamera();
  camera->center.x = camera->position.x + camera->camFront.x;
  camera->center.y = camera->position.y + camera->camFront.y;
  camera->center.z = camera->position.z + camera->camFront.z;
  lookAt(camera->view, camera->position, camera->center, camera->camUp);
  perspective(camera->projection, 45.0f, 800/800, 0.1, 100);

  boundingBoxProgram = createShader("shaders/boundingbox.vert", "shaders/boundingbox.frag")->program;

  glClearColor((1/255.0f)*191, (1/255.0f)*217, (1/255.0f)*204, 1.0f);
  glViewport(0, 0, 800, 800);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    changeView(camera);
    GLfloat modelStack[16];
    identity(modelStack);

    ObjectList transparentObjects;
    initObjectList(&transparentObjects);
    ObjectList illuminatedObjects;
    initObjectList(&illuminatedObjects);

    traverseDraw(scene, modelStack, &transparentObjects, &illuminatedObjects);
    drawIlluminatedObjects(&illuminatedObjects);
    drawTransparentObjects(&transparentObjects);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

int main(void) {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  //LIBGL_ALWAYS_SOFTWARE=true
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
    timeAtDraw = glfwGetTime();
    processInput(window, camera);
    draw();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free(objects);
  glfwTerminate();
  return 0;
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
    cam->position.x += cameraSpeed * cam->camFront.x;
    cam->position.y += cameraSpeed * cam->camFront.y;
    cam->position.z += cameraSpeed * cam->camFront.z;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    cam->position.x -= cameraSpeed * cam->camFront.x;
    cam->position.y -= cameraSpeed * cam->camFront.y;
    cam->position.z -= cameraSpeed * cam->camFront.z;
  }
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    Vec3 temp = crossProduct(cam->camFront, cam->camUp);
    normalize(&temp);
    cam->position.x -= temp.x * cameraSpeed;
    cam->position.y -= temp.y * cameraSpeed;
    cam->position.z -= temp.z * cameraSpeed;
  }
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    Vec3 temp = crossProduct(cam->camFront, cam->camUp);
    normalize(&temp);
    cam->position.x += temp.x * cameraSpeed;
    cam->position.y += temp.y * cameraSpeed;
    cam->position.z += temp.z * cameraSpeed;
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
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
    drawBoundingBoxes = !drawBoundingBoxes;
  }
}
