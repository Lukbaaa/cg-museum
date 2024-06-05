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

void traverseDraw(Object* root, GLfloat model[16]) {
    assert(root != NULL);

    if(root->animate != NULL) {
        root->animate(root);
    } 

    GLfloat objModel[16];
    identity(objModel);
    createModelFromTransform(objModel, root->transform);
    mat4Multiplication(model, model, objModel);
    if(root->shouldRender) {
        root->draw(root, model);
    }
    if(root->children == NULL) {
        return;
    }
    for(int i = 0; i < root->childrenCount; i++) {
        traverseDraw(root->children[i], model);
    }
}

#endif