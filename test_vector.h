#ifndef TEST_VECTOR_H
#define TEST_VECTOR_H

#include "vector.h"
#include <math.h>

#define EPSILON 0.0000000001

char ok[] = "OK.\n";
char nok[] = "not OK.\n";



void testDotProduct(void) {
    // vec3Testobj = {2, 7, 3}
    char testname[] = "testDotProduct";
    Vec3 vec3Testobj = {2, 7, 3};
    float expectedResult = 62; //2 * 2 + 7 * 7 + 3 * 3;
    if (fabs(dotProduct(vec3Testobj, vec3Testobj) - expectedResult) < EPSILON) {
        printf("%s %s", testname, ok);
    } else {
        printf("%s %s", testname, nok);
    }
}

Vec3 a = {5, 0, 3};
Vec3 b = {7, 1, 9};
    
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





#endif
