#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

#include <time.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <GL/glew.h>

#include "object.h"

void writeArrayToFile(float* array, int size, const char* filename) {
    assert(array != NULL);
    assert(filename != NULL);

    FILE* file = fopen( filename, "w");
    if (file == NULL) {
        printf("Fehler beim Öffnen der Datei %s\n", filename);
        return;
    }

    for(int i = 0; i < size; i++) {
        fprintf(file, "%f ", array[i]);
        if((i + 1) % 3 == 0) {
            fprintf(file, "\n");
        }
    }

    fclose(file);
}

Object* createVAOwithObj(const char* objFilePath) {
    assert(objFilePath != NULL);

    Object* obj = (Object*)malloc(sizeof(Object));
    initObject(obj);

    //clock_t begin = clock();

    FILE* objFile = fopen(objFilePath, "r");
    if (objFile == NULL) {
        printf("File %s not found!\n", objFilePath);
        exit(1);
    }
    size_t vertMaxSize = 3*10*sizeof(GLfloat);
    size_t textMaxSize = 3*10*sizeof(GLfloat);
    size_t normMaxSize = 3*10*sizeof(GLfloat);
    size_t indMaxSize = 9*10*sizeof(int);

    GLfloat* vertices = (GLfloat*)malloc(vertMaxSize);
    GLfloat* textures = (GLfloat*)malloc(textMaxSize);
    GLfloat* normals = (GLfloat*)malloc(normMaxSize);
    int* indices = (int*)malloc(indMaxSize);

    char line[256];
    size_t vertSize = 0;
    size_t textSize = 0;
    size_t normSize = 0;
    size_t indSize = 0;

    GLfloat left = 0;
    GLfloat right = 0;
    GLfloat top = 0;
    GLfloat bottom = 0;
    GLfloat front = 0;
    GLfloat back = 0;

    while (fgets(line, sizeof(line), objFile) != NULL) {
        if(strncmp(line, "v ", 2) == 0) {
            sscanf(line, "v %f %f %f", vertices+vertSize, vertices+vertSize+1, vertices+vertSize+2);
            if(vertices[vertSize] > right) {
                right = vertices[vertSize];
            } else if (vertices[vertSize] < left) {
                left = vertices[vertSize];
            }
            if(vertices[vertSize+1] > top) {
                top = vertices[vertSize+1];
            } else if (vertices[vertSize+1] < bottom) {
                bottom = vertices[vertSize+1];
            }
            if(vertices[vertSize+2] > front) {
                front = vertices[vertSize+2];
            } else if (vertices[vertSize+2] < back) {
                back = vertices[vertSize+2];
            }
            vertSize += 3;
        } else if (strncmp(line, "vt", 2) == 0) {
            sscanf(line, "vt %f %f", textures+textSize, textures+textSize+1);
            textSize += 2;
        } else if (strncmp(line, "vn", 2) == 0) {
            sscanf(line, "vn %f %f %f", normals+normSize, normals+normSize+1, normals+normSize+2);
            normSize += 3;
        } else if (strncmp(line, "f ", 2) == 0) {
            sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", indices+indSize, indices+indSize+1, indices+indSize+2, indices+indSize+3, indices+indSize+4, indices+indSize+5, indices+indSize+6, indices+indSize+7, indices+indSize+8);
            indSize += 9;
        }

        if (vertMaxSize/sizeof(GLfloat) < vertSize + 3) {
            vertices = (GLfloat*)realloc(vertices, vertMaxSize+=300);
        }
        if (textMaxSize/sizeof(GLfloat) < textSize + 2) {
            textures = (GLfloat*)realloc(textures, textMaxSize+=200);
        }
        if (normMaxSize/sizeof(GLfloat) < normSize + 3) {
            normals = (GLfloat*)realloc(normals, normMaxSize+=300);
        }
        if (indMaxSize/sizeof(int) < indSize + 9) {
            indices = (int*)realloc(indices, indMaxSize+=900);
        }
    }         

    BoundingBox boundingBox = {
        {left,top,front},
        {right,top,front},
        {left,bottom,front},
        {right,bottom,front},
        {left,top,back},
        {right,top,back},
        {left,bottom,back},
        {right,bottom,back},
    };

    GLfloat boundingBoxVerts[36*3] = {
        //front
        boundingBox.topleftfront.x,boundingBox.topleftfront.y,boundingBox.topleftfront.z,
        boundingBox.toprightfront.x,boundingBox.toprightfront.y,boundingBox.toprightfront.z,
        boundingBox.bottomrightfront.x,boundingBox.bottomrightfront.y,boundingBox.bottomrightfront.z,

        boundingBox.bottomrightfront.x,boundingBox.bottomrightfront.y,boundingBox.bottomrightfront.z,
        boundingBox.bottomleftfront.x,boundingBox.bottomleftfront.y,boundingBox.bottomleftfront.z,
        boundingBox.topleftfront.x,boundingBox.topleftfront.y,boundingBox.topleftfront.z,
        
        //left
        boundingBox.topleftfront.x,boundingBox.topleftfront.y,boundingBox.topleftfront.z,
        boundingBox.bottomleftfront.x,boundingBox.bottomleftfront.y,boundingBox.bottomleftfront.z,
        boundingBox.topleftback.x,boundingBox.topleftback.y,boundingBox.topleftback.z,

        boundingBox.topleftback.x,boundingBox.topleftback.y,boundingBox.topleftback.z,
        boundingBox.bottomleftback.x,boundingBox.bottomleftback.y,boundingBox.bottomleftback.z,
        boundingBox.bottomleftfront.x,boundingBox.bottomleftfront.y,boundingBox.bottomleftfront.z,

        //back
        boundingBox.topleftback.x,boundingBox.topleftback.y,boundingBox.topleftback.z,
        boundingBox.toprightback.x,boundingBox.toprightback.y,boundingBox.toprightback.z,
        boundingBox.bottomrightback.x,boundingBox.bottomrightback.y,boundingBox.bottomrightback.z,

        boundingBox.bottomrightback.x,boundingBox.bottomrightback.y,boundingBox.bottomrightback.z,
        boundingBox.bottomleftback.x,boundingBox.bottomleftback.y,boundingBox.bottomleftback.z,
        boundingBox.topleftback.x,boundingBox.topleftback.y,boundingBox.topleftback.z,

        //right
        boundingBox.toprightfront.x,boundingBox.toprightfront.y,boundingBox.toprightfront.z,
        boundingBox.bottomrightfront.x,boundingBox.bottomrightfront.y,boundingBox.bottomrightfront.z,
        boundingBox.toprightback.x,boundingBox.toprightback.y,boundingBox.toprightback.z,

        boundingBox.toprightback.x,boundingBox.toprightback.y,boundingBox.toprightback.z,
        boundingBox.bottomrightback.x,boundingBox.bottomrightback.y,boundingBox.bottomrightback.z,
        boundingBox.bottomrightfront.x,boundingBox.bottomrightfront.y,boundingBox.bottomrightfront.z,

        //top
        boundingBox.topleftfront.x, boundingBox.topleftfront.y, boundingBox.topleftfront.z,
        boundingBox.topleftback.x, boundingBox.topleftback.y, boundingBox.topleftback.z,
        boundingBox.toprightback.x, boundingBox.toprightback.y, boundingBox.toprightback.z,

        boundingBox.toprightback.x, boundingBox.toprightback.y, boundingBox.toprightback.z,
        boundingBox.toprightfront.x, boundingBox.toprightfront.y, boundingBox.toprightfront.z,
        boundingBox.topleftfront.x, boundingBox.topleftfront.y, boundingBox.topleftfront.z,

        //bottom
        boundingBox.bottomleftfront.x, boundingBox.bottomleftfront.y, boundingBox.bottomleftfront.z,
        boundingBox.bottomleftback.x, boundingBox.bottomleftback.y, boundingBox.bottomleftback.z,
        boundingBox.bottomrightback.x, boundingBox.bottomrightback.y, boundingBox.bottomrightback.z,

        boundingBox.bottomrightback.x, boundingBox.bottomrightback.y, boundingBox.bottomrightback.z,
        boundingBox.bottomrightfront.x, boundingBox.bottomrightfront.y, boundingBox.bottomrightfront.z,
        boundingBox.bottomleftfront.x, boundingBox.bottomleftfront.y, boundingBox.bottomleftfront.z,


    };

    vertices = (GLfloat*)realloc(vertices, vertSize*sizeof(GLfloat));
    textures = (GLfloat*)realloc(textures, textSize*sizeof(GLfloat));
    normals = (GLfloat*)realloc(normals, normSize*sizeof(GLfloat));
    indices = (int*)realloc(indices, indSize*sizeof(int));

    GLfloat* vertexArray = (GLfloat*)malloc(indSize*sizeof(GLfloat));
    for (size_t i = 0; i < indSize; i+=3) {
        vertexArray[i] = vertices[(indices[i]-1)*3];
        vertexArray[i+1] = vertices[(indices[i]-1)*3+1];
        vertexArray[i+2] = vertices[(indices[i]-1)*3+2];
    }

    int texArrSize = indSize/9*6*sizeof(GLfloat);
    GLfloat* textureArray = (GLfloat*)malloc(texArrSize);
    for (size_t i = 1, j = 0; i < indSize; i+=3, j+=2) {
        textureArray[j] = textures[(indices[i]-1)*2];
        textureArray[j+1] = textures[(indices[i]-1)*2+1];
    }

    GLfloat* normalArray = (GLfloat*)malloc(indSize*sizeof(GLfloat));
    for (size_t i = 2; i < indSize; i+=3) {
        normalArray[i-2] = normals[(indices[i]-1)*3];
        normalArray[i+1-2] = normals[(indices[i]-1)*3+1];
        normalArray[i+2-2] = normals[(indices[i]-1)*3+2];
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);

    GLuint vboV, vboT, vboN;
    glBindVertexArray(vao);

    glGenBuffers(1, &vboV);
    glBindBuffer(GL_ARRAY_BUFFER, vboV);
    glBufferData(GL_ARRAY_BUFFER, indSize*sizeof(GLfloat), vertexArray, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vboT);
    glBindBuffer(GL_ARRAY_BUFFER, vboT);
    glBufferData(GL_ARRAY_BUFFER, texArrSize, textureArray, GL_DYNAMIC_DRAW);
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &vboN);
    glBindBuffer(GL_ARRAY_BUFFER, vboN);
    glBufferData(GL_ARRAY_BUFFER, indSize*sizeof(GLfloat), normalArray, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint boundingBoxVAO;
    GLuint boundingBoxVBO;
    glGenVertexArrays(1, &boundingBoxVAO);
    glBindVertexArray(boundingBoxVAO);
    glGenBuffers(1, &boundingBoxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, boundingBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, 108*sizeof(GLfloat), boundingBoxVerts, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    free(vertices);
    free(textures);
    free(normals);
    free(indices);
    free(vertexArray);
    free(textureArray);
    free(normalArray);

    obj->vao = vao;
    obj->vertCount = indSize/3;
    obj->boundingBox = boundingBox;
    obj->boundingBoxVao = boundingBoxVAO;

    fclose(objFile);
    //clock_t end = clock();
    //double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    //printf("Object %s took %lf\n", objFilePath, time_spent);
    return obj;
}

#endif