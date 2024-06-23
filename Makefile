GLFW_LIBS=$(shell pkgconf glfw3 --libs)
GLEW_LIBS=$(shell pkgconf glew --libs)

museum: museum.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h scene_graph.h light.h list.h framebuffer.h
	gcc -std=c11 -Wall -pedantic-errors -o museum museum.c -lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lm
