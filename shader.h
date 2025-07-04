#ifndef SHADER_H
#define SHADER_H
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

#ifdef __APPLE__
#include <GL/glew.h>
#include <OpenGL/gl.h>
#include <OpenGL/glext.h>
#include <GLFW/glfw3.h>
#endif

typedef struct Object Object;

typedef struct Shader {
    GLuint program;
} Shader;

/**
* Prüft ob ein Shader erfolgreich kompiliert wurde
* @param shader - Shader
* @param type - Typ des Shaders
* @param path - Pfad zur Shader Datei
*/
void checkCompileErrors(unsigned int shader, const char* type, const char* path) {
    assert(shader > 0);
    assert(type != NULL);
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            printf("%s\n", path);
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s \n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            printf("%s\n", path);
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s \n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
}

/**
* Erstellt einen Shader aus den angegebenen Dateien
* @param vertexPath - Pfad zur Vertex Shader Datei
* @param fragmentPath - Pfad zur Fragment Shader Datei
* @return Shader
*/
Shader* createShader(const char* vertexPath, const char* fragmentPath) {
    assert(vertexPath != NULL);
    assert(fragmentPath != NULL);

    // Read the Vertex Shader code from the file
    FILE* vertexShaderFile = fopen(vertexPath, "r");
    if (vertexShaderFile == NULL) {
        printf("Can not open %s\n", vertexPath);
        getchar();
        return NULL;
    }

    // Read the Fragment Shader code from the file
    FILE* fragmentShaderFile = fopen(fragmentPath, "r");
    if (fragmentShaderFile == NULL) {
        printf("Can not open %s\n", fragmentPath);
        getchar();
        return NULL;
    }

    // Create the shaders
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Compile Vertex Shader
    char vertexShaderCode[5096];
    size_t len = fread(vertexShaderCode, sizeof(char), 5096, vertexShaderFile);
    char* pVertexShaderCode = vertexShaderCode;
    pVertexShaderCode[len] = '\0';
    glShaderSource(vertexShaderID, 1, (const GLchar**)&pVertexShaderCode, NULL);
    glCompileShader(vertexShaderID);
    checkCompileErrors(vertexShaderID, "VERTEX", vertexPath);

    // Compile Fragment Shader
    char fragmentShaderCode[5096*5];
    len = fread(fragmentShaderCode, sizeof(char), 5096*5, fragmentShaderFile);
    char* pFragmentShaderCode = fragmentShaderCode;
    pFragmentShaderCode[len] = '\0';
    glShaderSource(fragmentShaderID, 1, (const GLchar**)&pFragmentShaderCode, NULL);
    glCompileShader(fragmentShaderID);
    checkCompileErrors(fragmentShaderID, "FRAGMENT", fragmentPath);

    // Link the program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    checkCompileErrors(programID, "PROGRAM", vertexPath);

    // Clean up
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    fclose(vertexShaderFile);
    fclose(fragmentShaderFile);

    // Create shader
    Shader* shader = malloc(sizeof(Shader));
    shader->program = programID;

    return shader;
}
#endif