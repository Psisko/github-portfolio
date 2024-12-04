/**
 * \file    fire.cpp : This file contains functions for creating Fire object and draw function with stencil buffer and blending
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#include "fire.h"

Fire::Fire(AShader shdrPrg, const string & name, const glm::vec3& position, const float& size,
	const float& angle, const glm::vec3& rotationDirection, const unsigned int & stenID)
	: ObjectInstance(shdrPrg, name, position, size, angle, rotationDirection), currentTime(0.0f), stencilID(stenID), isVisible(true)
{
	geometry = make_unique<ObjectGeometry>();

	geometry->numTriangles = 4;

	const float fireVertexData[4 * 5] = {

		// x      y     z     u     v
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	};

	// create VBO
	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	// store vertices and texCoords
	glBufferData(GL_ARRAY_BUFFER, sizeof(fireVertexData), fireVertexData, GL_STATIC_DRAW);

	CHECK_GL_ERROR();

	geometry->material.texture = pgr::createTexture("textures/fireSheet5x5.png");

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
	glVertexAttribPointer(shader->locations["position"], 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(shader->locations["texCoords"]);
	glVertexAttribPointer(shader->locations["texCoords"], 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	CHECK_GL_ERROR();

	glBindVertexArray(0);
}

void Fire::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	if (shader->initialized && (shader != nullptr))
	{
		if (!isVisible)
			return;
		// enable stencil test
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, stencilID, 0xff);

		// enable alpha channel blending
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(shader->program);

		glUniformMatrix4fv(shader->locations["PVM"], 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));
		glUniform1f(shader->locations["time"], currentTime);

		glBindTexture(GL_TEXTURE_2D, geometry->material.texture);

		glBindVertexArray(geometry->vertexArrayObject);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, geometry->numTriangles);

		CHECK_GL_ERROR();

		glBindVertexArray(0);
		glUseProgram(0);
		glDepthMask(GL_TRUE);

		glDisable(GL_BLEND);
		glDisable(GL_STENCIL_TEST);
	}
	else
	{
		cerr << "Fire::draw(): Can't draw, mesh not initialized properly!" << endl;
	}
}