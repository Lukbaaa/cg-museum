#ifndef PARTICLE_H
#define PARTICLE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>

#include "shader.h"
#include "object.h"

typedef struct Particle {
    GLfloat* position;
    GLfloat* velocity;
    GLfloat* color;
    GLfloat life;
} Particle;

typedef struct {
    Particle* particles;
    unsigned int amount;
    Shader shader;
    void* texture;
    void (*bindTexture)(void *);
    unsigned int VAO;
    unsigned int VBO;
} ParticleGenerator;

void Particle_Init(Particle *p);
void Particle_Destroy(Particle *p);
void ParticleGenerator_Init(ParticleGenerator *pg, Shader shader, void *texture, void (*bindTexture)(void *), unsigned int amount);
void ParticleGenerator_InitParticles(ParticleGenerator *pg);
void ParticleGenerator_InitBuffer(ParticleGenerator *pg);
void ParticleGenerator_Update(ParticleGenerator *pg, float dt, Object *object, unsigned int newParticles, GLfloat offset[2]);
void ParticleGenerator_Draw(ParticleGenerator *pg);
unsigned int ParticleGenerator_FirstUnusedParticle(ParticleGenerator *pg);
void ParticleGenerator_RespawnParticle(Particle *particle, Object *object, GLfloat offset[2]);
void ParticleGenerator_Destroy(ParticleGenerator *pg);

unsigned int lastUsedParticle = 0;

void Particle_Init(Particle *p) {
    p->position = (GLfloat *)malloc(2 * sizeof(GLfloat));
    p->velocity = (GLfloat *)malloc(2 * sizeof(GLfloat));
    p->color = (GLfloat *)malloc(4 * sizeof(GLfloat));
    p->position[0] = 0.0f; p->position[1] = 0.0f;
    p->velocity[0] = 0.0f; p->velocity[1] = 0.0f;
    p->color[0] = 1.0f; p->color[1] = 1.0f; p->color[2] = 1.0f; p->color[3] = 1.0f;
    p->life = 0.0f;
}

void Particle_Destroy(Particle *p) {
    free(p->position);
    free(p->velocity);
    free(p->color);
}

void ParticleGenerator_Init(ParticleGenerator *pg, Shader shader, void *texture, void (*bindTexture)(void *), unsigned int amount) {
    pg->particles = (Particle *)malloc(amount * sizeof(Particle));
    pg->amount = amount;
    pg->shader = shader;
    pg->texture = texture;
    pg->bindTexture = bindTexture;
    glGenVertexArrays(1, &pg->VAO);
    glGenBuffers(1, &pg->VBO);
    ParticleGenerator_InitParticles(pg);
    ParticleGenerator_InitBuffer(pg);
}

void ParticleGenerator_InitParticles(ParticleGenerator *pg) {
    for (unsigned int i = 0; i < pg->amount; ++i) {
        Particle_Init(&pg->particles[i]);
    }
}

void ParticleGenerator_InitBuffer(ParticleGenerator *pg) {
    float particle_quad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glBindVertexArray(pg->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, pg->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glBindVertexArray(0);
}

void ParticleGenerator_Update(ParticleGenerator *pg, float dt, Object *object, unsigned int newParticles, GLfloat offset[2]) {
    for (unsigned int i = 0; i < newParticles; ++i) {
        int unusedParticle = ParticleGenerator_FirstUnusedParticle(pg);
        ParticleGenerator_RespawnParticle(&pg->particles[unusedParticle], object, offset);
    }
    for (unsigned int i = 0; i < pg->amount; ++i) {
        Particle *p = &pg->particles[i];
        p->life -= dt;
        if (p->life > 0.0f) {
            p->position[0] -= p->velocity[0] * dt;
            p->position[1] -= p->velocity[1] * dt;
            p->color[3] -= dt * 2.5f;
        }
    }
}

void Shader_Use(Shader *shader) {
    glUseProgram(shader->program);
}

// Function to set a 2D vector uniform in the shader
void Shader_SetVector2f(Shader *shader, const GLchar *name, GLfloat x, GLfloat y) {
    glUniform2f(glGetUniformLocation(shader->program, name), x, y);
}

// Function to set a 4D vector uniform in the shader
void Shader_SetVector4f(Shader *shader, const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    glUniform4f(glGetUniformLocation(shader->program, name), x, y, z, w);
}

// Function to draw the particle generator
void ParticleGenerator_Draw(ParticleGenerator *pg) {
    // Enable additive blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    // Use the shader program
    Shader_Use(&pg->shader);

    // Iterate over all particles
    for (unsigned int i = 0; i < pg->amount; ++i) {
        Particle *particle = &pg->particles[i];
        // Check if the particle is alive
        if (particle->life > 0.0f) {
            // Set shader uniforms for the current particle
            Shader_SetVector2f(&pg->shader, "offset", particle->position[0], particle->position[1]);
            Shader_SetVector4f(&pg->shader, "color", particle->color[0], particle->color[1], particle->color[2], particle->color[3]);

            // Bind the particle texture using the provided callback
            pg->bindTexture(pg->texture);

            // Bind the VAO for the particle quad
            glBindVertexArray(pg->VAO);
            // Draw the particle quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Unbind the VAO
            glBindVertexArray(0);
        }
    }

    // Reset blending mode to default
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


unsigned int ParticleGenerator_FirstUnusedParticle(ParticleGenerator *pg) {
    for (unsigned int i = lastUsedParticle; i < pg->amount; ++i) {
        if (pg->particles[i].life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    for (unsigned int i = 0; i < lastUsedParticle; ++i) {
        if (pg->particles[i].life <= 0.0f) {
            lastUsedParticle = i;
            return i;
        }
    }
    lastUsedParticle = 0;
    return 0;
}

void ParticleGenerator_RespawnParticle(Particle *particle, Object *object, GLfloat offset[2]) {
    float random = ((rand() % 100) - 50) / 10.0f;
    float rColor = 0.5f + ((rand() % 100) / 100.0f);
    particle->position[0] = object->transform.position.x + random + offset[0];
    particle->position[1] = object->transform.position.y + random + offset[1];
    particle->color[0] = rColor;
    particle->color[1] = rColor;
    particle->color[2] = rColor;
    particle->color[3] = 1.0f;
    particle->life = 1.0f;
    particle->velocity[0] = 0.0f; // Assuming no velocity as Object has none
    particle->velocity[1] = 1.0f;
}

void ParticleGenerator_Destroy(ParticleGenerator *pg) {
    for (unsigned int i = 0; i < pg->amount; ++i) {
        Particle_Destroy(&pg->particles[i]);
    }
    free(pg->particles);
    glDeleteVertexArrays(1, &pg->VAO);
    glDeleteBuffers(1, &pg->VBO);
}

#endif
