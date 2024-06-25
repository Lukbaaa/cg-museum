#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "vector.h"
#include "matrix_functions.h"

typedef struct BoundingBox {
    Vec3 topleftfront;
    Vec3 toprightfront;
    Vec3 bottomleftfront;
    Vec3 bottomrightfront;
    Vec3 topleftback;
    Vec3 toprightback;
    Vec3 bottomleftback;
    Vec3 bottomrightback;
} BoundingBox;

int drawBoundingBoxes = 0;
GLuint boundingBoxProgram;

/**
* Erstellt eine Bounding Box mit den gegebenen Dimensionen
* @param width - Breite der Bounding Box
* @param length - Länge der Bounding Box
* @param depth - Tiefe der Bounding Box
*/
BoundingBox createBoundingBox(float width, float length, float depth) {
    BoundingBox box;

    GLfloat halfWidth = width / 2.0f;
    GLfloat halfLength = length / 2.0f;
    GLfloat halfDepth = depth / 2.0f;

    box.topleftfront = (Vec3){-halfWidth, halfLength, halfDepth};
    box.toprightfront = (Vec3){halfWidth, halfLength, halfDepth};
    box.bottomleftfront = (Vec3){-halfWidth, -halfLength, halfDepth};
    box.bottomrightfront = (Vec3){halfWidth, -halfLength, halfDepth};

    box.topleftback = (Vec3){-halfWidth, halfLength, -halfDepth};
    box.toprightback = (Vec3){halfWidth, halfLength, -halfDepth};
    box.bottomleftback = (Vec3){-halfWidth, -halfLength, -halfDepth};
    box.bottomrightback = (Vec3){halfWidth, -halfLength, -halfDepth};

    return box;
}

/**
* prüft ob ein Punkt innerhalb einer Bounding Box liegt
* @param point - Punkt der geprüft werden soll
* @param box - Bounding Box in der der Punkt geprüft werden soll
*/
int isInside(Vec3 point, BoundingBox box) {
    if (point.x >= box.topleftfront.x && point.x <= box.toprightfront.x &&
        point.y >= box.bottomleftfront.y && point.y <= box.topleftfront.y &&
        point.z >= box.topleftfront.z && point.z <= box.topleftback.z) {
        return 1;
    }
    return 0;
}

/**
* prüft ob zwei Bounding Boxen kollidieren
* @param box1 - erste Bounding Box
* @param box2 - zweite Bounding Box
*/
int isColliding(BoundingBox box1, BoundingBox box2) {
    if (isInside(box1.topleftfront, box2) || isInside(box1.toprightfront, box2) ||
        isInside(box1.bottomleftfront, box2) || isInside(box1.bottomrightfront, box2) ||
        isInside(box1.topleftback, box2) || isInside(box1.toprightback, box2) ||
        isInside(box1.bottomleftback, box2) || isInside(box1.bottomrightback, box2)) {
        return 1;
    }
    return 0;
}
/**
* Erstellt ein Vertex Array für die Bounding Box um sie zu zeichnen
* @param out - Array in das die Vertices geschrieben werden
* @param boundingBox - Bounding Box die gezeichnet werden soll
*/
void createBoundingBoxVertexArray(GLfloat out[32*3], BoundingBox boundingBox) {
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
    copyMat(out, boundingBoxVerts, 36*3);
}


#endif