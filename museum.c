#include "light.h"
#include "shader.h"
#include "camera.h"
#include "object.h"
#include "texture.h"
#include "framebuffer.h"
#include "scene_graph.h"
#include "objectLoader.h"
#include"skybox.h"
#include "particle2.h"
  
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#else
#define M_PI 3.14159265359
#endif

#include <GL/glew.h>
#include <GLFW/glfw3.h>

void processInput(GLFWwindow *window, Camera* cam);

GLuint particlesProgram;
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

void drawParticles(Object* obj) {
    updateParticles(deltaTime*1.5);  
    int program = obj->shader->program;
    glUseProgram(program);
    
    // ist das notw?
    glBindVertexArray(obj->vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2); 
    
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferparticles);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferparticles);
    for (int i = 0; i < NUM_PARTICLES; i++) {
        if (ps.particles[i].life > 0.0f) {

            GLfloat model[16];
            identity(model);
            translateParticleVector(model, model, ps.particles[i].position);
            // printf("particle%d position: %f, %f, %f\n", i, ps.particles[i].position[0], ps.particles[i].position[1], ps.particles[i].position[2]);            
            // printf("global position: %f, %f, %f\n", obj->globalPosition.x, obj->globalPosition.y, obj->globalPosition.z);
            // setObjectPosition(obj, ps.particles[i].position[0], ps.particles[i].position[1], ps.particles[i].position[2]);
            GLfloat normalMatrix[16];
            inverse4(normalMatrix, model);
            transpose4(normalMatrix, normalMatrix);
            glUniformMatrix4fv(glGetUniformLocation(program, "normalMatrix"), 1, GL_FALSE, normalMatrix);

            glUniform1f(glGetUniformLocation(program,"dt"),ps.particles[i].life);
            glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, model);
            glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, camera->view);
            glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, camera->projection);

            glUniform4fv(glGetUniformLocation(program, "material.emissive"), 1, obj->material.emissive);
            glUniform4fv(glGetUniformLocation(program, "material.ambient"), 1, obj->material.ambient);
            glUniform4fv(glGetUniformLocation(program, "material.diffuse"), 1, obj->material.diffuse);
            glUniform4fv(glGetUniformLocation(program, "material.specular"), 1, obj->material.specular);
            glUniform1f(glGetUniformLocation(program, "material.shininess"), obj->material.shininess);
            
            glUniform3fv(glGetUniformLocation(program, "light.position"), 1, (float*)&obj->light->position); 
            glUniform4fv(glGetUniformLocation(program, "light.ambient"), 1, (float*)&(obj->light->ambient.r));
            glUniform4fv(glGetUniformLocation(program, "light.diffuse"), 1, (float*)&(obj->light->diffuse.r));
            glUniform4fv(glGetUniformLocation(program, "light.specular"), 1, (float*)&(obj->light->specular.r));
            
            //glfw get time in time
            glUniform1f(glGetUniformLocation(program, "time"), glfwGetTime());

            

            glDrawElements(GL_TRIANGLES, obj->vertCount, GL_UNSIGNED_INT, (void*)0);
            
        }
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
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
    glUniform3fv(glGetUniformLocation(program, "light.position"), 1, (float*)&(obj->lightsAffectedBy.objects[i]->position));
    glUniform4fv(glGetUniformLocation(program, "light.ambient"), 1, (float*)&(obj->lightsAffectedBy.objects[i]->ambient));
    glUniform4fv(glGetUniformLocation(program, "light.diffuse"), 1, (float*)&(obj->lightsAffectedBy.objects[i]->diffuse));
    glUniform4fv(glGetUniformLocation(program, "light.specular"), 1, (float*)&(obj->lightsAffectedBy.objects[i]->specular));
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
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float x = obj->transform.position.x - camera->position.x;
  float y = obj->transform.position.z - camera->position.z;
  float angle = atan2(y,x)*180/M_PI;
  glUniform1f(glGetUniformLocation(program, "time"), timeAtDraw);
  glUniform1f(glGetUniformLocation(program, "angle"), angle);

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
} 

void boatAnimation(Object* obj) {
  setObjectRotation(obj, -cos(timeAtDraw)*10, 0, cos(timeAtDraw)*10);
}

