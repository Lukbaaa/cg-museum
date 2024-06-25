#ifndef TEST_MATRIX_FUNCTIONS_H
#define TEST_MATRIX_FUNCTIONS_H
//#include "matrix_functions.h"

#define EPSILON 0.0000000001

char mat_ok[] = "OK.\n";
char mat_nok[] = "not OK.\n";

GLfloat in[16] = {5, 0, 3, 1, 
                  2, 6, 8, 8,
                  6, 2, 1, 5,
                  1, 0, 4, 6};
GLfloat v[16] = {7, 1, 9, 5,
                 5, 8, 4, 3,
                 8, 2, 3, 7,
                 0, 6, 8, 9};
GLfloat out[16];

/**
 * testet mat4Multiplication-Funktion von matrix_functions.h
 */
void testMat4Multiplication(void) {
    char testname[] = "testMat4Multiplication";

    GLfloat expectedResult[16] = {96, 24, 58, 90,
                                  68, 56, 95, 107,
                                  69, 18, 71, 81,
                                  69, 52, 92, 142};
    mat4Multiplication(out, in, v);

    for (int i = 0; i < 16; i++) {
        if (fabs(out[i] - expectedResult[i]) >= EPSILON) {
            printf("%s %s", testname, mat_nok);
            return;
        }
    }

    // Sonderfall: zwei Paarameter verweisen auf dasselbe Array
    GLfloat inExc[16] = {5, 0, 3, 1, 
                  2, 6, 8, 8,
                  6, 2, 1, 5, 
                  1, 0, 4, 6};
    mat4Multiplication(inExc, inExc, inExc);
    GLfloat expectedResult2[16] = {  44, 6, 22, 26, 
                                    78, 52, 94, 138,
                                    45, 14, 55, 57, 
                                    35, 8, 31, 57};

    for (int i = 0; i < 16; i++) {
        if (fabs(inExc[i] - expectedResult2[i]) >= EPSILON) {
            printf("%s %s", testname, mat_nok);
            return;
        }
    }

    printf("%s %s", testname, mat_ok);
}

/**
 * testet transpose4-Funktion von matrix_functions.h
 */
void testTranspose4(void) {
    char testname[] = "testTranspose4";
    GLfloat expectedResult[16] = {5, 2, 6, 1,
                                0, 6, 2, 0,
                                3, 8, 1, 4,
                                1, 8, 5, 6};
    transpose4(out, in);

    for (int i = 0; i < 16; i++) {
        if (fabs(out[i] - expectedResult[i]) >= EPSILON) {
            printf("%s %s", testname, mat_nok);
            return;
        } 
    }
    printf("%s %s", testname, mat_ok); 

}

/**
 * testet inverse4-Funktion von matrix_functions.h
 */
void testInverse4(void) {
    char testname[] = "testInverse4";
    GLfloat expectedResult[16] = {29/244.0, -7/244.0, 21/244.0, -13/244.0,
                                  -23/244.0, 35/244.0, 17/244.0, -57/244.0,
                                  89/488.0, 29/488.0, -87/488.0, 19/488.0,
                                  -69/488.0, -17/488.0, 51/488.0, 73/488.0};
    int x = inverse4(out, in);

    for (int i = 0; i < 16; i++) {
        if (fabs(out[i] - expectedResult[i]) >= EPSILON) {
            printf("%s %s", testname, mat_nok);
            return;
        }
    }
    printf("%s %s", testname, mat_ok);
}

/**
 * Funktion fuehrt alle Tests der matrix_functions.h Funktionen aus
 */
void testing_matrix_functions(void) {
    testMat4Multiplication();
    testTranspose4();
    testInverse4();
}

#endif
