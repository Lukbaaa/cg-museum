GLFW_LIBS=$(shell pkgconf glfw3 --libs)
GLEW_LIBS=$(shell pkgconf glew --libs)

museum: museum.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h scene_graph.h light.h
	gcc -std=c11 -Wall  -Wno-deprecated -o museum museum.c $(GLFW_LIBS) $(GLEW_LIBS) -framework OpenGL && ./museum