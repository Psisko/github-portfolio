/**
 * \file    singlemesh.cpp : This file contains functions to load the model into the GPU with texture and material with different styles
 *								and function to setup few uniforms as well as the draw the the loaded object
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#include <iostream>
#include "singlemesh.h"


void SingleMesh::update(float elapsedTime)
{
	ObjectInstance::update(elapsedTime);
}

void SingleMesh::draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	if (initialized && (shader != nullptr)) {

		glUseProgram(shader->program);

		//active texture animation for water
		if (isWater)
		{	// enable stencil test
			glEnable(GL_STENCIL_TEST);
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, stencilID, 0xff);

			glUniform1i(shader->locations["water"], 1);
		}
		else
		{
			glUniform1i(shader->locations["water"], 0);
		}

		glUniformMatrix4fv(shader->locations["PVM"], 1, GL_FALSE, glm::value_ptr(projectionMatrix * viewMatrix * modelMatrix));
		glUniformMatrix4fv(shader->locations["modelMatrix"], 1, GL_FALSE, glm::value_ptr(modelMatrix));
		CHECK_GL_ERROR();

		// passes material uniforms
		glUniform3fv(shader->locations["Mdiffuse"], 1, glm::value_ptr(geometry->material.diffuse));
		glUniform3fv(shader->locations["Mambient"], 1, glm::value_ptr(geometry->material.ambient));
		glUniform3fv(shader->locations["Mspecular"], 1, glm::value_ptr(geometry->material.specular));
		glUniform1f(shader->locations["Mshininess"], geometry->material.shininess);
		CHECK_GL_ERROR();
		
		// sample texture
		if (geometry->material.texture != 0) {

			glUniform1i(shader->locations["MuseTexture"], 1);
			glBindTexture(GL_TEXTURE_2D, geometry->material.texture);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		}
		// don't sample the texture
		else {
			glUniform1i(shader->locations["MuseTexture"], 0);
		}

		CHECK_GL_ERROR();
		glBindVertexArray(geometry->vertexArrayObject);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, geometry->numTriangles * 3, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
		glUseProgram(0);
		if (isWater)
			glDisable(GL_STENCIL_TEST);
		CHECK_GL_ERROR();
	}
	else {
		std::cerr << "SingleMesh::draw(): Can't draw, mesh not initialized properly!" << std::endl;
	}
}

/** Load one mesh using assimp library (vertices only, for more attributes see method extended version in Asteroids)
 * \param fileName [in] file to open/load
 * \param shader [in] vao will connect loaded data to shader
 * \param geometry
 */
