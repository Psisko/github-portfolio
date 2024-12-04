/**
 * \file    shader.cpp : This file contains functions to correctly load the shader and to correctly set up uniforms inside the shaders
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once

#include "shader.h"

Shader::Shader() : program(0), initialized(false) {}

Shader::Shader(const string& VertexShaderPath, const string& FragmentShaderPath) : program(0), initialized(false)
{
	load_Shader(VertexShaderPath, FragmentShaderPath);
}

Shader::~Shader()
{
	if (initialized)
		pgr::deleteProgramAndShaders(program);
}

void Shader::load_Shader(const string& VertexShaderPath, const string& FragmentShaderPath)
{
	std::vector<GLuint> shaderlist;

	// push vertex shader and fragment shader
	shaderlist.push_back(pgr::createShaderFromFile(GL_VERTEX_SHADER, VertexShaderPath));
	shaderlist.push_back(pgr::createShaderFromFile(GL_FRAGMENT_SHADER, FragmentShaderPath));

	program = pgr::createProgram(shaderlist);
	if (program == 0)
		cout << "loadShaderPrograms: commonShader loading failed! " << endl;
}

void Shader::setupLightUniforms(const string& uniformClassName)
{
	string name = uniformClassName + "ight.ambient";
	this->locations[uniformClassName + "ambient"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.diffuse";
	this->locations[uniformClassName + "diffuse"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.specular";
	this->locations[uniformClassName + "specular"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.position";
	this->locations[uniformClassName + "position"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.spotDirection";
	this->locations[uniformClassName + "spotDirection"] = glGetUniformLocation(this->program, name.c_str());

}

void Shader::setupSpotLightUniforms(const string& uniformClassName)
{
	string name = uniformClassName + "ight.ambient";
	this->locations[uniformClassName + "ambient"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.diffuse";
	this->locations[uniformClassName + "diffuse"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.specular";
	this->locations[uniformClassName + "specular"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.position";
	this->locations[uniformClassName + "position"] = glGetUniformLocation(this->program, name.c_str());
	
	name = uniformClassName + "ight.spotDirection";
	this->locations[uniformClassName + "spotDirection"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.spotCosCutOff";
	this->locations[uniformClassName + "spotCosCutOff"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.spotExponent";
	this->locations[uniformClassName + "spotExponent"] = glGetUniformLocation(this->program, name.c_str());

	name = uniformClassName + "ight.spotAttenuation";
	this->locations[uniformClassName + "spotAttenuation"] = glGetUniformLocation(this->program, name.c_str());



}

void Shader::setupLightUniformsAsserts(const string& uniformClassName)
{
	assert(this->locations[uniformClassName + "ambient"] != -1);
	assert(this->locations[uniformClassName + "diffuse"] != -1);
	assert(this->locations[uniformClassName + "specular"] != -1);
	assert(this->locations[uniformClassName + "position"] != -1);
}
void Shader::setupSpotLightUniformsAsserts(const string& uniformClassName)
{
	assert(this->locations[uniformClassName + "ambient"] != -1);
	assert(this->locations[uniformClassName + "diffuse"] != -1);
	assert(this->locations[uniformClassName + "specular"] != -1);
	assert(this->locations[uniformClassName + "position"] != -1);
	assert(this->locations[uniformClassName + "spotDirection"] != -1);
	assert(this->locations[uniformClassName + "spotCosCutOff"] != -1);
	assert(this->locations[uniformClassName + "spotExponent"] != -1);
	assert(this->locations[uniformClassName + "spotAttenuation"] != -1);
}

void Shader::loadLightUniforms(const string& uniformClassName, ALight light)
{
	glUniform3fv(this->locations[uniformClassName + "ambient"], 1, glm::value_ptr(light->ambient));
	glUniform3fv(this->locations[uniformClassName + "diffuse"], 1, glm::value_ptr(light->diffuse));
	glUniform3fv(this->locations[uniformClassName + "specular"], 1, glm::value_ptr(light->specular));
	glUniform3fv(this->locations[uniformClassName + "position"], 1, glm::value_ptr(light->position));

	if (!light->isSpotlight)
		return;

	glUniform3fv(this->locations[uniformClassName + "spotDirection"], 1, glm::value_ptr(light->spotDirection));
	glUniform1f(this->locations[uniformClassName + "spotCosCutOff"], light->spotCosCutOff);
	glUniform1f(this->locations[uniformClassName + "spotExponent"], light->spotExponent);
	glUniform1f(this->locations[uniformClassName + "spotAttenuation"], light->spotAttenuation);
}
