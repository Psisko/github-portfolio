/**
 * \file    skybox.h : This header file contains functions to correctly load skybox into the scene and draw it
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once
#include "pgr.h"
#include "shader.h"
#include "object.h"

using namespace std;

class Skybox;

using ASkybox = shared_ptr<Skybox>;

class Skybox : public ObjectInstance
{
public:

	Skybox(AShader shader);

	void loadTexture();

	void draw(const glm::mat4& PV);

	GLuint textureID;
};