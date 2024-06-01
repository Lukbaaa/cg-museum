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



LightSource light = 
  {
    {-12.0f, 2.0f, 2.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f, 1.0f}
  };

GLfloat model[16];
GLfloat view[16];
GLfloat projection[16];
GLfloat normalMatrix[16];

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

  glClearColor((1/255.0f)*191, (1/255.0f)*217, (1/255.0f)*204, 1.0f);
  glViewport(0, 0, 800, 800);
}

void createObjects(void) {

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
  copyMat(out, view);
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
  copyMat(out, temp);
}

void changeView() {
  center[0] = camPos[0] + camFront[0];
  center[1] = camPos[1] + camFront[1];
  center[2] = camPos[2] + camFront[2];
  lookAt(view, camPos, center, camUp);
} 
 
// noch überlegen wie unterschiedliche objekte unterschiedlich bewegt werden sollen
// maybe auch function pointer
void changeTransform(Object* obj) {
  identity(model);

  GLfloat t[16];
  GLfloat s[16];

  setRotation(obj, 0, glfwGetTime()*10, 0);

  createTransMat3f(t, obj->transform.position[0], obj->transform.position[1], obj->transform.position[2]);
  createScaleMat3f(s, obj->transform.scale[0], obj->transform.scale[1], obj->transform.scale[2]);
  translate(model, model, t);
  rotatex(model, model, obj->transform.rotation[0]);
  rotatey(model, model, obj->transform.rotation[1]);
  rotatez(model, model, obj->transform.rotation[2]);
  scale(model, model, s);

}

void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   
  changeView();
  
  for (int i = 0; i < objCount; i++) {
    if (!objects[i]->shouldRender) {
      continue;
    }
    int program = objects[i]->shader->program;
    glUseProgram(program);

    inverse4(model, normalMatrix);
    transpose4(normalMatrix, normalMatrix);
    changeTransform(objects[i]);

    if (objects[i]->shader->setUniforms != NULL) {
      objects[i]->shader->setUniforms(objects[i]);
    }

    for (int j = 0; j < objects[i]->textureCount; j++) {
      char uniformName[15]; 
      sprintf(uniformName, "tex%d", j);
      glUniform1i(glGetUniformLocation(program, uniformName), j);
      glActiveTexture(GL_TEXTURE0+j);
      glBindTexture(GL_TEXTURE_2D, objects[i]->textures[j]);
    } 

    glBindVertexArray(objects[i]->vao);
    glDrawArrays(GL_TRIANGLES, 0, objects[i]->vertCount);
  }
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
  createObjects();

  identity(model);
  center[0] = camPos[0] + camFront[0];
  center[1] = camPos[1] + camFront[1];
  center[2] = camPos[2] + camFront[2];
  lookAt(view, camPos, center, camUp);
  perspective(projection, 45.0f, 800/800, 0.1, 100);
  
  while (!glfwWindowShouldClose(window)) {

    processInput(window);
    draw();
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  free(objects);
  glfwTerminate();
  return 0;
}
