GLFW_LIBS=$(shell pkgconf glfw3 --libs)
GLEW_LIBS=$(shell pkgconf glew --libs)

museum: main.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h
	gcc -std=c11 -Wall -pedantic-errors -o museum main.c -lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lm
