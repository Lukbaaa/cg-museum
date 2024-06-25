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

#define NUM_PARTICLES 50
#define PARTICLES_X -1.1
#define PARTICLES_Y 1.2
#define PARTICLES_Z -1.6

#define PARTICLE_SIZE 0.001

#define PARTICLE_LIFE 1.4f




typedef struct {
    float position[3];
    float velocity[3];
    float life;
} Particle;

int boolean;

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
float startVel[3];

Object* particleObject;
Object* partList[NUM_PARTICLES];
// Object* ParticleSystemObject
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


/**
* Initialisiert die Buffer arrays in denen die Partikel gespeichert werden
*/
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

/**
* Initialisiert das Partikelsystem
*/
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
        ps.particles[i].position[0] = ps.position[0];
        ps.particles[i].position[1] = ps.position[1];
        ps.particles[i].position[2] = ps.position[2];
        ps.particles[i].velocity[0] = (rand() % 100) / 500.0f - 0.1f;
        ps.particles[i].velocity[1] = (rand() % 100) / 500.0f - 0.1f;
        ps.particles[i].velocity[2] = (rand() % 100) / 500.0f - 0.1f;
        startVel[0] = ps.particles[i].velocity[0];
        startVel[1] = ps.particles[i].velocity[1];
        startVel[2] = ps.particles[i].velocity[2];
        ps.particles[i].life = PARTICLE_LIFE;
    }

    initBuffers();
}

/**
* Partikeln position und velocity werden aktualisiert
* @param deltaTime - Zeit seit dem letzten Frame
*/
void updateParticles(float deltaTime) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        ps.particles[i].life -= deltaTime/1.6;
        if (ps.particles[i].life > 0.0f) {
            ps.particles[i].position[0] -= ps.particles[i].velocity[0] * deltaTime;
            ps.particles[i].position[1] -= ps.particles[i].velocity[1] * deltaTime;
            ps.particles[i].position[2] -= ps.particles[i].velocity[2] * deltaTime;

        }
        else {
            ps.particles[i].velocity[0] = -ps.particles[i].velocity[0];
            ps.particles[i].velocity[1] = -ps.particles[i].velocity[1];
            ps.particles[i].velocity[2] = -ps.particles[i].velocity[2];
            ps.particles[i].life = PARTICLE_LIFE;
        }

    }
}

#endif // !PARTICLE2_H
