/**
 * \file    camera.cpp : This file contains the camera functions for moving camera and updating viewMatrix
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#include "camera.h"
#include <glm/gtx/rotate_vector.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 dir, bool m)
{
	position = pos;
	direction = glm::normalize(dir);
	upvector = glm::vec3(0.0f, 1.0f, 0.0f);
	moveable = m;
	updateviewMatrix();
}

void Camera::updateviewMatrix()
{
	viewMatrix = glm::lookAt(position, position + glm::normalize(direction), upvector);
}

void Camera::moveForward()
{
	position = position + (cameraSpeed * glm::normalize(direction));
	updateviewMatrix();
}

void Camera::moveBackward()
{
	position = position - (cameraSpeed * glm::normalize(direction));
	updateviewMatrix();
}

void Camera::moveRight()
{
	position = position + glm::normalize(glm::cross(direction, upvector)) * cameraSpeed;
	updateviewMatrix();
}

void Camera::moveLeft()
{
	position = position - glm::normalize(glm::cross(direction, upvector)) * cameraSpeed;
	updateviewMatrix();
}

void Camera::rotateLeftRight(float degrees)
{

	direction = glm::rotate(direction, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
	upvector = glm::rotate(upvector, glm::radians(degrees), glm::vec3(0.0f, 1.0f, 0.0f));
	updateviewMatrix();
}

void Camera::rotateUpDown(float degrees)
{
	direction = glm::rotate(direction, glm::radians(degrees), glm::cross(direction, upvector));
	upvector = glm::rotate(upvector, glm::radians(degrees), glm::cross(direction, upvector));
	updateviewMatrix();
}