bool SingleMesh::loadSingleMesh(const string& modelFilePath, const string& modelTexturePath) {
	Assimp::Importer importer;

	// unitize object in size (scale the model to fit into (-1..1)^3)
	importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

	// load asset from the file - you can play with various processing steps
	const aiScene* scn = importer.ReadFile(modelFilePath.c_str(), 0
		| aiProcess_Triangulate             // triangulate polygons (if any)
		| aiProcess_PreTransformVertices    // transforms scene hierarchy into one root with geometry-leafs only, for more see Doc
		| aiProcess_GenSmoothNormals        // calculate normals per vertex
		| aiProcess_JoinIdenticalVertices);

	// abort if the loader fails
	if (scn == NULL) {
		std::cerr << "SingleMesh::loadSingleMesh(): assimp error - " << importer.GetErrorString() << std::endl;
		return false;
	}
	cout << "Number of meshes: " << scn->mNumMeshes << endl;
	auto meshes = scn->mMeshes[0];
	// some formats store whole scene (multiple meshes and materials, lights, cameras, ...) in one file, we cannot handle that in our simplified example
	if (scn->mNumMeshes != 1)
	{
		meshes = scn->mMeshes[1];
	}

	// in this phase we know we have one mesh in our loaded scene, we can directly copy its data to OpenGL ...
	const aiMesh* mesh = meshes;

	geometry = make_unique<ObjectGeometry>();

	// create VBO
	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);
	// alocate vertices
	//glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW);
	// allocate memory for vertices, normals and texCoords
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh->mNumVertices, 0, GL_STATIC_DRAW);


	// store all vertices
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * sizeof(float) * mesh->mNumVertices, mesh->mVertices);

	// store all normals
	if (!mesh->mNormals)
		throw std::exception("No normals found");
	glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float) * mesh->mNumVertices, 3 * sizeof(float) * mesh->mNumVertices, mesh->mNormals);


	// create texCoords array
	auto textureCoords = std::make_unique<float[]>(2 * mesh->mNumVertices);  // 2 floats per vertex
	float* tmpTextureCoords = textureCoords.get();
	aiVector3D tmpVec;

	// copy texture coordinates
	if (mesh->HasTextureCoords(0)) {
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			tmpVec = (mesh->mTextureCoords[0])[i];
			*tmpTextureCoords++ = tmpVec.x;
			*tmpTextureCoords++ = tmpVec.y;
		}
	}

	// store all texCoords
	glBufferSubData(GL_ARRAY_BUFFER, 6 * sizeof(float) * mesh->mNumVertices, 2 * sizeof(float) * mesh->mNumVertices, textureCoords.get());

	// copy all mesh faces into one big array (assimp supports faces with ordinary number of vertices, we use only 3 -> triangles)
	auto indices = std::make_unique<unsigned int[]>(mesh->mNumFaces * 3);

	for (unsigned int f = 0; f < mesh->mNumFaces; ++f) {
		indices[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
		indices[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
		indices[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
	}


	// create EBO
	glGenBuffers(1, &(geometry->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);

	// store indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * mesh->mNumFaces, indices.get(), GL_STATIC_DRAW);

	// copy the material info to MeshGeometry structure
	const aiMaterial* mat = scn->mMaterials[mesh->mMaterialIndex];
	aiColor4D color;
	aiString name;

	// get returns: aiReturn_SUCCESS 0 | aiReturn_FAILURE -1 | aiReturn_OUTOFMEMORY -3
	mat->Get(AI_MATKEY_NAME, name); // may be "" after the input mesh processing, must be aiString type!

	// load diffuse color
	if ((aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color)) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	// load diffuse color
	if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	geometry->material.diffuse = glm::vec3(color.r, color.g, color.b);

	// load ambient color
	if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_AMBIENT, &color) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);

	geometry->material.ambient = glm::vec3(color.r, color.g, color.b);

	// load specular color
	if (aiGetMaterialColor(mat, AI_MATKEY_COLOR_SPECULAR, &color) != AI_SUCCESS)
		color = aiColor4D(0.0f, 0.0f, 0.0f, 0.0f);
	geometry->material.specular = glm::vec3(color.r, color.g, color.b);

	// load shinines
	ai_real shininess, strength;
	unsigned int max;

	if (aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS, &shininess, &max) != AI_SUCCESS)
		shininess = 1.0f;

	if (aiGetMaterialFloatArray(mat, AI_MATKEY_SHININESS_STRENGTH, &strength, &max) != AI_SUCCESS)
		strength = 1.0f;

	geometry->material.shininess = shininess * strength;

	geometry->material.texture = pgr::createTexture(modelTexturePath);

	CHECK_GL_ERROR();


	glGenVertexArrays(1, &(geometry->vertexArrayObject));
	glBindVertexArray(geometry->vertexArrayObject);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject); // bind our element array buffer (indices) to vao
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	bool validInit = true;

	// checking if no error
	if (!shader || !shader->initialized || (shader->locations["position"] == -1) || (shader->locations["normal"] == -1) || (shader->locations["texCoords"] == -1))
	{
		cout << "Singlemesh::SingleMesh(AShader shader) error while loading hardcoded model" << endl;
		validInit = false;
	}

	glEnableVertexAttribArray(shader->locations["position"]);
	glVertexAttribPointer(shader->locations["position"], 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(shader->locations["normal"]);
	glVertexAttribPointer(shader->locations["normal"], 3, GL_FLOAT, GL_FALSE, 0, (void*)(3 * sizeof(float) * mesh->mNumVertices));

	glEnableVertexAttribArray(shader->locations["texCoords"]);
	glVertexAttribPointer(shader->locations["texCoords"], 2, GL_FLOAT, GL_FALSE, 0, (void*)(6 * sizeof(float) * mesh->mNumVertices));

	CHECK_GL_ERROR();

	glBindVertexArray(0);

	geometry->numTriangles = mesh->mNumFaces;

	return validInit;
}

// pro nahrane soubory bez potreby transformace
SingleMesh::SingleMesh(const string& modelFilePath, const string& modelTexturePath, AShader shdrPrg) : ObjectInstance(shdrPrg), initialized(false)
{
	if (!loadSingleMesh(modelFilePath, modelTexturePath))
	{
		if (geometry == nullptr) {
			std::cerr << "SingleMesh::SingleMesh(): geometry not initialized!" << std::endl;
		}
		else {
			std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
		}
	}
	else 
	{
		if ((shader != nullptr) && shader->initialized && (shader->locations["PVMmatrix"] != -1))
		{
			initialized = true;
		}
		else 
		{
			std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
		}
	}
}

