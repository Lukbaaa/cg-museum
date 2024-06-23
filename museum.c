#include "light.h"
#include "shader.h"
#include "camera.h"
#include "object.h"
#include "texture.h"
#include "scene_graph.h"
#include "objectLoader.h"
#include"skybox.h"
#include "particle2.h"
  
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow *window, Camera* cam);

GLuint particlesProgram;
Object* scene;

Object** objects;
int objCount = 0;   

double timeAtDraw = 0;
double timeAtStart;

void drawTextures(Object* obj) {
  for (int j = 0; j < obj->textureCount; j++) {
    char uniformName[15]; 
    sprintf(uniformName, "tex%d", j);
    glUniform1i(glGetUniformLocation(obj->shader->program, uniformName), j);
    glActiveTexture(GL_TEXTURE0+j);
    glBindTexture(GL_TEXTURE_2D, obj->textures[j]);
  }
}

void drawSphere(Object* obj) {

  int program = obj->shader->program;
  glUseProgram(program);

  drawTextures(obj);

  glBindVertexArray(obj->vao);

  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);

  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawSkybox() {
  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  glBindVertexArray(getVAO());
  glUseProgram(scene->shader->program);
  glUniform1i(glGetUniformLocation(scene->shader->program, "skybox"), 0);
  glUniformMatrix4fv(glGetUniformLocation(scene->shader->program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(scene->shader->program, "projection"), 1, GL_TRUE, camera->projection);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, getCubemapTexture());
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glDepthFunc(GL_LESS);
  glDepthMask(GL_TRUE);
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

void sunAnimation(Object* obj) {
  //setObjectPosition(obj, 0, 0, 0);
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
  //printVec3(obj->light->position);
}

void drawFloor(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);
  drawTextures(obj);
  glBindVertexArray(obj->vao);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawFundament(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);
  drawTextures(obj);
  glBindVertexArray(obj->vao);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawPodest(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);
  drawTextures(obj);
  glBindVertexArray(obj->vao);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawRopes(Object* obj) {
  int program = obj->shader->program;
  glUseProgram(program);

  glBindVertexArray(obj->vao);
  glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, obj->model);
  glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
  glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);
  glUniform1f(glGetUniformLocation(program, "time"), timeAtDraw);
  glDrawArrays(GL_TRIANGLES, 0, obj->vertCount);
}

