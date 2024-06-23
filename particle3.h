#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "object.h"

#define MAX_PARTICLES 5000

typedef struct {
    GLfloat position[3];
    GLfloat velocity[3];
    GLfloat rotation[3];
    GLfloat rotationOverLifetime[3];
    GLfloat scale;
    GLfloat lifetime;
    GLfloat totalLifetime;
    GLfloat startSize;
    GLfloat color[3];
} Particle;

typedef struct {
    int isInitialized;
    int startCount;
    Particle particles[MAX_PARTICLES];
    int particleCount;
    GLfloat emissionTimer;
    GLfloat emissionAmount;
    GLfloat emissionRange[3];
    GLfloat sizeRange[2];
    GLfloat minVelocity[3];
    GLfloat maxVelocity[3];
    GLfloat minRotation[3];
    GLfloat maxRotation[3];
    GLfloat minRotationOverLifetime[3];
    GLfloat maxRotationOverLifetime[3];
    GLfloat minLifetime;
    GLfloat maxLifetime;
    GLfloat fadeInOutScale;
    GLfloat startColorTint[3];
    GLfloat endColorTint[3];
    int drawInstanced;
    GLfloat position[3];
} ParticleSystem;

// particle.h
ParticleSystem ps;

void RandomRange(float* result, float min, float max) {
    *result = ((float)rand() / RAND_MAX) * (max - min) + min;
}

void InitParticleSystem() {
    ps.isInitialized = 0;
    ps.particleCount = 0;
    ps.startCount = 1;
    ps.emissionTimer = 0.0f;
    ps.emissionAmount = 60.0f;
    ps.emissionRange[0] = 0.5f; ps.emissionRange[1] = 0.0f; ps.emissionRange[2] = 0.5f;
    ps.sizeRange[0] = 0.01f; ps.sizeRange[1] = 0.02f;
    ps.minVelocity[0] = 0.0f; ps.minVelocity[1] = 0.15f; ps.minVelocity[2] = 0.0f;
    ps.maxVelocity[0] = 0.0f; ps.maxVelocity[1] = 0.45f; ps.maxVelocity[2] = 0.0f;
    ps.minRotation[0] = 0.0f; ps.minRotation[1] = 0.0f; ps.minRotation[2] = 0.0f;
    ps.maxRotation[0] = 360.0f; ps.maxRotation[1] = 360.0f; ps.maxRotation[2] = 360.0f;
    ps.minRotationOverLifetime[0] = 0.0f; ps.minRotationOverLifetime[1] = 0.0f; ps.minRotationOverLifetime[2] = 0.0f;
    ps.maxRotationOverLifetime[0] = 30.0f; ps.maxRotationOverLifetime[1] = 30.0f; ps.maxRotationOverLifetime[2] = 30.0f;
    ps.minLifetime = 2.2f;
    ps.maxLifetime = 3.3f;
    ps.fadeInOutScale = 0.3f;
    ps.startColorTint[0] = 1.0f; ps.startColorTint[1] = 1.0f; ps.startColorTint[2] = 1.0f;
    ps.endColorTint[0] = 1.0f; ps.endColorTint[1] = 1.0f; ps.endColorTint[2] = 1.0f;
    ps.drawInstanced = 0; // Instanced drawing currently not implemented
    ps.position[0] = 0.0f; ps.position[1] = 0.0f; ps.position[2] = 0.0f;
}

void AddParticle() {
    if (ps.particleCount >= MAX_PARTICLES) {
        return;
    }

    Particle* p = &ps.particles[ps.particleCount++];
    RandomRange(&p->position[0], ps.position[0] - ps.emissionRange[0] / 2, ps.position[0] + ps.emissionRange[0] / 2);
    RandomRange(&p->position[1], ps.position[1] - ps.emissionRange[1] / 2, ps.position[1] + ps.emissionRange[1] / 2);
    RandomRange(&p->position[2], ps.position[2] - ps.emissionRange[2] / 2, ps.position[2] + ps.emissionRange[2] / 2);

    RandomRange(&p->velocity[0], ps.minVelocity[0], ps.maxVelocity[0]);
    RandomRange(&p->velocity[1], ps.minVelocity[1], ps.maxVelocity[1]);
    RandomRange(&p->velocity[2], ps.minVelocity[2], ps.maxVelocity[2]);

    RandomRange(&p->rotation[0], ps.minRotation[0], ps.maxRotation[0]);
    RandomRange(&p->rotation[1], ps.minRotation[1], ps.maxRotation[1]);
    RandomRange(&p->rotation[2], ps.minRotation[2], ps.maxRotation[2]);

    RandomRange(&p->rotationOverLifetime[0], ps.minRotationOverLifetime[0], ps.maxRotationOverLifetime[0]);
    RandomRange(&p->rotationOverLifetime[1], ps.minRotationOverLifetime[1], ps.maxRotationOverLifetime[1]);
    RandomRange(&p->rotationOverLifetime[2], ps.minRotationOverLifetime[2], ps.maxRotationOverLifetime[2]);

    RandomRange(&p->startSize, ps.sizeRange[0], ps.sizeRange[1]);
    p->scale = p->startSize;

    RandomRange(&p->totalLifetime, ps.minLifetime, ps.maxLifetime);
    p->lifetime = 0.0f;

    p->color[0] = ps.startColorTint[0];
    p->color[1] = ps.startColorTint[1];
    p->color[2] = ps.startColorTint[2];
}

