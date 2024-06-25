#ifndef SCENE_GRAPH_H
#define SCENE_GRAPH_H

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

#include <stdlib.h>
#include <assert.h>
#include <GL/glew.h>

#include "list.h"
#include "object.h"
#include "boundingbox.h"
#include "matrix_functions.h"


void sgAddChild(Object* parent, Object* child) {
    assert(parent != NULL);
    assert(child != NULL);
    objectListAdd(&child->parents, parent);
    objectListAdd(&parent->children, child);
}

void traverse(Object* root) {
    assert(root != NULL);
    printf("%i\n", root->vao);
    if(root->children.length == 0) {
        printf("rock bottom\n");
        return;
    }
    for(int i = 0; i < root->children.length; i++) {
        traverse(root->children.objects[i]);
    }
}

void swap(Object* elem1, Object* elem2)
{
    Object temp = *elem1;
    *elem1 = *elem2;
    *elem2 = temp;
}

void swap2(Object* arr, int i, int j)
{
    Object temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

// bubble sort
void sortObjectsByDist(ObjectHardList* objects) {
    int swapped;
    for (int i = 0; i < objects->length - 1; i++) {
        swapped = 0;
        for (int j = 0; j < objects->length - i - 1; j++) {
            if (distToCamera(objects->objects[j].globalPosition, camera->position) < distToCamera(objects->objects[j+1].globalPosition, camera->position)) {
                //printf("swap ");
                //printf("%f < %f \n", distToCamera(objects->objects[j].globalPosition, camera->position), distToCamera(objects->objects[j+1].globalPosition, camera->position));
                //swap(&(objects->objects[j]), &(objects->objects[j+1]));
                swap2(objects->objects, j, j+1);
                swapped = 1;
            }
        }
        if (swapped == 0) break;
    }
}

void drawObjectsFromList(ObjectHardList* list) {
    for(int i = 0; i < list->length; i++) {
        list->objects[i].draw(&list->objects[i]);
    }
}

void drawIlluminatedObjects(ObjectHardList* objects) {
    drawObjectsFromList(objects);
}

void drawTransparentObjects(ObjectHardList* objects) {
    sortObjectsByDist(objects);
    drawObjectsFromList(objects);
}

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

void traverseDraw(Object* root, GLfloat modelStack[16], ObjectHardList* transparentObjects, ObjectHardList* illuminatedObjects) {
    assert(root != NULL);
    assert(transparentObjects != NULL);
    assert(illuminatedObjects != NULL);

    if(root->animate != NULL) {
        root->animate(root);
    } 

    GLfloat temp[16];
    createModelFromTransform(root->model, root->transform);
    mat4Multiplication(modelStack, modelStack, root->model);
    copyMat(root->model, modelStack, 16);
    copyMat(temp, root->model, 16);
    root->globalPosition = getGlobalPosition(modelStack, root->transform.position);

    if (root->camera != NULL) {
        //root->camera->position = getGlobalPosition(modelStack, root->camera->position);
    }
    if (root->light != NULL) {
        root->light->position = root->globalPosition;
    }
    if(root->shouldRender) {
        if (drawBoundingBoxes) {
            drawBoundingBox(root);
        }
        if(root->isTransparent) {
            //printf("%d ", root->vao);
            objectHardListAdd(transparentObjects, *root);
        } else if(root->lightsAffectedBy.length > 0) {
            objectHardListAdd(illuminatedObjects, *root);
        } else {
            root->draw(root);
        }
    }

    if(root->children.length == 0) {
        return;
    }

    for(int i = 0; i < root->children.length; i++) {
        traverseDraw(root->children.objects[i], modelStack, transparentObjects, illuminatedObjects);
        copyMat(modelStack, temp, 16);
    }
}

#endif