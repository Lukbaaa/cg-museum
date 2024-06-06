#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>
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
void sortObjectsByDist(Object** objs, int count) {
    int swapped;
    for (int i = 0; i < count - 1; i++) {
        swapped = 0;
        for (int j = 0; j < count - i - 1; j++) {
            if (distToCamera(objs[j]->globalPosition, objs[j]->camera->camPos) < distToCamera(objs[j+1]->globalPosition, objs[j+1]->camera->camPos)) {
                swap(objs[j], objs[j + 1]);
                swapped = 1;
            }
        }
        if (swapped == 0) break;
    }
}

void drawTransparentObjects(Object** objs, int count) {
    sortObjectsByDist(objs, count);
    for(int i = 0; i < count; i++) {
        objs[i]->draw(objs[i]);
    }
}

void traverseDraw(Object* root, GLfloat modelStack[16], Object*** transparentObjects, int* toCount) {
    assert(root != NULL);

    GLfloat temp[16];
    copyMat(temp, modelStack, 16);
    createModelFromTransform(root->model, root->transform);
    mat4Multiplication(modelStack, modelStack, root->model);
    copyMat(root->model, modelStack, 16);
    mat4VectorMultiplication(root->globalPosition, modelStack, root->transform.position);
    if(root->animate != NULL) {
        root->animate(root);
    } 

    if(root->shouldRender) {

        if(!root->isTransparent) {
            root->draw(root);
        } else {
            *transparentObjects = (Object**)realloc(*transparentObjects, sizeof(Object*)*((*toCount)+1));
            (*transparentObjects)[*toCount] = root;
            (*toCount)++;
        }
    }

    if(root->children == NULL) {
        return;
    }

    for(int i = 0; i < root->childrenCount; i++) {
        traverseDraw(root->children[i], modelStack, transparentObjects, toCount);
        copyMat(modelStack, temp, 16);
    }
}

#endif