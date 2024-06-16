GLFW_LIBS=$(shell pkgconf glfw3 --libs)
GLEW_LIBS=$(shell pkgconf glew --libs)


appleMuseum: museum.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h scene_graph.h light.h
	gcc -g -O0 -std=c11 -o appleMuseum museum.c $(GLEW_LIBS) $(GLFW_LIBS) -framework OpenGL

museum: museum.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h scene_graph.h light.h
	gcc -std=c11 -Wall -pedantic-errors -o museum museum.c -lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lm

