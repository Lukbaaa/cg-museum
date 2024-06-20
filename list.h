#ifndef LIST_H
#define LIST_H

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "object.h"


typedef struct ObjectList {
    Object** objects;
    int length;
} ObjectList;

void initObjectList(ObjectList* list) {
    list->objects = NULL;
    list->length=0;
}

void olAdd(ObjectList* list, Object* object) {
    list->objects = (Object**)realloc(list->objects, sizeof(Object*)*((list->length)+1));
    (list->objects)[list->length] = object;
    (list->length)++;
}

Object* olGet(ObjectList* list, int i) {
    assert(i<list->length);
    return list->objects[i];
}

void drawObjectsFromList(ObjectList* list) {
    for(int i = 0; i < list->length; i++) {
        olGet(list, i)->draw(olGet(list, i));
    }
}

#endif