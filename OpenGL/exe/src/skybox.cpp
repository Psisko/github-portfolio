/**
 * \file    skybox.cpp : This file contains functions to correctly load skybox into the scene and draw it
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#include "skybox.h"

Skybox::Skybox(AShader shader) : ObjectInstance(shader)
{
	geometry = make_unique<ObjectGeometry>();

	geometry->numTriangles = 12;

	int numVertices = 36;

	float skyboxVertices[] = {
		// RIGHT T1
		-500.0f,  500.0f, -500.0f,
		-500.0f, -500.0f, -500.0f,
		 500.0f, -500.0f, -500.0f,
		 // RIGHT T2
		 500.0f, -500.0f, -500.0f,
		 500.0f,  500.0f, -500.0f,
		-500.0f,  500.0f, -500.0f,
		// LEFT T1
		-500.0f, -500.0f,  500.0f,
		-500.0f, -500.0f, -500.0f,
		-500.0f,  500.0f, -500.0f,
		// LEFT T2
		-500.0f,  500.0f, -500.0f,
		-500.0f,  500.0f,  500.0f,
		-500.0f, -500.0f,  500.0f,
		// TOP T1
		 500.0f, -500.0f, -500.0f,
		 500.0f, -500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		 // TOP T2
		 500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f, -500.0f,
		 500.0f, -500.0f, -500.0f,
		 // BOTTOM T1
		-500.0f, -500.0f,  500.0f,
		-500.0f,  500.0f,  500.0f,
		 500.0f,  500.0f,  500.0f,
		 // BOTTOM T2
		 500.0f,  500.0f,  500.0f,
		 500.0f, -500.0f,  500.0f,
		-500.0f, -500.0f,  500.0f,
		// BACK T1
		-500.0f,  500.0f, -500.0f,
		 500.0f,  500.0f, -500.0f,
		 500.0f,  500.0f,  500.0f,
		 // BACK T2
		 500.0f,  500.0f,  500.0f,
		-500.0f,  500.0f,  500.0f,
		-500.0f,  500.0f, -500.0f,
		// FRONT T1
		-500.0f, -500.0f, -500.0f,
		-500.0f, -500.0f,  500.0f,
		 500.0f, -500.0f, -500.0f,
		 // FRONT T2
		 500.0f, -500.0f, -500.0f,
		-500.0f, -500.0f,  500.0f,
		 500.0f, -500.0f,  500.0f
	};

	// create VBO
	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	// allocate memory for vertices
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * numVertices, 0, GL_STATIC_DRAW);

	// store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * numVertices, skyboxVertices);

	CHECK_GL_ERROR();

	loadTexture();

	// create VAO
	glGenVertexArrays(1, &(geometry->vertexArrayObject));
	glBindVertexArray(geometry->vertexArrayObject);

	// binding VBO
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	bool validInit = true;

	// checking if no error
	if (!shader || !shader->initialized || (shader->locations["position"] == -1))
	{
		cout << "Singlemesh::SingleMesh(AShader shader) error while loading hardcoded model" << endl;
		validInit = false;
	}

	glEnableVertexAttribArray(shader->locations["position"]);
	glVertexAttribPointer(shader->locations["position"], 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	CHECK_GL_ERROR();

	glBindVertexArray(0);

}

void Skybox::loadTexture()
{
	// create cube map texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	// load right
	if (!pgr::loadTexImage2D("textures/skybox/skyboxRight.png", GL_TEXTURE_CUBE_MAP_POSITIVE_X))
		throw std::exception("Skybox cube textures failed to load");
	// load left
	if (!pgr::loadTexImage2D("textures/skybox/skyboxLeft.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_X))
		throw std::exception("Skybox cube textures failed to load");
	// load top
	if (!pgr::loadTexImage2D("textures/skybox/skyboxTop.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Y))
		throw std::exception("Skybox cube textures failed to load");
	// load bottom
	if (!pgr::loadTexImage2D("textures/skybox/skyboxBottom.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Y))
		throw std::exception("Skybox cube textures failed to load");
	// load back
	if (!pgr::loadTexImage2D("textures/skybox/skyboxBack.png", GL_TEXTURE_CUBE_MAP_POSITIVE_Z))
		throw std::exception("Skybox cube textures failed to load");
	// load front
	if (!pgr::loadTexImage2D("textures/skybox/skyboxFront.png", GL_TEXTURE_CUBE_MAP_NEGATIVE_Z))
		throw std::exception("Skybox cube textures failed to load");


	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void Skybox::draw(const glm::mat4 & PV)
{
	if (shader->initialized && (shader != nullptr))
	{
		glDepthMask(GL_FALSE);
		glUseProgram(shader->program);

		glUniformMatrix4fv(shader->locations["PV"], 1, GL_FALSE, glm::value_ptr(PV));
		CHECK_GL_ERROR();

		glBindVertexArray(geometry->vertexArrayObject);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

		glDrawArrays(GL_TRIANGLES, 0, geometry->numTriangles * 3);


		glBindVertexArray(0);
		glUseProgram(0);
		glDepthMask(GL_TRUE);
		CHECK_GL_ERROR();
	}
	else 
	{
		cerr << "Skybox::draw(): Can't draw, mesh not initialized properly!" << endl;
	}
}