// pro nahrane soubory s potrebou transformovat
SingleMesh::SingleMesh(const string& modelFilePath, const string& modelTexturePath, AShader shdrPrg, const string & name, const glm::vec3& position, const float& size,
	const float& angle, const glm::vec3& rotationDirection) : ObjectInstance(shdrPrg, name, position, size, angle, rotationDirection), initialized(false)
{
	if (!loadSingleMesh(modelFilePath, modelTexturePath))
	{
		if (geometry == nullptr) {
			std::cerr << "SingleMesh::SingleMesh(): geometry not initialized!" << std::endl;
		}
		else {
			std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
		}
	}
	else
	{
		if ((shader != nullptr) && shader->initialized && (shader->locations["PVMmatrix"] != -1))
		{
			initialized = true;
		}
		else
		{
			std::cerr << "SingleMesh::SingleMesh(): shaderProgram struct not initialized!" << std::endl;
		}
	}
}

// hardcoded
SingleMesh::SingleMesh(const string& modelTexturePath, AShader shdrPrg, const string & name, const glm::vec3& position, const float& size, const float& angle, const glm::vec3& rotationDirection, const float& angle2, const glm::vec3& rotationDirection2)
	: ObjectInstance(shdrPrg, name, position, size, angle, rotationDirection, angle2, rotationDirection2), initialized(true)
{
	geometry = make_unique<ObjectGeometry>();

	CHECK_GL_ERROR();

	// creating VBO
	glGenBuffers(1, &(geometry->vertexBufferObject));
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	// throwing VERTICES into VBO
	glBufferData(GL_ARRAY_BUFFER, Chair::chair_n_attribs_per_vertex * sizeof(float) * Chair::chair_n_vertices, Chair::chair_vertices , GL_STATIC_DRAW);

	// creating EBO
	glGenBuffers(1, &(geometry->elementBufferObject));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);

	// store indices
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int) * Chair::chair_n_triangles, Chair::chair_triangles, GL_STATIC_DRAW);

	// load texture

	geometry->material.texture = pgr::createTexture(modelTexturePath);

	// set material
	geometry->material.ambient = glm::vec3(0.6f);
	geometry->material.diffuse = glm::vec3(0.6f);
	geometry->material.specular = glm::vec3(0.1f);
	geometry->material.shininess = 1.0f;

	CHECK_GL_ERROR();

	// creating VAO
	glGenVertexArrays(1, &(geometry->vertexArrayObject));
	glBindVertexArray(geometry->vertexArrayObject);

	// binding EBO to VAO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry->elementBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, geometry->vertexBufferObject);

	// checking if no error
	if (!shader || !shader->initialized || (shader->locations["position"] == -1) || (shader->locations["normal"] == -1) || (shader->locations["texCoords"] == -1))
	{
		cout << "Singlemesh::SingleMesh(AShader shader) error while loading hardcoded model" << endl;
		initialized = false;
	}
	
	glEnableVertexAttribArray(shader->locations["position"]);
	glVertexAttribPointer(shader->locations["position"], 3, GL_FLOAT, GL_FALSE, sizeof(float) * Chair::chair_n_attribs_per_vertex, (void*)0);
	
	glEnableVertexAttribArray(shader->locations["normal"]);
	glVertexAttribPointer(shader->locations["normal"], 3, GL_FLOAT, GL_FALSE, sizeof(float) * Chair::chair_n_attribs_per_vertex, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(shader->locations["texCoords"]);
	glVertexAttribPointer(shader->locations["texCoords"], 2, GL_FLOAT, GL_FALSE, sizeof(float) * Chair::chair_n_attribs_per_vertex, (void*)(6 * sizeof(float)));

	CHECK_GL_ERROR();

	glBindVertexArray(0);

	geometry->numTriangles = Chair::chair_n_triangles;
}

SingleMesh::~SingleMesh() {

	if (geometry != nullptr) {
		glDeleteVertexArrays(1, &(geometry->vertexArrayObject));
		glDeleteBuffers(1, &(geometry->elementBufferObject));
		glDeleteBuffers(1, &(geometry->vertexBufferObject));

		geometry = nullptr;
	}

	initialized = false;
}