void UpdateParticle(Particle* p, float deltaTime) {
    p->lifetime += deltaTime;
    if (p->lifetime >= p->totalLifetime) {
        p->lifetime = p->totalLifetime;
    }

    p->position[0] += p->velocity[0] * deltaTime;
    p->position[1] += p->velocity[1] * deltaTime;
    p->position[2] += p->velocity[2] * deltaTime;

    p->rotation[0] += p->rotationOverLifetime[0] * deltaTime;
    p->rotation[1] += p->rotationOverLifetime[1] * deltaTime;
    p->rotation[2] += p->rotationOverLifetime[2] * deltaTime;

    if (ps.fadeInOutScale > 0.0f) {
        if (p->lifetime <= ps.fadeInOutScale) {
            p->scale = (p->lifetime / ps.fadeInOutScale) * p->startSize;
        } else if (p->lifetime >= (p->totalLifetime - ps.fadeInOutScale)) {
            float lastPortion = p->totalLifetime - ps.fadeInOutScale;
            p->scale = (1.0f - ((p->lifetime - lastPortion) / ps.fadeInOutScale)) * p->startSize;
        } else {
            p->scale = p->startSize;
        }
    }

    float interp = p->lifetime / p->totalLifetime;
    p->color[0] = interp * ps.endColorTint[0] + (1.0f - interp) * ps.startColorTint[0];
    p->color[1] = interp * ps.endColorTint[1] + (1.0f - interp) * ps.startColorTint[1];
    p->color[2] = interp * ps.endColorTint[2] + (1.0f - interp) * ps.startColorTint[2];
}

void DrawParticle(Particle *p){
    printf("Drawing particle funtion is empty..\n");
}

// void DrawParticle(Particle* p) {
//     // Save current matrix
//     glPushMatrix();

//     // Translate to the particle's position
//     glTranslatef(p->position[0], p->position[1], p->position[2]);
//     printf("position: %f %f %f\n", p->position[0], p->position[1], p->position[2]);

//     // Rotate according to the particle's rotation
//     glRotatef(p->rotation[0], 1.0f, 0.0f, 0.0f);
//     glRotatef(p->rotation[1], 0.0f, 1.0f, 0.0f);
//     glRotatef(p->rotation[2], 0.0f, 0.0f, 1.0f);

//     // Scale the particle
//     glScalef(p->scale, p->scale, p->scale);

//     // Set the particle's color
//     glColor3f(p->color[0], p->color[1], p->color[2]);

//     // Draw the particle (assuming a simple quad for this example)
//     glBegin(GL_QUADS);
//         glVertex3f(-0.5f, -0.5f, 0.0f);
//         glVertex3f( 0.5f, -0.5f, 0.0f);
//         glVertex3f( 0.5f,  0.5f, 0.0f);
//         glVertex3f(-0.5f,  0.5f, 0.0f);
//     glEnd();


//     // Restore previous matrix
//     glPopMatrix();
// }

void DrawGameObject() {
    if (!ps.isInitialized) {
        printf("initiating particle system\n");
        ps.isInitialized = 1;
        for (int i = 0; i < ps.startCount; i++) {
            AddParticle();
        }
    }

    ps.emissionTimer += 0.016f; // Assuming a fixed delta time, replace with actual delta time
    if (ps.emissionTimer >= 1.0f / ps.emissionTimer) {
        int particleCount = (int)floor(ps.emissionTimer / (1.0f / ps.emissionAmount));
        if (particleCount > MAX_PARTICLES) {
            particleCount = MAX_PARTICLES;
        }
        ps.emissionTimer = 0.0f;
        if (particleCount < MAX_PARTICLES) {
            for (int i = 0; i < particleCount; i++) {
                AddParticle();
            }
        }
    }
    // falls zu viele
    if (ps.particleCount > MAX_PARTICLES) {
        printf("need to delete particles\n");

        DestroyParticle(&ps.particles[0]);
        for (int i = 1; i < ps.particleCount; i++) {
            ps.particles[i - 1] = ps.particles[i];
        }
        ps.particleCount--;
    }

    for (int i = 0; i < ps.particleCount; i++) {
        UpdateParticle(&ps.particles[i],deltaTime);
    }

    if (ps.startColorTint[0] == ps.endColorTint[0] &&
        ps.startColorTint[1] == ps.endColorTint[1] &&
        ps.startColorTint[2] == ps.endColorTint[2]) {
        // obj->changesColor = false;
    } else {
        // obj->changesColor = true;
    }

    // Instanced Drawing (currently not working)
    // if (obj->PS_DrawInstanced && obj->PS_ParticlesCount > 0) {
    //     BindData();
    // }
}


void UpdateAndDrawParticles(float deltaTime) {

    
    for (int i = 0; i < ps.particleCount; ++i) {
        UpdateParticle(&ps.particles[i], deltaTime);
        DrawParticle(&ps.particles[i]);
    }
}

void DestroyParticle(Particle* p) {
    free(p);
    p = NULL;
}


// void UpdateAndDrawParticles(float deltaTime) {
//     for (int i = 0; i < ps.particleCount; ++i) {
//         UpdateParticle(&ps.particles[i], deltaTime);
//         DrawParticle(&ps.particles[i]);
//     }
// }