void drawPodestPanels(Object* obj) {
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

  Object* house = createObject("objects/cube.obj");
  Object* houseFloor = createObject("objects/house_objects/house_floor.obj");
  Object* houseFundament = createObject("objects/house_objects/house_fundament.obj");
  Object* houseRoof = createObject("objects/house_objects/house_roof.obj");
  Object* ropes = createObject("objects/house_objects/house_ropes.obj");
  Object* baloon1 = createObject("objects/house_objects/house_baloon1.obj");
  
  Object* particleLight = createObject("objects/sphere.obj");

  Object* vitrineFront= createObject("objects/house_objects/vitrine/glas_front.obj"); //vitrineFront
  Object* vitrineRigth= createObject("objects/house_objects/vitrine/glas_right.obj"); //vitrineRigth
  Object* vitrineLeft= createObject("objects/house_objects/vitrine/glas_left.obj"); //vitrineLeft
  Object* vitrineBack= createObject("objects/house_objects/vitrine/glas_back.obj"); //vitrineBack
  Object* vitrineTop= createObject("objects/house_objects/vitrine/top.obj"); //vitrineTop
  Object* vitrinePodest= createObject("objects/house_objects/vitrine/podest.obj"); //vitrinePodest

  Object* vitrine1=createObject("objects/cube.obj");
  Object* vitrine2=createObject("objects/cube.obj");
  Object* vitrine3=createObject("objects/cube.obj");
  Object* vitrine4=createObject("objects/cube.obj");

  // particleObject = createObject("objects/cube.obj");

  sgAddChild(root, window);
  sgAddChild(root, window2);
  sgAddChild(root, bjarne);
  sgAddChild(root,house);

  sgAddChild(house,houseFundament);
  sgAddChild(house,houseFloor);
  sgAddChild(house,houseRoof);
  sgAddChild(house,ropes);
  sgAddChild(house,baloon1);
  sgAddChild(house,vitrine1);

  sgAddChild(bjarne, bjarneLight);
  sgAddChild(vitrine1, sun);
  sgAddChild(vitrine1,vitrinePodest);
  sgAddChild(sun, earth);
  sgAddChild(earth, moon);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);

  sgAddChild(house,vitrine2);
  sgAddChild(vitrine2, water);
  sgAddChild(vitrine2,vitrinePodest);
  sgAddChild(water, boat);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);

  sgAddChild(house,vitrine3);
  sgAddChild(vitrine3, water);
  sgAddChild(vitrine3,vitrinePodest);
  sgAddChild(water, boat);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);

  sgAddChild(house,vitrine4);
  sgAddChild(vitrine4, water);
  sgAddChild(vitrine4,vitrinePodest);
  sgAddChild(water, boat);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);


  sgAddChild(root, particleLight);


  root->shouldRender = 0;
  house->shouldRender = 0;
  vitrine1->shouldRender =0;
  vitrine2->shouldRender =0;
  vitrine3->shouldRender =0;
  vitrine4->shouldRender =0;
  sun->shouldRender = 1;
  
  // particleObject->shouldRender = 1;

  sun->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  earth->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  moon->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  window->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  window2->shader = createShader("shaders/tex.vert", "shaders/tex.frag");
  water->shader = createShader("shaders/water.vert", "shaders/water.frag");
  bjarne->shader = createShader("shaders/bjarne.vert", "shaders/bjarne.frag");
  bjarneLight->shader = createShader("shaders/lightsource.vert", "shaders/lightsource.frag");
  boat->shader = createShader("shaders/boat.vert", "shaders/boat.frag");
  houseFloor->shader = createShader("shaders/house_shader/floor.vert","shaders/house_shader/floor.frag");
  houseFundament->shader = createShader("shaders/house_shader/fundament.vert","shaders/house_shader/fundament.frag");
  houseRoof->shader = houseFloor->shader;
  ropes->shader = createShader("shaders/house_shader/ropes.vert","shaders/house_shader/ropes.frag");
  baloon1->shader = houseFloor->shader;

  //Vitrine 1, gibt es eine Möglichkeit, diese wiederzuverwenden?
  vitrinePodest->shader = createShader("shaders/house_shader/podest.vert","shaders/house_shader/podest.frag");
  vitrineFront->shader = createShader("shaders/house_shader/vitrine_glas.vert","shaders/house_shader/vitrine_glas.frag");
  vitrineBack->shader = vitrineFront->shader;
  vitrineRigth->shader = vitrineFront->shader;
  vitrineLeft->shader = vitrineFront->shader;
  vitrineTop->shader = vitrinePodest->shader;

  particleLight->shader = createShader("shaders/house_shader/floor.vert","shaders/house_shader/floor.frag");
  
  // particleObject->shader = createShader("shaders/particle.vert", "shaders/particle.frag");


  loadTexture(sun, "textures/sun.png", 0);
  loadTexture(earth, "textures/earth_day.png", 0);
  loadTexture(moon, "textures/moon.png", 0);
  loadTexture(window, "textures/window_red.png", 0);
  loadTexture(window2, "textures/window_blue.png", 0);
  loadTexture(boat, "textures/boat.png", 0);
  loadTexture(water, "textures/water.png", 0);
  loadTexture(houseFloor, "textures/wood_floor.png",0);
  loadTexture(houseFundament, "textures/brick_texture.png",0);
  loadTexture(vitrinePodest, "textures/black_marble_texture.jpg",0);
  loadTexture(vitrineTop,"textures/black_marble_texture.jpg",0);
  loadTexture(vitrineFront,"textures/podest_glass.png",0);
  loadTexture(vitrineBack,"textures/podest_glass.png",0);
  loadTexture(vitrineRigth,"textures/podest_glass.png",0);
  loadTexture(vitrineLeft,"textures/podest_glass.png",0);

  sun->draw = &drawSphere;
  earth->draw = &drawSphere;
  moon->draw = &drawSphere;
  window->draw = &drawSphere;
  window2->draw = &drawSphere;
  water->draw = &drawWater;
  bjarne->draw = &drawBjarne;
  bjarneLight->draw = &drawBjarneLight;
  boat->draw = &drawBoat;
  houseFloor->draw = &drawFloor;
  houseFundament->draw = &drawFundament;
  houseRoof->draw = &drawFloor;
  ropes->draw = &drawRopes;
  baloon1->draw = &drawFloor;
  vitrinePodest->draw = &drawPodest;
  particleLight->draw = &drawFloor;
  vitrineFront->draw = &drawPodestPanels;
  vitrineBack->draw = &drawPodestPanels;
  vitrineRigth->draw = &drawPodestPanels;
  vitrineLeft->draw = &drawPodestPanels;
  vitrineTop->draw = drawPodest;
  
  // particleObject->draw = &drawParticles;
  

  sun->animate = &sunAnimation;
  earth->animate = &earthAnimation;
  moon->animate = &moonAnimation;
  bjarneLight->animate = &bjarneLightAnimation;


  bjarne->material = rubin;
  // particleObject->material = wood;


  setObjectPosition(window, 3,0,0);
  setObjectPosition(window2, 5,0,0);
  // setObjectPosition(water, 0, 0, 20);
  setObjectPosition(bjarne, 10, 0, 0);
  setObjectPosition(particleLight, 1.0,2.0,0.4);
  setObjectPosition(vitrine1,11.25,-15,16);
  setObjectPosition(vitrine2,11.25,-15,-16);
  setObjectPosition(vitrine3,-11.25,-15,16);
  setObjectPosition(vitrine4,-11.25,-15,-16);
  setObjectScale(sun, 0.5,0.5,0.5);
  setObjectScale(bjarneLight, 0.3,0.3,0.3);
  setObjectScale(house,0.1,0.1,0.1);
  //setObjectScale(boat, 0.5, 0.5, 0.5);
  setObjectScale(water, 0.5, 0.5, 0.5);
  //setObjectScale(houseFloor, 0.2,0.2,0.2);
  // setObjectScale(vitrinePodest,1.2,1.2,1.2);
  setObjectScale(particleLight,0.1,0.1,0.1);
  


  LightSource* light = createLight();
  Vec4 ambient = {1,1,1,1};
  Vec4 diffuse = {1,1,1,1};
  Vec4 specular = {1,1,1,1};  light->diffuse = diffuse;
  light->specular = specular;

  bjarneLight->light=light;

  addLightAffectedBy(bjarne, light);

  window->isTransparent = 1;
  window2->isTransparent = 1;
  vitrineBack->isTransparent = 1; //Irgenwie spackt das rum
  vitrineFront->isTransparent = 1;
  vitrineLeft->isTransparent = 1;
  vitrineRigth->isTransparent = 1;
  
  
  // for (int i = 0; i < NUM_PARTICLES; i++) {
  //   // init
  //   partList[i] = createObject("objects/leer.obj");
  //   sgAddChild(particleObject, partList[i]);
  //   partList[i]->shader = createShader("shaders/house_shader/fundament.vert","shaders/house_shader/fundament.frag");
  //   partList[i]->draw = &drawPart;
  //   // partList[i]->shader = createShader("shaders/particle.vert", "shaders/particle.frag");
  //   // partList[i]->draw = &drawParticles;
    
  //   // partList[i]->material = wood;

  // }
  
  scene = root;
}

void draw() {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawSkybox();
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
  timeAtStart = glfwGetTime();
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

  initializeSkybox();
  loadCubemapTexture();
  scene->shader = createShader("shaders/skybox.vert", "shaders/skybox.frag");
    
  while (!glfwWindowShouldClose(window)) {
    timeAtDraw = glfwGetTime();
    float deltaTime = timeAtDraw - timeAtStart;

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
