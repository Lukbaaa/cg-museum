#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "object.h"
#include "matrix_functions.h"

#define NUM_PARTICLES 100

typedef struct {
    float position[3];
    float velocity[3];
    float life;
} Particle;

Particle particles[NUM_PARTICLES];

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

void initParticles() {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].position[0] = (rand() % 100) / 100.0f - 0.5f;
        particles[i].position[1] = (rand() % 100) / 100.0f - 0.5f;
        particles[i].position[2] = (rand() % 100) / 100.0f - 0.5f;
        particles[i].velocity[0] = (rand() % 100) / 500.0f - 0.1f;
        particles[i].velocity[1] = (rand() % 100) / 500.0f - 0.1f;
        particles[i].velocity[2] = (rand() % 100) / 500.0f - 0.1f;
        particles[i].life = 5.0f;
    }
}

void updateParticles(float deltaTime) {
    for (int i = 0; i < NUM_PARTICLES; i++) {
        particles[i].life -= deltaTime;
        if (particles[i].life > 0.0f) {
            particles[i].position[0] += particles[i].velocity[0] * deltaTime;
            particles[i].position[1] += particles[i].velocity[1] * deltaTime;
            particles[i].position[2] += particles[i].velocity[2] * deltaTime;
        } else {
            particles[i].position[0] = (rand() % 100) / 100.0f - 0.5f;
            particles[i].position[1] = (rand() % 100) / 100.0f - 0.5f;
            particles[i].position[2] = (rand() % 100) / 100.0f - 0.5f;
            particles[i].velocity[0] = (rand() % 100) / 500.0f - 0.1f;
            particles[i].velocity[1] = (rand() % 100) / 500.0f - 0.1f;
            particles[i].velocity[2] = (rand() % 100) / 500.0f - 0.1f;
            particles[i].life = 5.0f;
        }
    }
}

GLuint VertexArrayID, vertexbuffer, elementbuffer;

void initBuffers() {
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
}

void translate(GLfloat* out, GLfloat in[16], GLfloat v[3]) {
    assert(out != NULL);
    assert(in != NULL);
    assert(v != NULL);

    out[12] = in[0] + v[0];
    out[13] = in[1] + v[1];
    out[14] = in[2] + v[2];
    out[15] = 1.0f;
}

void renderParticles(GLuint MatrixID, Camera* camera) {
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

    for (int i = 0; i < NUM_PARTICLES; i++) {
        if (particles[i].life > 0.0f) {
            GLfloat model[16];
            identity(model);
            translate(model, model, particles[i].position);
            // translate(model, particles[i].position[0], particles[i].position[1], particles[i].position[2]);
            // translate(mat4(1.0f), vec3(particles[i].position[0], particles[i].position[1], particles[i].position[2]));
            // mat4 MVP = Projection * View * Model;
            glUniformMatrix4fv(glGetUniformLocation(MatrixID, "model"), 1, GL_FALSE, model);
            glUniformMatrix4fv(glGetUniformLocation(MatrixID, "view"), 1, GL_FALSE, camera->view);
            glUniformMatrix4fv(glGetUniformLocation(MatrixID, "projection"), 1, GL_TRUE, camera->projection);

            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);
        }
    }

    glDisableVertexAttribArray(0);
}