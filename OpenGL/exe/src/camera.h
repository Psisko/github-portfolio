/**
 * \file    camera.h : This header file contains the camera functions for moving camera and updating viewMatrix
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once
#include "object.h"

class Camera;

using ACamera = shared_ptr<Camera>;

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 view, bool m);
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 upvector;
	bool moveable;

	void updateviewMatrix();

	void moveForward();
	void moveBackward();
	void moveRight();
	void moveLeft();
	void rotateLeftRight(float degrees);
	void rotateUpDown(float degrees);

	glm::mat4 viewMatrix;

	float cameraSpeed = 0.0f;
};

