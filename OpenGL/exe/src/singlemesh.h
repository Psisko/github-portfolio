/**
 * \file    singlemesh.h : This header file contains functions to load the model into the GPU with texture and material with different styles
 *								and function to setup few uniforms as well as the draw the the loaded object
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once

#include "object.h"
#include "shader.h"
#include "chair.h"

using namespace std;


class SingleMesh : public ObjectInstance
{
public:
	// pro nahrane soubory bez potreby transformace
	SingleMesh(const string & modelFilePath, const string & modelTexturePath , AShader shdrPrg);
	// pro nahrane soubory s potrebou transformovat
	SingleMesh(const string& modelFilePath, const string& modelTexturePath, AShader shdrPrg, const string & name, const glm::vec3& position, const float& size,
		const float& angle, const glm::vec3& rotationDirection);
	// pro hardcoded soubor
	SingleMesh(const string& modelTexturePath, AShader shdrPrg, const string& name, const glm::vec3& position, const float& size,
		const float & angle, const glm::vec3 & rotationDirection, const float& angle2, const glm::vec3& rotationDirection2);
	
	~SingleMesh();

	void update(float elapsedTime) override;
	void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) override;

private:

	bool loadSingleMesh(const string& modelFilePath, const string& modelTexturePath);

	bool initialized;  ///< object has the shader with defined locations

	
};

