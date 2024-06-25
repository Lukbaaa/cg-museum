#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H

#include "vector.h"
#include <math.h>

#define EPSILON 0.0000000001

char ok[] = "OK.\n";
char nok[] = "not OK.\n";
Vec3 a = {5, 0, 3};
Vec3 b = {7, 1, 9};

/**
 * testet dotProdukt-Funktion von vector.h
 */
void testDotProduct(void) {
    char testname[] = "testDotProduct";
    float expectedResult = 34;
    if (fabs(dotProduct(a, a) - expectedResult) < EPSILON) {
        printf("%s %s", testname, ok);
    } else {
        printf("%s %s", testname, nok);
    }
}
    
/**
 * testet crossProduct-Funktion von vector.h
 */
void testCrossProduct(void) {
    char testname[] = "testCrossProduct";
    Vec3 expectedResult = {-3, -24, 5};
    Vec3 cross = crossProduct(a, b);
    if (fabs(cross.x - expectedResult.x) < EPSILON) {
        if (fabs(cross.y - expectedResult.y) < EPSILON) {
            if (fabs(cross.z - expectedResult.z) < EPSILON) {
                printf("%s %s", testname, ok);
                return;
            }
        }
    }
    printf("%s %s", testname, nok);
}

/**
 * testet length2d-Funktion von vector.h
 */
void testLength2d(void) {
    char testname[] = "testLength2d";
    Vec2 vec = {2, 5};
    float expectedResult = sqrt(29);
    float result = length2d(vec);

    if (fabs(expectedResult - result) < EPSILON) {
        printf("%s %s", testname, ok);
        return;
    }
    printf("%s %s", testname, nok);
}

/**
 * testet length3d-Funktion von vector.h
 */
void testLength3d(void) {
    char testname[] = "testLength3d";
    Vec3 vec = {1, 2, 3};
    float expectedResult = sqrt(14);
    float result = length3d(vec);

    if (fabs(expectedResult - result) < EPSILON) {
        printf("%s %s", testname, ok);
        return;
    }
    printf("%s %s", testname, nok);
}

/**
 * testet length2f-Funktion von vector.h
 */
void testLength2f(void) {
    char testname[] = "testLength2f";
    float a = 4;
    float b = 7;
    float expectedResult = sqrt(65);
    float result = length2f(a, b);

    if (fabs(expectedResult - result) < EPSILON) {
        printf("%s %s", testname, ok);
        return;
    }
    printf("%s %s", testname, nok);
}

/**
 * testet normalize-Funktion von vector.h
 */
void testNormalize(void) {
    char testname[] = "testNormalize";
    Vec3 x = {6, 3, 6};
    Vec3 expectedResult = {2.0/3, 1.0/3, 2.0/3};
    
    normalize(&x);

    if (fabs(x.x - expectedResult.x) < EPSILON) {
        if (fabs(x.y - expectedResult.y) < EPSILON) {
            if (fabs(x.z - expectedResult.z) < EPSILON) {
                printf("%s %s", testname, ok);
                return;
            }
        }
    }
    printf("%s %s", testname, nok);    
}

/**
 * Funktion fuehrt alle Tests der vector.h Funktionen aus
 */
void testing_vector(void) {
    testDotProduct();
    testCrossProduct();
    testLength2d();
    testLength3d();
    testLength2f();
    testNormalize();
}

#endif
