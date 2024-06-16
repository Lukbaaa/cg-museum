#ifndef SKYBOX_H
#define SKYBOX_H


#include <GL/glew.h>
#ifndef __APPLE__
#include <OpenGL/gl3.h>
#endif // !__APPLE__



#include <GLFW/glfw3.h>


// declare fucntions


GLuint cubemapTexture;
GLuint skyboxVAO, skyboxVBO, skyboxEBO;

float skyboxVertices[] =
{
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f
};
// skybox.h
GLuint getCubemapTexture() {
    return cubemapTexture;
}
// skybox.h
GLuint getVAO() {
    return skyboxVAO;
}
// skybox.h
GLuint getVBO() {
    return skyboxVBO;
}
// skybox.h
GLuint getEBO() {
    return skyboxEBO;
}



unsigned int skyboxIndices[] =
{
	// Right
	1, 2, 6,
	6, 5, 1,
	// Left
	0, 4, 7,
	7, 3, 0,
	// Top
	4, 5, 6,
	6, 7, 4,
	// Bottom
	0, 3, 2,
	2, 1, 0,
	// Back
	0, 1, 5,
	5, 4, 0,
	// Front
	3, 7, 6,
	6, 2, 3
};
// facesCubemap loction 
const char* facesCubemap[6] = {
    "./skybox/right.jpg",
    "./skybox/left.jpg",
    "./skybox/top.jpg",
    "./skybox/bottom.jpg",
    "./skybox/front.jpg",
    "./skybox/back.jpg"
};

void initializeSkybox() {

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}


void loadCubemapTexture() {
    printf("Loading cubemap texture\n");
    // Creates the cubemap texture object
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// These are very important to prevent seams
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	// This might help with seams on some systems
	// glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    printf("Loading cubemap texture1\n");
    // Cycles through all the textures and attaches them to the cubemap object
	for (unsigned int i = 0; i < 6; i++)
	{
		int width, height, nrChannels;
		unsigned char* data = stbi_load(facesCubemap[i], &width, &height, &nrChannels, 0);
		if (data)
		{
            printf("Loading cubemap texture2\n");
			stbi_set_flip_vertically_on_load(0);
			glTexImage2D
			(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		}
		else
		{
			printf("Failed to load texture: %s", facesCubemap[i]);
			stbi_image_free(data);
		}
	}
}

#endif