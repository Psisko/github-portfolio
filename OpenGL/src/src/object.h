/**
 * \file    object.h : This header file contains informations about objects in the scene and also functions to draw the object and to calculate modelMatrix
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once
using namespace std;
#include "pgr.h"
#include "shader.h"

class ObjectInstance;
struct ObjectGeometry;

using AObjectGeometry = std::unique_ptr<ObjectGeometry>;
using AObjectInstance = std::shared_ptr<ObjectInstance>;

using ObjectList = std::vector<AObjectInstance>;

struct Material {
	glm::vec3	diffuse;
	glm::vec3	ambient;
	glm::vec3	specular;
	float		shininess;

	GLuint		texture;
};


struct ObjectGeometry
{
	GLuint        vertexBufferObject;   ///< identifier for the vertex buffer object
	GLuint        elementBufferObject;  ///< identifier for the element buffer object
	GLuint        vertexArrayObject;    ///< identifier for the vertex array object
	unsigned int  numTriangles;         ///< number of triangles in the mesh
	Material material;					///< material and lightning
	// ...
};

class ObjectInstance {

public:

	AObjectGeometry geometry;

	string name;

	glm::mat4 modelMatrix;
	glm::vec3 position;
	float size;

	float rotationAngle;
	glm::vec3 rotationDirection;

	float rotationAngle2;
	glm::vec3 rotationDirection2;
	
	// water atributes
	bool isWater;
	float multiplier;
	unsigned int stencilID;

	AShader shader;

	ObjectList children;

	/**
	 * \brief ObjectInstance constructor. Takes a pointer to the shader and must create object resources (VBO and VAO)
	 * \param shdrPrg pointer to the shader program for rendering objects data
	 */
	ObjectInstance(AShader _shader = nullptr)
		: shader(_shader), modelMatrix(glm::mat4(1.0f)),
		position(glm::vec3(0.0f, 0.0f, 0.0f)), size(1.0f),
		rotationAngle(0.0f), rotationDirection(glm::vec3(0.0f, 1.0f, 0.0f)), isWater(false), multiplier(1.0f), stencilID(0)
	{
		updateModelMatrix();
	}
	// objekt s potøebou 1 otoèení
	ObjectInstance(AShader _shader, const string & n, const glm::vec3& pos, const float& s, const float & a, const glm::vec3 & rot)
		: shader(_shader), name(n), modelMatrix(glm::mat4(1.0f)), position(pos), size(s), rotationAngle(a), rotationDirection(rot), isWater(false), multiplier(1.0f), stencilID(0)
	{
		updateModelMatrix();
	}
	// objekt s potøebou 2 otoèení
	ObjectInstance(AShader _shader, const string& n, const glm::vec3& pos, const float& s, const float& a1, const glm::vec3& rot1, const float& a2, const glm::vec3& rot2)
		: shader(_shader), name(n), modelMatrix(glm::mat4(1.0f)), position(pos), size(s), rotationAngle(a1), rotationDirection(rot1),
		rotationAngle2(a2), rotationDirection2(rot2), isWater(false), multiplier(1.0f), stencilID(0)
	{
		updateModelMatrix2();
	}
	~ObjectInstance() {}

	/**
	* \brief Recalculates the global matrix and updates all children.
	*   Derived classes should also call this method (using ObjectInstance::update()).
	* \param elapsedTime time value in seconds, such as 0.001*glutGet(GLUT_ELAPSED_TIME) (conversion milliseconds => seconds)
	* \param parentModelMatrix parent transformation in the scene-graph subtree
	*/
	virtual void update(const float elapsedTime) {
		// update all children
		for (AObjectInstance child : children) {
			if (child != nullptr)
				child->update(elapsedTime);
		}
	}

	/**
	 * \brief Draw instance geometry and calls the draw() on child nodes.
	 */
	virtual void draw(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix) {
		// process all children
		for (AObjectInstance child : children) {   //for (auto child : children) {
			if (child != nullptr)
				child->draw(viewMatrix, projectionMatrix);
		}
	}

	virtual void updateModelMatrix() {

		auto translateMatrix = glm::translate(glm::mat4(1.0f), position);
		auto rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationDirection);
		auto scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(size));

		modelMatrix = translateMatrix * rotateMatrix * scaleMatrix;
	}

	virtual void updateModelMatrix2() {

		auto translateMatrix = glm::translate(glm::mat4(1.0f), position);
		auto rotateMatrix1 = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), rotationDirection);
		auto rotateMatrix2 = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle2), rotationDirection2);
		auto scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(size));

		modelMatrix = translateMatrix * rotateMatrix1 * rotateMatrix2 * scaleMatrix;
	}

};
