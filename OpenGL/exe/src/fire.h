/**
 * \file    fire.h : This header file contains functions for creating Fire object and draw function with stencil buffer and blending
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once
#include "pgr.h"
#include "object.h"
#include "shader.h"

class Fire;

using AFire = shared_ptr<Fire>;

class Fire : public ObjectInstance
{
public:
	Fire(AShader shdrPrg, const string & name, const glm::vec3& position, const float& size,
		const float& angle, const glm::vec3& rotationDirection, const unsigned int & stenID);

	void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

	float currentTime;

	unsigned int stencilID;

	bool isVisible;

};