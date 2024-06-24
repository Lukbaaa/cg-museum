GLFW_LIBS=$(shell pkgconf glfw3 --libs)
GLEW_LIBS=$(shell pkgconf glew --libs)

apple: museum.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h scene_graph.h light.h list.h framebuffer.h
	gcc -std=c11 -Wall  -Wno-deprecated -o museum museum.c $(GLFW_LIBS) $(GLEW_LIBS) -framework OpenGL && ./museum

linux: museum.c camera.h materials.h object.h objectLoader.h shader.h texture.h matrix_functions.h scene_graph.h light.h list.h framebuffer.h
	gcc -std=c11 -Wall  -Wno-deprecated -o museum museum.c -lGLEW -lEGL -lGL -lGLU -lOpenGL -lglfw -lm