#ifndef PARTICLE2_H
#define PARTICLE2_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "object.h"
#include "matrix_functions.h"
#include "light.h"
#include "materials.h"

#define NUM_PARTICLES 100
#define PARTICLES_X 10
#define PARTICLES_Y 10
#define PARTICLES_Z 10

#define PARTICLE_SIZE 0.01

#define PARTICLE_LIFE 10.0f


typedef struct {
    float position[3];
    float velocity[3];
    float life;
} Particle;

typedef struct {
    Particle particles[NUM_PARTICLES];
    float dt;
    float position[3];
} ParticleSystem;

// LightSource* light;
Vec4 ambient = {1,1,1,1};
Vec4 diffuse = {1,1,1,1};
Vec4 specular = {1,1,1,1};
// Vec3 lightPosition = {0.0,0.0,-0.4};
// Material material;

Object* particleObject;
Object* partList[NUM_PARTICLES];
// Object* ParticleSystemObject;

ParticleSystem ps;

// Particle particles[NUM_PARTICLES];

const GLfloat cube_vertices[] = {
    -0.05f, -0.05f, -0.05f,
     0.05f, -0.05f, -0.05f,
     0.05f,  0.05f, -0.05f,
    -0.05f,  0.05f, -0.05f,
    -0.05f, -0.05f,  0.05f,
     0.05f, -0.05f,  0.05f,
     0.05f,  0.05f,  0.05f,
    -0.05f,  0.05f,  0.05f,
};

const GLuint cube_indices[] = {
    0, 1, 2, 2, 3, 0, // back face
    4, 5, 6, 6, 7, 4, // front face
    4, 5, 1, 1, 0, 4, // bottom face
    7, 6, 2, 2, 3, 7, // top face
    4, 7, 3, 3, 0, 4, // left face
    5, 6, 2, 2, 1, 5, // right face
};

GLuint vertexbufferparticles, elementbufferparticles;

void initBuffers() {

    // glGenVertexArrays(1, &particleObject->vao);
    glBindVertexArray(particleObject->vao);

    glGenBuffers(1, &vertexbufferparticles);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbufferparticles);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbufferparticles);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbufferparticles);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

}
void initParticles() {

    particleObject->light = createLight();  
    particleObject->light->ambient = ambient;
    particleObject->light->diffuse = diffuse;
    particleObject->light->specular = specular;
    ps.position[0] = PARTICLES_X;
    ps.position[1] = PARTICLES_Y;
    ps.position[2] = PARTICLES_Z;
    particleObject->light->position= arrayToVec3(ps.position);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        // particles[i].position[0] = (rand() % 100) / 100.0f - 0.5f;
        // particles[i].position[1] = (rand() % 100) / 100.0f - 0.5f;
        // particles[i].position[2] = (rand() % 100) / 100.0f - 0.5f;
        ps.particles[i].position[0] = ps.position[0];
        ps.particles[i].position[1] = ps.position[1];
        ps.particles[i].position[2] = ps.position[2];
        ps.particles[i].velocity[0] = (rand() % 100) / 500.0f - 0.1f;
        ps.particles[i].velocity[1] = (rand() % 100) / 500.0f - 0.1f;
        ps.particles[i].velocity[2] = (rand() % 100) / 500.0f - 0.1f;
        // ps.particles[i].velocity[0] = ps.position[0];
        // ps.particles[i].velocity[1] = ps.position[1];
        // ps.particles[i].velocity[2] = ps.position[2];
        ps.particles[i].life = PARTICLE_LIFE;
    }
    initBuffers();
}

void updateParticles(float deltaTime) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        ps.particles[i].life -= deltaTime;
        if (ps.particles[i].life > 0.0f) {
            ps.particles[i].position[0] += ps.particles[i].velocity[0] * deltaTime;
            ps.particles[i].position[1] += ps.particles[i].velocity[1] * deltaTime;
            ps.particles[i].position[2] += ps.particles[i].velocity[2] * deltaTime;
        } else {
            // particles[i].position[0] = (rand() % 100) / 100.0f - 0.5f;
            // particles[i].position[1] = (rand() % 100) / 100.0f - 0.5f;
            // particles[i].position[2] = (rand() % 100) / 100.0f - 0.5f;
            ps.particles[i].position[0] = ps.position[0];
            ps.particles[i].position[1] = ps.position[1];
            ps.particles[i].position[2] = ps.position[2];
            ps.particles[i].velocity[0] = (rand() % 100) / 500.0f - 0.1f;
            ps.particles[i].velocity[1] = (rand() % 100) / 500.0f - 0.1f;
            ps.particles[i].velocity[2] = (rand() % 100) / 500.0f - 0.1f;
            ps.particles[i].life = PARTICLE_LIFE;
        }
    }
}



// void renderParticles(GLuint MatrixID, Camera* camera) {
//     glUseProgram(MatrixID);
//     glEnableVertexAttribArray(0);
//     glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
//     for (int i = 0; i < NUM_PARTICLES; i++) {
//         if (ps.particles[i].life > 0.0f) {
//             GLfloat model[16];
//             identity(model);
//             translateVector(model, model, ps.particles[i].position);

//             GLfloat normalMatrix[16];
//             inverse4(normalMatrix, model);
//             transpose4(normalMatrix, normalMatrix);
//             glUniformMatrix4fv(glGetUniformLocation(MatrixID, "normalMatrix"), 1, GL_FALSE, normalMatrix);

//             glUniform1f(glGetUniformLocation(MatrixID,"dt"),ps.particles[i].life);
//             glUniformMatrix4fv(glGetUniformLocation(MatrixID, "model"), 1, GL_FALSE, model);
//             glUniformMatrix4fv(glGetUniformLocation(MatrixID, "view"), 1, GL_FALSE, camera->view);
//             glUniformMatrix4fv(glGetUniformLocation(MatrixID, "projection"), 1, GL_TRUE, camera->projection);

//             glUniform4fv(glGetUniformLocation(MatrixID, "material.emissive"), 1, material.emissive);
//             glUniform4fv(glGetUniformLocation(MatrixID, "material.ambient"), 1, material.ambient);
//             glUniform4fv(glGetUniformLocation(MatrixID, "material.diffuse"), 1, material.diffuse);
//             glUniform4fv(glGetUniformLocation(MatrixID, "material.specular"), 1, material.specular);
//             glUniform1f(glGetUniformLocation(MatrixID, "material.shininess"), material.shininess);
            
//             glUniform3fv(glGetUniformLocation(MatrixID, "light.position"), 1, (float*)&lightPosition); 
//             glUniform4fv(glGetUniformLocation(MatrixID, "light.ambient"), 1, (float*)&(light->ambient.r));
//             glUniform4fv(glGetUniformLocation(MatrixID, "light.diffuse"), 1, (float*)&(light->diffuse.r));
//             glUniform4fv(glGetUniformLocation(MatrixID, "light.specular"), 1, (float*)&(light->specular.r));

            

//             glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
//         }
//     }

//     glDisableVertexAttribArray(0);
// }

#endif // !PARTICLE2_H
