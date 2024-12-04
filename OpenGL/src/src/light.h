/**
 * \file    light.h : This header file contains class Light used to store information about Lights in the scene
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once

#include "pgr.h"

using namespace std;

class Light;

using ALight = shared_ptr<Light>;

class Light
{
public:

	Light(const bool& sL, const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& p,
		const glm::vec3& sD = glm::vec3(0.0f), const float& sC = 0.0f, const float& sE = 0.0f, const float& sA = 0.0f);

	// for all light types
	glm::vec3	ambient;
	glm::vec3	diffuse;
	glm::vec3	specular;
	glm::vec3	position;

	bool isSpotlight;

	// for spot light
	glm::vec3 spotDirection;
	float spotCosCutOff;
	float spotExponent;
	float spotAttenuation;
};
