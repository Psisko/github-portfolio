/**
 * \file    light.cpp : This file contains class Light used to store information about Lights in the scene
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#include "Light.h"

Light::Light(const bool & sL, const glm::vec3& a, const glm::vec3& d, const glm::vec3& s, const glm::vec3& p,
	const glm::vec3& sD, const float& sC, const float& sE, const float& sA)
 : isSpotlight(sL), ambient(a), diffuse(d), specular(s), position(p), 
	spotDirection(sD), spotCosCutOff(sC), spotExponent(sE), spotAttenuation(sA) {}