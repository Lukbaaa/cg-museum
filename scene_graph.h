#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
#include "list.h"
#include "object.h"
#include "matrix_functions.h"


void scAddChild(Object* parent, Object* child) {
    assert(parent != NULL);
    assert(child != NULL);
    child->parents = (Object**)realloc(child->parents, sizeof(Object*)*(parent->parentCount+1));
    child->parents[child->parentCount] = parent;
    child->parentCount++;
    parent->children = (Object**)realloc(parent->children, sizeof(Object*)*(parent->childrenCount+1));
    parent->children[parent->childrenCount] = child;
    parent->childrenCount++;
}

void traverse(Object* root) {
    assert(root != NULL);
    printf("%i\n", root->vao);
    if(root->children == NULL) {
        printf("rock bottom\n");
        return;
    }
    for(int i = 0; i < root->childrenCount; i++) {
        traverse(root->children[i]);
    }
}

void swap(Object* x, Object* y)
{
    Object temp = *x;
    *x = *y;
    *y = temp;
}

// bubble sort
void sortObjectsByDist(ObjectList* objects) {
    int swapped;
    for (int i = 0; i < objects->length - 1; i++) {
        swapped = 0;
        for (int j = 0; j < objects->length - i - 1; j++) {
            if (distToCamera(olGet(objects, j)->globalPosition, camera->position) < distToCamera(olGet(objects, j+1)->globalPosition, camera->position)) {
                swap(olGet(objects, j), olGet(objects, j+1));
                swapped = 1;
            }
        }
        if (swapped == 0) break;
    }
}

void drawIlluminatedObjects(ObjectList* objects) {
    for(int i = 0; i < objects->length; i++) {
        olGet(objects, i)->draw(olGet(objects, i));
    }
}

void drawTransparentObjects(ObjectList* objects) {
    sortObjectsByDist(objects);
    for(int i = 0; i < objects->length; i++) {
        olGet(objects, i)->draw(olGet(objects, i));
    }
}

int drawBoundingBoxes = 1;
GLuint boundingBoxProgram;

void drawBoundingBox(Object* obj) {
  if (drawBoundingBoxes) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glUseProgram(boundingBoxProgram);
    glBindVertexArray(obj->boundingBoxVao);
    glUniformMatrix4fv(glGetUniformLocation(boundingBoxProgram, "model"), 1, GL_FALSE, obj->model);
    glUniformMatrix4fv(glGetUniformLocation(boundingBoxProgram, "view"), 1, GL_FALSE, camera->view);
    glUniformMatrix4fv(glGetUniformLocation(boundingBoxProgram, "projection"), 1, GL_TRUE, camera->projection);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
}

void traverseDraw(Object* root, GLfloat modelStack[16], ObjectList* transparentObjects, ObjectList* illuminatedObjects) {
    assert(root != NULL);
    assert(transparentObjects != NULL);
    assert(illuminatedObjects != NULL);
    
    if(root->animate != NULL) {
        root->animate(root);
    } 

    GLfloat temp[16];
    copyMat(temp, modelStack, 16);
    createModelFromTransform(root->model, root->transform);
    mat4Multiplication(modelStack, modelStack, root->model);
    copyMat(root->model, modelStack, 16);
    root->globalPosition = getGlobalPosition(modelStack, root->transform.position);

    if (root->camera != NULL) {
        //root->camera->camPos = getGlobalPosition(modelStack, root->camera->camPos);
    }
    if (root->light != NULL) {
        root->light->position = root->transform.position;
    }
    if(root->shouldRender) {
        if (drawBoundingBoxes) {
            drawBoundingBox(root);
        }
        if(root->isTransparent) {
            olAdd(transparentObjects, root);
        } else if(root->lightCount > 0) {
            olAdd(illuminatedObjects, root);
        } else {
            root->draw(root);
        }
    }

    if(root->children == NULL) {
        return;
    }

    for(int i = 0; i < root->childrenCount; i++) {
        traverseDraw(root->children[i], modelStack, transparentObjects, illuminatedObjects);
        copyMat(modelStack, temp, 16);
    }
}

#endif