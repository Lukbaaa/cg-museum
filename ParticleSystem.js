class ParticleSystem {
    PS_Particles = [];
    PS_StartCount = 1;
    PS_EmissionAmount = 60.0;
    PS_MaxParticles = 5000;
    PS_EmisionRange = [0.5, 0.0, 0.5];
    PS_SizeRange = [0.01, 0.02];
    PS_MinVelocity = [0.0, 0.15, 0.0];
    PS_MaxVelocity = [0.0, 0.45, 0.0];
    PS_MinRotation = [0.0, 0.0, 0.0];
    PS_MaxRotation = [360.0, 360.0, 360.0];
    PS_MinRotationOverLifetime = [0.0, 0.0, 0.0];
    PS_MaxRotationOverLifetime = [30.0, 30.0, 30.0];
    PS_MinLifetime = 2.2;
    PS_MaxLifetime = 3.3;
    PS_FadeInOutScale = 0.3;
    PS_Material = null;
    PS_StartColorTint = [1.0, 1.0, 1.0];
    PS_EndColorTint = [1.0, 1.0, 1.0];
    PS_DrawInstanced = false; //Funktioniert aktuell nicht

    position = [0.0, 0.0, 0.0];

    #PS_Verts;
    #PS_Initialized = false;
    #PS_EmissionTimer = 0.0;

    name = "ParticleSystem";
    dontDrawAutomatic = false;
    origin = -1;
    modelname = -1;

    changesColor = false;

    /**
     * Erzeugt ein neues ParticleSystem
     * @param {[]} vnuv Modelldaten aus der Funktion modelFromObj()
     * @param {number} modelname Origin für alle Partikel
     */
    constructor(vnuv, modelname = -1) {
        this.#PS_Verts = vnuv;
        this.modelname = modelname;
        Globals.gameObjects.push(this);
    }

    DrawGameObject() {
        if(!this.#PS_Initialized) {
            this.#PS_Initialized = true;
            for(let i = 0; i < this.PS_StartCount; i++) {
                this.AddParticle();
            }
        }
        this.#PS_EmissionTimer += GameTime.deltaTime;
        if(this.#PS_EmissionTimer >= 1.0 / this.PS_EmissionAmount) {
            let particleCount = Math.floor(this.#PS_EmissionTimer / (1.0 / this.PS_EmissionAmount));
            if(particleCount > this.PS_MaxParticles) {
                particleCount = this.PS_MaxParticles;
            }
            this.#PS_EmissionTimer = 0.0;
            if(this.PS_Particles.length < this.PS_MaxParticles) {
                if(particleCount == 1) {
                    this.AddParticle();
                }else {
                    for(let i = 0; i < particleCount; i++) {
                        this.AddParticle();
                    }
                }
            }
        }
        if(this.PS_Particles.length > this.PS_MaxParticles) {
            let toDestroy = this.PS_Particles.shift();
            toDestroy.P_GameObject.Destroy(false);
        }
        for(let i = 0; i < this.PS_Particles.length; i++) {
            this.PS_Particles[i].UpdateParticle();
        }
        if(this.PS_StartColorTint[0] == this.PS_EndColorTint[0] && this.PS_StartColorTint[1] == this.PS_EndColorTint[1] && this.PS_StartColorTint[2] == this.PS_EndColorTint[2]) {
            this.changesColor = false;
        }else {
            this.changesColor = true;
        }
        //Instanced Drawing FUNKTIONIERT AKTUELL NICHT
        if(this.PS_DrawInstanced && this.PS_Particles.length > 0) {
            let gl = Globals.gl;
            /*const ext = gl.getExtension('ANGLE_instanced_arrays');
            if (!ext) {
                return alert('need ANGLE_instanced_arrays');
            }*/ 
            this.PS_Particles[0].P_GameObject.BindData();
            let program = Globals.program;
            let matrixBuffer = gl.createBuffer();
            let matrixData = [];
            for(let i = 0; i < this.PS_Particles.length; i++) {
                let currentMatrix = this.PS_Particles[i].P_GameObject.GO_CreateMatrices(false);
                for(let e = 0; e < currentMatrix.length; e++) {
                    matrixData.push(currentMatrix[e]);
                }
            }
            gl.bindBuffer(gl.ARRAY_BUFFER, matrixBuffer);
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(matrixData), gl.DYNAMIC_DRAW);
            let transformAttribLocation = gl.getAttribLocation(program, "transform");
            for(let i = 0; i < 4; i++) {
                const loc = transformAttribLocation + i;
                gl.enableVertexAttribArray(loc);
                const offset = i * 4;
                gl.vertexAttribPointer(
                    loc,
                    4,
                    gl.FLOAT,
                    gl.FALSE,
                    4 * Float32Array.BYTES_PER_ELEMENT,
                    offset,
                );
                gl.vertexAttribDivisor(loc, 1);
            }
            gl.drawArraysInstanced(
                gl.TRIANGLES,
                0,
                this.PS_Particles[0].P_GameObject.GO_vertices,
                this.PS_Particles.length,
            );
        }
    }

    AddParticle() {
        let newParticle = new GameObject(this.#PS_Verts, this.modelname);
        newParticle.position = [this.position[0] + (Math.random() - 0.5) * this.PS_EmisionRange[0], this.position[1] + (Math.random() - 0.5) * this.PS_EmisionRange[1], this.position[2] + (Math.random() - 0.5) * this.PS_EmisionRange[2]];
        newParticle.rotation = [
            RandomRange(this.PS_MinRotation[0], this.PS_MaxRotation[0]),
            RandomRange(this.PS_MinRotation[1], this.PS_MaxRotation[1]), 
            RandomRange(this.PS_MinRotation[2], this.PS_MaxRotation[2])
        ];
        newParticle.isParticle = true;
        let partSize = RandomRange(this.PS_SizeRange[0], this.PS_SizeRange[1]);
        newParticle.scale = [partSize, partSize, partSize];
        newParticle.material = this.PS_Material;
        newParticle.dontDrawAutomatic = true;
        newParticle.name = "Particle";
        let particleInstance = new Particle(this, newParticle);
        particleInstance.P_RotationOverLifetime = [
            RandomRange(this.PS_MinRotationOverLifetime[0], this.PS_MaxRotationOverLifetime[0]),
            RandomRange(this.PS_MinRotationOverLifetime[1], this.PS_MaxRotationOverLifetime[1]),
            RandomRange(this.PS_MinRotationOverLifetime[2], this.PS_MaxRotationOverLifetime[2]),
        ];
        this.PS_Particles.push(particleInstance);
    }
}

class Particle {
    P_ParticleSystem;
    P_GameObject;
    P_RotationOverLifetime;

    #StartSize;
    #Velocity;
    #Lifetime = 0.0;
    #TotalLifeTime;


    constructor(ps, gm) {
        this.P_ParticleSystem = ps;
        this.P_GameObject = gm;
        this.#StartSize = gm.scale;
        this.#TotalLifeTime = RandomRange(ps.PS_MinLifetime, ps.PS_MaxLifetime);
        let xVel = RandomRange(ps.PS_MinVelocity[0], ps.PS_MaxVelocity[0]);
        let yVel = RandomRange(ps.PS_MinVelocity[1], ps.PS_MaxVelocity[1]);
        let zVel = RandomRange(ps.PS_MinVelocity[2], ps.PS_MaxVelocity[2]);
        this.#Velocity = [xVel, yVel, zVel];
    }

    UpdateParticle() {
        let fadeTime = this.P_ParticleSystem.PS_FadeInOutScale;
        if(fadeTime > 0.0) {
            if(this.#Lifetime <= fadeTime) {
                let currentScale = (this.#Lifetime / fadeTime) * this.#StartSize[0];
                this.P_GameObject.scale = [currentScale, currentScale, currentScale];
            }else if(this.#Lifetime >= (this.#TotalLifeTime - fadeTime)) {
                let lastPortion = this.#TotalLifeTime - fadeTime;
                let currentScale = (1.0 - ((this.#Lifetime - lastPortion) / fadeTime)) * this.#StartSize[0];
                this.P_GameObject.scale = [currentScale, currentScale, currentScale];
            }else {
                this.P_GameObject.scale = [this.#StartSize[0], this.#StartSize[1], this.#StartSize[2]];
            }
        }
        if(this.#Lifetime >= this.#TotalLifeTime) {
            this.P_GameObject.Destroy(false);
            this.P_ParticleSystem.PS_Particles.splice(this.P_ParticleSystem.PS_Particles.indexOf(this), 1);
            return;
        }
        this.P_GameObject.position = [this.P_GameObject.position[0] + this.#Velocity[0] * GameTime.deltaTime, this.P_GameObject.position[1] + this.#Velocity[1] * GameTime.deltaTime, this.P_GameObject.position[2] + this.#Velocity[2] * GameTime.deltaTime];
        this.P_GameObject.Rotate(
            this.P_RotationOverLifetime[0] * GameTime.deltaTime,
            this.P_RotationOverLifetime[1] * GameTime.deltaTime,
            this.P_RotationOverLifetime[2] * GameTime.deltaTime,
        );
        if(this.P_ParticleSystem.changesColor) {
            let interp = this.#Lifetime / this.#TotalLifeTime;
            this.P_GameObject.material.ChangeColorTint([
                interp * this.P_ParticleSystem.PS_EndColorTint[0] + (1.0 - interp) * this.P_ParticleSystem.PS_StartColorTint[0],
                interp * this.P_ParticleSystem.PS_EndColorTint[1] + (1.0 - interp) * this.P_ParticleSystem.PS_StartColorTint[1],
                interp * this.P_ParticleSystem.PS_EndColorTint[2] + (1.0 - interp) * this.P_ParticleSystem.PS_StartColorTint[2],
            ]);
        } 
        if(!this.P_ParticleSystem.PS_DrawInstanced) {
            this.P_GameObject.DrawGameObject();
            Globals.lastOrigin = this.P_GameObject.origin;
        }
        this.#Lifetime += GameTime.deltaTime;
    }
} 

function RandomRange(min, max) {
    return Math.random() * (max - min) + min;
}