void rmDisplayAnimation(Object* obj) {
  float x = obj->transform.position.x - camera->position.x;
  float y = obj->transform.position.z - camera->position.z;
  setObjectRotation(obj, 0, 180-atan2(y,x)*180/M_PI, 0);
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

void createScene(void) {
  
  Object* root = createObject(NULL);
  Object* sun = createObject("objects/sphere.obj");
  Object* earth = createObject("objects/sphere.obj");
  Object* moon = createObject("objects/sphere.obj");
  Object* water = createObject("objects/plane.obj");
  //Object* mountRushmore = createObject("objects/mt_rushmore.obj");
  Object* bjarneLight = createObject("objects/sphere.obj");
  Object* skyboxSun = createObject("objects/sphere.obj");
  Object* boat = createObject("objects/boat.obj");
  Object* rmRenderer = createObject("objects/cube.obj");
  Object* rmDisplay = createObject("objects/window.obj");


  Object* house = createObject(NULL);
  Object* houseFloor = createObject("objects/house_objects/house_floor.obj");
  // Object* houseFundament = createObject("objects/house_objects/house_fundament.obj");
  Object* houseFundamentFront_L = createObject("objects/house_objects/house_fundament_front_L.obj");
  Object* houseFundamentFront_R = createObject("objects/house_objects/house_fundament_front_R.obj");
  Object* houseFundamentFront_M = createObject("objects/house_objects/house_fundament_front_M.obj");
  Object* houseFundamentBack = createObject("objects/house_objects/house_fundament_back.obj");
  Object* houseFundamentLeft = createObject("objects/house_objects/house_fundament_left.obj");
  Object* houseFundamentRight = createObject("objects/house_objects/house_fundament_right.obj");
  Object* houseRoof = createObject("objects/house_objects/house_roof.obj");
  Object* ropes = createObject("objects/house_objects/house_ropes.obj");
  Object* baloon1 = createObject("objects/house_objects/house_baloon1.obj");
  Object* baloon2 = createObject("objects/house_objects/house_baloon2.obj");
  //Object* mountRushmore = createObject("objects/mt_rushmore.obj");
  Object* particleLight = createObject("objects/cube.obj");
  

  Object* vitrineFront= createObject("objects/house_objects/vitrine/glas_front.obj"); //vitrineFront
  Object* vitrineRigth= createObject("objects/house_objects/vitrine/glas_right.obj"); //vitrineRigth
  Object* vitrineLeft= createObject("objects/house_objects/vitrine/glas_left.obj"); //vitrineLeft
  Object* vitrineBack= createObject("objects/house_objects/vitrine/glas_back.obj"); //vitrineBack
  Object* vitrineTop= createObject("objects/house_objects/vitrine/top.obj"); //vitrineTop
  Object* vitrinePodest= createObject("objects/house_objects/vitrine/podest.obj"); //vitrinePodest

  Object* vitrine1=createObject(NULL);
  Object* vitrine2=createObject(NULL);
  Object* vitrine3=createObject(NULL);
  Object* vitrine4=createObject(NULL);

  Object* houseWindow1=createObject("objects/house_objects/house_window1.obj");
  Object* houseWindow2=createObject("objects/house_objects/house_window2.obj");
  Object* houseWindow3=createObject("objects/house_objects/house_window3.obj");
  Object* houseWindow4=createObject("objects/house_objects/house_window4.obj");
  Object* houseWindow5=createObject("objects/house_objects/house_window5.obj");
  Object* houseWindow6=createObject("objects/house_objects/house_window6.obj");
  Object* houseWindow7=createObject("objects/house_objects/house_window7.obj");
  Object* houseWindow8=createObject("objects/house_objects/house_window8.obj");

  rmRenderer->renderTarget = createRenderTarget();

  particleObject = createObject("objects/cube.obj");
  initParticles();

  // child objects
  sgAddChild(root, particleObject);
  //sgAddChild(root, mountRushmore);
  sgAddChild(root,house);
  sgAddChild(root, rmRenderer);
  sgAddChild(root, rmDisplay);

  sgAddChild(house,houseFundamentFront_L);
  sgAddChild(house,houseFundamentFront_M);
  sgAddChild(house,houseFundamentFront_R);
  sgAddChild(house,houseFundamentBack);
  sgAddChild(house,houseFundamentLeft);
  sgAddChild(house,houseFundamentRight);

  sgAddChild(house,houseWindow1);
  sgAddChild(house,houseWindow2);
  sgAddChild(house,houseWindow3);
  sgAddChild(house,houseWindow4);
  sgAddChild(house,houseWindow5);
  sgAddChild(house,houseWindow6);
  sgAddChild(house,houseWindow7);
  sgAddChild(house,houseWindow8);
  
  sgAddChild(house,houseFloor);
  sgAddChild(house,houseRoof);
  sgAddChild(house,ropes);
  sgAddChild(house,baloon1);
  sgAddChild(house,baloon2);
  sgAddChild(root,skyboxSun);
  sgAddChild(house,vitrine1);

  
  // Solar System
  sgAddChild(vitrine1, sun);
  sgAddChild(vitrine1,vitrinePodest);
  sgAddChild(sun, earth);
  sgAddChild(earth, moon);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);

  // Boat
  sgAddChild(house,vitrine2);
  sgAddChild(vitrine2, water);
  sgAddChild(vitrine2,vitrinePodest);
  sgAddChild(water, boat);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);

  // mount Rushmore
  sgAddChild(house,vitrine3);
  //sgAddChild(vitrine3, mountRushmore);
  //sgAddChild(mountRushmore, bjarneLight);
  sgAddChild(vitrine3,vitrinePodest);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);

  // 
  sgAddChild(house,vitrine4);
  // sgAddChild(vitrine4, rmRenderer); 
  // sgAddChild(vitrine4, rmDisplay);

  sgAddChild(vitrine4, particleObject);
  sgAddChild(vitrine4,vitrinePodest);
  sgAddChild(vitrinePodest,vitrineFront);
  sgAddChild(vitrineFront,vitrineBack);
  sgAddChild(vitrineBack,vitrineLeft);
  sgAddChild(vitrineLeft,vitrineRigth);
  sgAddChild(vitrineRigth,vitrineTop);


  sgAddChild(root, particleLight);

  // render visibility of objects
  root->shouldRender = 0;
  house->shouldRender = 0;
  vitrine1->shouldRender =0;
  vitrine2->shouldRender =0;
  vitrine3->shouldRender =0;
  vitrine4->shouldRender =0;
  sun->shouldRender = 1;
  
  // particleObject->shouldRender = 1;

  // shaders
  Shader* textureShader = createShader("shaders/texture.vert", "shaders/texture.frag");
  sun->shader = textureShader;
  earth->shader = textureShader;
  moon->shader = textureShader;
  houseWindow1->shader = textureShader;
  houseWindow2->shader = textureShader;
  houseWindow3->shader = textureShader;
  houseWindow4->shader = textureShader;
  houseWindow5->shader = textureShader;
  houseWindow6->shader = textureShader;
  houseWindow7->shader = textureShader;
  houseWindow8->shader = textureShader;
  water->shader = createShader("shaders/water.vert", "shaders/water.frag");
  //mountRushmore->shader = createShader("shaders/bjarne.vert", "shaders/bjarne.frag");
  bjarneLight->shader = createShader("shaders/lightsource.vert", "shaders/lightsource.frag");
  skyboxSun->shader = bjarneLight->shader;
  boat->shader = createShader("shaders/boat.vert", "shaders/boat.frag");
  rmRenderer->shader = createShader("shaders/raymarching.vert", "shaders/raymarching.frag");
  rmDisplay->shader = createShader("shaders/rmDisplay.vert", "shaders/rmDisplay.frag");
  Shader* houseShader = createShader("shaders/texture.vert", "shaders/house.frag");
  houseFloor->shader = textureShader;
  houseFundamentBack->shader = houseShader;
  houseFundamentFront_L->shader = houseShader;
  houseFundamentFront_M->shader = houseShader;
  houseFundamentFront_R->shader = houseShader;
  houseFundamentLeft->shader = houseShader;
  houseFundamentRight->shader = houseShader;
  houseRoof->shader = textureShader;
  ropes->shader = createShader("shaders/ropes.vert","shaders/texture.frag");
  Shader* baloonShader = createShader("shaders/baloon.vert","shaders/baloon.frag");
  baloon1->shader = baloonShader;
  baloon2->shader = baloonShader;
  //mountRushmore->shader = createShader("shaders/mountRushmore.vert","shaders/mountRushmore.frag");

  //Vitrine 1, gibt es eine Möglichkeit, diese wiederzuverwenden?
  vitrinePodest->shader = textureShader;
  vitrineFront->shader = textureShader;
  vitrineBack->shader = textureShader;
  vitrineRigth->shader = textureShader;
  vitrineLeft->shader = textureShader;
  vitrineTop->shader = textureShader;

  particleLight->shader = textureShader;
  
  particleObject->shader = createShader("shaders/particle.vert", "shaders/particle.frag");

  rmRenderer->shader = createShader("shaders/raymarching.vert", "shaders/raymarching.frag");
  rmDisplay->shader = createShader("shaders/texture.vert", "shaders/rmDisplay.frag");

  // textures
  loadTexture(sun, "textures/sun.png", 0);
  loadTexture(earth, "textures/earth_day.png", 0);
  loadTexture(moon, "textures/moon.png", 0);
  loadTexture(houseWindow1, "textures/window_blue.png", 0);
  loadTexture(houseWindow2, "textures/window_blue.png", 0);
  loadTexture(houseWindow3, "textures/window_blue.png", 0);
  loadTexture(houseWindow4, "textures/window_blue.png", 0);
  loadTexture(houseWindow5, "textures/window_blue.png", 0);
  loadTexture(houseWindow6, "textures/window_blue.png", 0);
  loadTexture(houseWindow7, "textures/window_blue.png", 0);
  loadTexture(houseWindow8, "textures/window_blue.png", 0);
  loadTexture(boat, "textures/boat.png", 0);
  loadTexture(water, "textures/water.png", 0);
  loadTexture(houseFloor, "textures/wood_floor.png",0);
  loadTexture(houseFundamentFront_L, "textures/brick_texture.png",0);
  loadTexture(houseFundamentFront_M, "textures/brick_texture.png",0);
  loadTexture(houseFundamentFront_R, "textures/brick_texture.png",0);
  loadTexture(houseFundamentBack, "textures/brick_texture.png",0);
  loadTexture(houseFundamentLeft, "textures/brick_texture.png",0);
  loadTexture(houseFundamentRight, "textures/brick_texture.png",0);
  loadTexture(houseRoof, "textures/roof.jpg",0);
  loadTexture(ropes, "textures/rope1.jpg",0);
  
  
  loadTexture(vitrinePodest, "textures/black_marble_texture.jpg",0);
  loadTexture(vitrineTop,"textures/black_marble_texture.jpg",0);
  loadTexture(vitrineFront,"textures/podest_glass.png",0);
  loadTexture(vitrineBack,"textures/podest_glass.png",0);
  loadTexture(vitrineRigth,"textures/podest_glass.png",0);
  loadTexture(vitrineLeft,"textures/podest_glass.png",0);
  Texture rmTexture = loadTexture(rmDisplay, NULL, 0);
  attachRenderTexture(rmRenderer->renderTarget, rmTexture);

  // draw functions
  sun->draw = &drawWithTexture;
  earth->draw = &drawWithTexture;
  moon->draw = &drawWithTexture;
  houseWindow1->draw = &drawWithTexture;
  houseWindow2->draw = &drawWithTexture;
  houseWindow3->draw = &drawWithTexture;
  houseWindow4->draw = &drawWithTexture;
  houseWindow5->draw = &drawWithTexture;
  houseWindow6->draw = &drawWithTexture;
  houseWindow7->draw = &drawWithTexture;
  houseWindow8->draw = &drawWithTexture;
  water->draw = &drawWater;
  //mountRushmore->draw = &drawBjarne;
  bjarneLight->draw = &drawBjarneLight;
  skyboxSun->draw = &drawBjarneLight;
  boat->draw = &drawBoat;
  rmRenderer->draw = &drawRMRenderer;
  rmDisplay->draw = &drawTextures;
  houseFloor->draw = &drawWithTexture;
  houseFundamentFront_L->draw = &drawWithTexture;
  houseFundamentFront_M->draw = &drawWithTexture;
  houseFundamentFront_R->draw = &drawWithTexture;
  houseFundamentBack->draw = &drawWithTexture;
  houseFundamentLeft->draw = &drawWithTexture;
  houseFundamentRight->draw = &drawWithTexture;
  houseRoof->draw = &drawWithTexture;
  ropes->draw = &drawWithTexture;
  baloon1->draw = &drawBjarne;
  baloon2->draw = &drawBjarne;
  vitrinePodest->draw = &drawWithTexture;
  particleLight->draw = &drawWithTexture;
  vitrineFront->draw = &drawWithTexture;
  vitrineBack->draw = &drawWithTexture;
  vitrineRigth->draw = &drawWithTexture;
  vitrineLeft->draw = &drawWithTexture;
  vitrineTop->draw = &drawWithTexture;
  //mountRushmore->draw = &drawBjarne;
  particleObject->draw = &drawParticles;
  
  rmDisplay->draw = &drawWithTexture;
  rmRenderer->draw = &drawRMRenderer;

  // animations
  sun->animate = &sunAnimation;
  earth->animate = &earthAnimation;
  moon->animate = &moonAnimation;
  bjarneLight->animate = &bjarneLightAnimation;
  boat->animate = &boatAnimation;
  rmDisplay->animate = &rmDisplayAnimation;

  // materials
  //mountRushmore->material = rubin;
  baloon1->material = rubin;
  baloon2->material = rubin;
  particleObject->material = rubin;

  // setObjectPosition(water, 0, 0, 20);
  //setObjectPosition(mountRushmore, 0, 0.9, 0);
  setObjectPosition(particleLight, 1.0,2.0,0.4);
  setObjectPosition(vitrine1,11.25,-15,16);
  setObjectPosition(vitrine2,11.25,-15,-16);
  setObjectPosition(vitrine3,-11.25,-15,16);
  setObjectPosition(vitrine4,-11.25,-15,-16);

  setObjectPosition(rmRenderer, -15, 0, 0);
  setObjectPosition(rmDisplay, -10, 0, 0);
  // setObjectPosition(rmRenderer, 0, 1, 0);
  // setObjectPosition(rmDisplay, 0, 1, 0); 
  setObjectPosition(particleObject, -1, -1, 1);

  setObjectPosition(skyboxSun,10.0,40.0,50.0);
  setObjectScale(particleObject, 0.15, 0.15, 0.15);
  setObjectScale(sun, 0.5,0.5,0.5);
  setObjectScale(bjarneLight, 0.3,0.3,0.3);
  setObjectScale(house,0.1,0.1,0.1);
  //setObjectScale(mountRushmore,0.1,0.1,0.1);
  //setObjectScale(boat, 0.5, 0.5, 0.5);
  setObjectScale(water, 0.5, 0.5, 0.5);
  //setObjectScale(houseFloor, 0.2,0.2,0.2);
  // setObjectScale(vitrinePodest,1.2,1.2,1.2);
  setObjectScale(particleLight,0.1,0.1,0.1);
  //setObjectRotation(mountRushmore,0.0,180.0,0.0);


  LightSource* light = createLight();
  Vec4 ambient = {1,1,1,1};
  Vec4 diffuse = {1,1,1,1};
  Vec4 specular = {1,1,1,1};  
  light->diffuse = diffuse;
  light->specular = specular;

  bjarneLight->light=light;
  //addLightAffectedBy(mountRushmore, light);

  LightSource* sunLight = createLight();
  Vec4 sun_ambient = {1,1,1,1};
  Vec4 sun_diffuse = {1,1,1,1};
  Vec4 sun_specular = {1,1,1,1};  
  sunLight->diffuse = sun_diffuse;
  sunLight->specular = sun_specular;

  skyboxSun->light = sunLight;
  addLightAffectedBy(baloon1,sunLight);
  addLightAffectedBy(baloon2,sunLight);


  vitrineBack->isTransparent = 1; //Irgenwie spackt das rum
  vitrineFront->isTransparent = 1;
  vitrineLeft->isTransparent = 1;
  vitrineRigth->isTransparent = 1;
  houseWindow1->isTransparent = 1;
  houseWindow2->isTransparent = 1;
  houseWindow3->isTransparent = 1;
  houseWindow4->isTransparent = 1;
  houseWindow5->isTransparent = 1;
  houseWindow6->isTransparent = 1;
  houseWindow7->isTransparent = 1;
  houseWindow8->isTransparent = 1;
  
  rmDisplay->isTransparent = 0;

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
