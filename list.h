#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

typedef struct Object Object;
typedef struct LightSource LightSource;


typedef struct ObjectList {
    Object** objects;
    int length;
} ObjectList;

typedef struct LightSourceList {
    LightSource** objects;
    int length;
} LightSourceList;

void initObjectList(ObjectList* list) {
    list->objects = NULL;
    list->length=0;
}

void initLightSourceList(LightSourceList* list) {
    list->objects = NULL;
    list->length=0;
}

void objectListAdd(ObjectList* list, Object* object) {
    list->objects = (Object**)realloc(list->objects, sizeof(Object*)*((list->length)+1));
    (list->objects)[list->length] = object;
    (list->length)++;
}

void lightSourceListAdd(LightSourceList* list, LightSource* lightSource) {
    list->objects = (LightSource**)realloc(list->objects, sizeof(LightSource*)*((list->length)+1));
    (list->objects)[list->length] = lightSource;
    (list->length)++;
}

Object* objectListGet(ObjectList* list, int i) {
    assert(i<list->length);
    return list->objects[i];
}

LightSource* lightSourceListGet(LightSourceList* list, int i) {
    assert(i<list->length);
    return list->objects[i];
}

#endif