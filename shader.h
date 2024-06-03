#ifndef SHADER_H
#define SHADER_H
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>

typedef struct Object Object;

typedef struct Shader {
    GLuint program;
    void (*setUniforms)(Object*);
} Shader;

void checkCompileErrors(unsigned int shader, const char* type) {
    assert(shader > 0);
    assert(type != NULL);
    
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s \n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s \n -- --------------------------------------------------- -- \n", type, infoLog);
        }
    }
}

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
    checkCompileErrors(vertexShaderID, "VERTEX");

    // Compile Fragment Shader
    char fragmentShaderCode[5096];
    len = fread(fragmentShaderCode, sizeof(char), 5096, fragmentShaderFile);
    char* pFragmentShaderCode = fragmentShaderCode;
    pFragmentShaderCode[len] = '\0';
    glShaderSource(fragmentShaderID, 1, (const GLchar**)&pFragmentShaderCode, NULL);
    glCompileShader(fragmentShaderID);
    checkCompileErrors(fragmentShaderID, "FRAGMENT");

    // Link the program
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);
    checkCompileErrors(programID, "PROGRAM");

    // Clean up
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    fclose(vertexShaderFile);
    fclose(fragmentShaderFile);

    // Create shader
    Shader* shader = malloc(sizeof(Shader));
    shader->program = programID;
    shader->setUniforms = NULL;

    return shader;
}
#endif