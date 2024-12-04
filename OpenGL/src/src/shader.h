/**
 * \file    shader.h : This header file contains functions to correctly load the shader and to correctly set up uniforms inside the shaders
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once

#include <iostream>
#include "pgr.h"
#include "map"
#include "light.h"

using namespace std;

class Shader;

using AShader = std::shared_ptr<Shader>;

class Shader
{
public:
	GLuint program;

	bool initialized;

	// position
	// PVMmatrix
	map<string, GLint> locations;

	Shader();
	
	Shader(const string& VertexShaderPath, const string& FragmentShaderPath);

	~Shader();

	void load_Shader(const string& VertexShaderPath, const string& FragmentShaderPath);

	void setupLightUniforms(const string& uniformClassName);

	void setupSpotLightUniforms(const string& uniformClassName);

	void setupLightUniformsAsserts(const string& uniformClassName);

	void setupSpotLightUniformsAsserts(const string& uniformClassName);

	void loadLightUniforms(const string& uniformClassName, ALight light);
};