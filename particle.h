#ifndef PARTICLE_H
#define PARTICLE_H

#define NUM_PARTICLES 100


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

#endif // !PARTICLE_H
