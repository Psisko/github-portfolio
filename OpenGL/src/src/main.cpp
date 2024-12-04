/**
 * \file    main.cpp : This file contains the 'main' function and callbacks.
			Program execution begins and ends there.
 * \author  Patrik Krulík
 * \date    15/05/2024
 */


#include <iostream>
#include "pgr.h"
#include "shader.h"
#include "object.h"
#include "singlemesh.h"
#include "camera.h"
#include "light.h"
#include "singlemesh.h"
#include "skybox.h"
#include "fire.h"
#include "parameters.h"

using namespace std;


bool keyMap[4];
enum { KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_UP_ARROW, KEY_DOWN_ARROW, KEY_SPACE, KEYS_COUNT };

// objects in scene
ObjectList objects;
// cameras
vector<ACamera> cameras;
ACamera mainCamera;
// shaders
AShader commonShader;
AShader skyboxShader;
AShader fireShader;
// lights
map<string, ALight> lights;
// skybox
ASkybox skybox;
// fire
AFire fire1;
AFire fire2;
AFire fire3;
// time
float currentTime;
// bird for ellipse
shared_ptr<SingleMesh> bird;
glm::vec3 ellipseCenter;
// lake
shared_ptr<SingleMesh> lake;
// -----------------------  OpenGL stuff ---------------------------------

/**
 * \brief Load and compile shader programs. Get attribute locations.
 */
void loadShaderPrograms()
{
	// COMMON SHADER
	AShader newShader = make_shared<Shader>("./shaders/commonVertexShader.vert", "./shaders/commonFragmentShader.frag");
	
	commonShader = newShader;

	// vertexes, normals and texCoords
	commonShader->locations["position"] = glGetAttribLocation(commonShader->program, "position");
	commonShader->locations["normal"] = glGetAttribLocation(commonShader->program, "normal");
	commonShader->locations["texCoords"] = glGetAttribLocation(commonShader->program, "texCoords");
	// uniform matrixes
	commonShader->locations["PVM"] = glGetUniformLocation(commonShader->program, "PVM");
	commonShader->locations["modelMatrix"] = glGetUniformLocation(commonShader->program, "modelMatrix");
	// uniform cameraPos
	commonShader->locations["cameraPos"] = glGetUniformLocation(commonShader->program, "cameraPos");
	// uniform material
	commonShader->locations["Mambient"] = glGetUniformLocation(commonShader->program, "material.ambient");
	commonShader->locations["Mdiffuse"] = glGetUniformLocation(commonShader->program, "material.diffuse");
	commonShader->locations["Mspecular"] = glGetUniformLocation(commonShader->program, "material.specular");
	commonShader->locations["Mshininess"] = glGetUniformLocation(commonShader->program, "material.shininess");
	commonShader->locations["MuseTexture"] = glGetUniformLocation(commonShader->program, "material.useTexture");
	// uniform time
	commonShader->locations["time"] = glGetUniformLocation(commonShader->program, "time");
	// uniform water
	commonShader->locations["water"] = glGetUniformLocation(commonShader->program, "water");
	commonShader->locations["multiplier"] = glGetUniformLocation(commonShader->program, "multiplier");

	// uniform directionalLight
	commonShader->setupLightUniforms("dL");
	commonShader->setupLightUniformsAsserts("dL");
	// uniform spotLight
	commonShader->setupSpotLightUniforms("sL");
	commonShader->setupSpotLightUniformsAsserts("sL");
	// uniform pointLight
	commonShader->setupLightUniforms("p1L");
	commonShader->setupLightUniformsAsserts("p1L");
	commonShader->setupLightUniforms("p2L");
	commonShader->setupLightUniformsAsserts("p2L");
	commonShader->setupLightUniforms("p3L");
	commonShader->setupLightUniformsAsserts("p3L");
	// asserts
	assert(commonShader->locations["position"] != -1);
	assert(commonShader->locations["normal"] != -1);
	assert(commonShader->locations["texCoords"] != -1);

	assert(commonShader->locations["PVM"] != -1);
	assert(commonShader->locations["modelMatrix"] != -1);
	
	assert(commonShader->locations["cameraPos"] != -1);

	assert(commonShader->locations["Mambient"] != -1);
	assert(commonShader->locations["Mdiffuse"] != -1);
	assert(commonShader->locations["Mspecular"] != -1);
	assert(commonShader->locations["Mshininess"] != -1);
	assert(commonShader->locations["MuseTexture"] != -1);

	assert(commonShader->locations["time"] != -1);
	assert(commonShader->locations["water"] != -1);
	assert(commonShader->locations["multiplier"] != -1);

	CHECK_GL_ERROR();

	commonShader->initialized = true;
	
	// SKYBOX SHADER

	newShader = make_shared<Shader>("./shaders/skyboxVertexShader.vert", "./shaders/skyboxFragmentShader.frag");

	skyboxShader = newShader;
	// vertexes
	skyboxShader->locations["position"] = glGetAttribLocation(skyboxShader->program, "position");
	// uniform matrixes
	skyboxShader->locations["PV"] = glGetUniformLocation(skyboxShader->program, "PV");
	// uniform cameraPos
	skyboxShader->locations["cameraPos"] = glGetUniformLocation(skyboxShader->program, "cameraPos");

	assert(skyboxShader->locations["position"] != -1);
	assert(skyboxShader->locations["PV"] != -1);
	assert(skyboxShader->locations["cameraPos"] != -1);

	skyboxShader->initialized = true;

	// FIRE SHADER

	newShader = make_shared<Shader>("./shaders/fireVertexShader.vert", "./shaders/fireFragmentShader.frag");

	fireShader = newShader;

	// vertexes and texCoords
	fireShader->locations["position"] = glGetAttribLocation(fireShader->program, "position");
	fireShader->locations["texCoords"] = glGetAttribLocation(fireShader->program, "texCoords");
	// uniform matrixes
	fireShader->locations["PVM"] = glGetUniformLocation(fireShader->program, "PVM");
	// uniform time
	fireShader->locations["time"] = glGetUniformLocation(fireShader->program, "time");

	assert(fireShader->locations["position"] != -1);
	assert(fireShader->locations["texCoords"] != -1);
	assert(fireShader->locations["PVM"] != -1);
	assert(fireShader->locations["time"] != -1);

	fireShader->initialized = true;

}

/**
 * \ Draw all scene objects.
 */
void drawScene(void)
{
	glUseProgram(commonShader->program);

	commonShader->loadLightUniforms("dL", lights["directional"]);

	commonShader->loadLightUniforms("sL", lights["spotlight"]);

	// if fire1 is not visible, turn the ambient, diffuse and specular to zero
	if(fire1->isVisible)
		commonShader->loadLightUniforms("p1L", lights["pointlight1"]);
	else
		commonShader->loadLightUniforms("p1L", make_shared<Light>(false, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.22838, 0.935077, 1.06817)));
	
	if(fire2->isVisible)
		commonShader->loadLightUniforms("p2L", lights["pointlight2"]);
	else
		commonShader->loadLightUniforms("p2L", make_shared<Light>(false, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.22838, 0.935077, -1.07979)));

	commonShader->loadLightUniforms("p3L", lights["pointlight3"]);

	glUniform3fv(commonShader->locations["cameraPos"], 1, glm::value_ptr(mainCamera->position));
	glUniform3fv(skyboxShader->locations["cameraPos"], 1, glm::value_ptr(mainCamera->position));

	glUniform1f(commonShader->locations["time"], currentTime);

	glUniform1f(commonShader->locations["water"], lake->isWater);

	glUniform1f(commonShader->locations["multiplier"], lake->multiplier);

	CHECK_GL_ERROR();

	glUseProgram(0);

	glm::mat4 viewMatrix = mainCamera->viewMatrix;

	glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), (float)Parameters::WINDOW_WIDTH / (float)Parameters::WINDOW_HEIGHT, 0.01f, 1000.0f);

	for (AObjectInstance object : objects) {
		if (object != nullptr)
			object->draw(viewMatrix, projectionMatrix);
	}

	// glm::mat4 odstrani translaci
	skybox->draw(projectionMatrix * glm::mat4(glm::mat3(viewMatrix)));

	// musim mit az po objektech ve scene, aby se dobre vykreslila transparentnost pozadi obrazku
	fire1->draw(viewMatrix, projectionMatrix);
	fire2->draw(viewMatrix, projectionMatrix);
	fire3->draw(viewMatrix, projectionMatrix);
}

// -----------------------  Window callbacks ---------------------------------

/**
 * \brief Draw the window contents.
 */
void displayCb() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// draw the window contents (scene objects)
	drawScene();

	glutSwapBuffers();
}

/**
 * \brief Window was reshaped.
 * \param newWidth New window width
 * \param newHeight New window height
 */
void reshapeCb(int newWidth, int newHeight) {
	// TODO: Take new window size and update the application state,
	// window and projection.

	// glViewport(...);
};

// -----------------------  Keyboard ---------------------------------

/**
 * \brief Handle the key pressed event.
 * Called whenever a key on the keyboard was pressed. The key is given by the "keyPressed"
 * parameter, which is an ASCII character. It's often a good idea to have the escape key (ASCII value 27)
 * to call glutLeaveMainLoop() to exit the program.
 * \param keyPressed ASCII code of the key
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void keyboardCb(unsigned char keyPressed, int mouseX, int mouseY) 
{
	switch (keyPressed)
	{
		case 27:
			glutLeaveMainLoop();
			exit(EXIT_SUCCESS);
			break;
		case 'c':
			mainCamera->moveable = !mainCamera->moveable;
			break;
		case '1':
			mainCamera->position = cameras[Parameters::ST1_CAMERA_INDEX]->position;
			mainCamera->direction = cameras[Parameters::ST1_CAMERA_INDEX]->direction;
			mainCamera->updateviewMatrix();
			break;
		case '2':
			mainCamera->position = cameras[Parameters::ST2_CAMERA_INDEX]->position;
			mainCamera->direction = cameras[Parameters::ST2_CAMERA_INDEX]->direction;
			mainCamera->updateviewMatrix();
			break;
		case '3':
			mainCamera->position = cameras[Parameters::BIRD_CAMERA_INDEX]->position;
			mainCamera->direction = cameras[Parameters::BIRD_CAMERA_INDEX]->direction;
			mainCamera->updateviewMatrix();
			break;
	}
}

// Called whenever a key on the keyboard was released. The key is given by
// the "keyReleased" parameter, which is in ASCII. 
/**
 * \brief Handle the key released event.
 * \param keyReleased ASCII code of the released key
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void keyboardUpCb(unsigned char keyReleased, int mouseX, int mouseY) {
}

//
/**
 * \brief Handle the non-ASCII key pressed event (such as arrows or F1).
 *  The special keyboard callback is triggered when keyboard function (Fx) or directional
 *  keys are pressed.
 * \param specKeyPressed int value of a predefined glut constant such as GLUT_KEY_UP
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void specialKeyboardCb(int specKeyPressed, int mouseX, int mouseY) {

	switch (specKeyPressed) {
	case GLUT_KEY_RIGHT:
		keyMap[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT:
		keyMap[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP:
		keyMap[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN:
		keyMap[KEY_DOWN_ARROW] = true;
		break;
	default:
		; 
	}
}

void specialKeyboardUpCb(int specKeyReleased, int mouseX, int mouseY) {

	switch (specKeyReleased) {
	case GLUT_KEY_RIGHT:
		keyMap[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT:
		keyMap[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP:
		keyMap[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN:
		keyMap[KEY_DOWN_ARROW] = false;
		break;
	default:
		; // printf("Unrecognized special key pressed\n");
	}
} // key released

// -----------------------  Mouse ---------------------------------
// three events - mouse click, mouse drag, and mouse move with no button pressed

// 
/**
 * \brief React to mouse button press and release (mouse click).
 * When the user presses and releases mouse buttons in the window, each press
 * and each release generates a mouse callback (including release after dragging).
 *
 * \param buttonPressed button code (GLUT_LEFT_BUTTON, GLUT_MIDDLE_BUTTON, or GLUT_RIGHT_BUTTON)
 * \param buttonState GLUT_DOWN when pressed, GLUT_UP when released
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void mouseCb(int buttonPressed, int buttonState, int mouseX, int mouseY) {
	
	switch (buttonPressed)
	{
		case GLUT_LEFT_BUTTON:
			//cout << mainCamera->position.x << "f," << mainCamera->position.y << "f," << mainCamera->position.z << "f" << endl;
			break;
		case GLUT_RIGHT_BUTTON:
			if (buttonState == GLUT_UP)
				break;
			unsigned char clickedID;
			glReadPixels(mouseX,Parameters::WINDOW_HEIGHT - mouseY - 1, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &clickedID);

			if(clickedID == 0)
				cout << "background was hit" << endl;

			else if (clickedID == fire1->stencilID)
			{
				cout << "fire1 was hit" << endl;
				fire1->isVisible = false;
			}

			else if (clickedID == fire2->stencilID)
			{
				cout << "fire2 was hit" << endl;
				fire2->isVisible = false;
			}
			else if (clickedID == fire3->stencilID)
			{
				cout << "fire3 was hit" << endl;
				fire1->isVisible = true;
				fire2->isVisible = true;
			}
			else if (clickedID == lake->stencilID)
			{
				cout << "lake was hit" << endl;
				lake->multiplier++;
			}
			break;
	}
	

}

/**
 * \brief Handle mouse dragging (mouse move with any button pressed).
 *        This event follows the glutMouseFunc(mouseCb) event.
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void mouseMotionCb(int mouseX, int mouseY) {
}

/**
 * \brief Handle mouse movement over the window (with no button pressed).
 * \param mouseX mouse (cursor) X position
 * \param mouseY mouse (cursor) Y position
 */
void passiveMouseMotionCb(int mouseX, int mouseY) {
	
	//hide cursor
	glutSetCursor(GLUT_CURSOR_NONE);

	// rotate left right
	if (mouseX !=Parameters::WINDOW_WIDTH / 2 && mainCamera->moveable == true) {

		float LeftRightDegrees = -0.1f * (mouseX -Parameters::WINDOW_WIDTH / 2);

		mainCamera->rotateLeftRight(LeftRightDegrees);

	}

	// rotate up and down
	if (mouseY !=Parameters::WINDOW_HEIGHT / 2 && mainCamera->moveable == true) {

		float UpDownDegrees = -0.1f * (mouseY -Parameters::WINDOW_HEIGHT / 2);

		mainCamera->rotateUpDown(UpDownDegrees);

	}

	glutWarpPointer(Parameters::WINDOW_WIDTH / 2,Parameters::WINDOW_HEIGHT / 2);

	glutPostRedisplay();
}


glm::vec3 checkFenceBoundaries(const glm::vec3& oldPosition)
{
	glm::vec3 newPosition = mainCamera->position;
	for (auto object : objects)
	{
		if (object->name == "fence1" || object->name == "fence2" || object->name == "fence7" || object->name == "fence8")
		{
			float dist1 = distance(newPosition, object->position);
			float dist2 = distance(newPosition, object->position + glm::vec3(0.7f, 0.0f, 0.0f));
			float dist3 = distance(newPosition, object->position - glm::vec3(0.7f, 0.0f, 0.0f));
			if (dist1 < 0.5f || dist2 < 0.5f || dist3 < 0.5f)
				newPosition = oldPosition;
		}
		if (object->name == "fence3" || object->name == "fence4" || object->name == "fence5" || object->name == "fence6")
		{
			float dist1 = distance(newPosition, object->position);
			float dist2 = distance(newPosition, object->position + glm::vec3(0.0f, 0.0f, 0.7f));
			float dist3 = distance(newPosition, object->position - glm::vec3(0.0f, 0.0f, 0.7f));
			if (dist1 < 0.5f || dist2 < 0.5f || dist3 < 0.5f)
				newPosition = oldPosition;
		}

	}

	return newPosition;
}

glm::vec3 checkSceneBoundaries(const glm::vec3 & oldPosition)
{
	glm::vec3 newPosition = mainCamera->position;

	if (newPosition.x > Parameters::SCENE_MAX_X)
	{
		newPosition.x = oldPosition.x;
	}
	if (newPosition.x < Parameters::SCENE_MIN_X)
	{
		newPosition.x = oldPosition.x;
	}
	if (newPosition.y > Parameters::SCENE_MAX_Y)
	{
		newPosition.y = oldPosition.y;
	}
	if (newPosition.y < Parameters::SCENE_MIN_Y)
	{
		newPosition.y = oldPosition.y;
	}
	if (newPosition.z > Parameters::SCENE_MAX_Z)
	{
		newPosition.z = oldPosition.z;
	}
	if (newPosition.z < Parameters::SCENE_MIN_Z)
	{
		newPosition.z = oldPosition.z;
	}
	return newPosition;
}


// -----------------------  Timer ---------------------------------

/**
 * \brief Callback responsible for the scene update.
 */
void timerCb(int)
{ 
	glm::vec3 oldPosition = mainCamera->position;

	if (keyMap[KEY_RIGHT_ARROW] == true && mainCamera->moveable == true)
	{
		mainCamera->moveRight();
	}

	if (keyMap[KEY_LEFT_ARROW] == true && mainCamera->moveable == true)
	{
		mainCamera->moveLeft();
	}

	if (keyMap[KEY_UP_ARROW] == true && mainCamera->moveable == true)
	{
		mainCamera->moveForward();
	}

	if (keyMap[KEY_DOWN_ARROW] == true && mainCamera->moveable == true)
	{
		mainCamera->moveBackward();
	}
	
	currentTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME));

	// checkign fence boundaries
	mainCamera->position = checkFenceBoundaries(oldPosition);
	// checking scene boundaries
	mainCamera->position = checkSceneBoundaries(oldPosition);

	for (auto object : objects)
	{
		if (object->name == "bird")
		{
			float speed = 0.1f;

			// Calculate the new position on the ellipse
			float angle = glm::mod(currentTime * speed * 2.0f * glm::pi<float>(), 2.0f * glm::pi<float>());
			float x = ellipseCenter.x + 2.0f * glm::cos(angle);
			float z = ellipseCenter.z + 2.0f * glm::sin(angle);

			glm::vec3 nextPosition = glm::vec3(x, object->position.y, z);

			// Calculate the direction of movement
			glm::vec3 nextDirection = glm::normalize((nextPosition - object->position));
		
			glm::vec3 currentFacingDirection = glm::vec3(cos(glm::radians(object->rotationAngle)), sin(glm::radians(object->rotationAngle)), 0.0f);

			// Calculate the angle between the current facing direction and the direction to the next position
			float angleToNext = acos(glm::dot(currentFacingDirection, nextDirection));

			// Update the bird's position and rotation angle
			object->position = nextPosition;
			object->rotationAngle -= angleToNext;

			if (object->rotationAngle < -360)
				object->rotationAngle += 360;
			object->updateModelMatrix();

			if (cameras[Parameters::MAIN_CAMERA_INDEX]->position == cameras[Parameters::BIRD_CAMERA_INDEX]->position)
			{
				// update bird camera
				cameras[Parameters::BIRD_CAMERA_INDEX]->position = nextPosition + glm::vec3(0.0f, 0.2f, 0.0f);
				cameras[Parameters::BIRD_CAMERA_INDEX]->direction = nextDirection;
				cameras[Parameters::BIRD_CAMERA_INDEX]->updateviewMatrix();
				// update main Camera
				cameras[Parameters::MAIN_CAMERA_INDEX]->position = cameras[Parameters::BIRD_CAMERA_INDEX]->position;
				cameras[Parameters::MAIN_CAMERA_INDEX]->direction = cameras[Parameters::BIRD_CAMERA_INDEX]->direction;
				cameras[Parameters::MAIN_CAMERA_INDEX]->viewMatrix = cameras[Parameters::BIRD_CAMERA_INDEX]->viewMatrix;
			}
			else
			{
				// update bird camera
				cameras[Parameters::BIRD_CAMERA_INDEX]->position = nextPosition + glm::vec3(0.0f, 0.2f, 0.0f);
				cameras[Parameters::BIRD_CAMERA_INDEX]->direction = nextDirection;
				cameras[Parameters::BIRD_CAMERA_INDEX]->updateviewMatrix();
			}

		}
	}
	
	// updating viewMatrix so it is current
	mainCamera->updateviewMatrix();

	// updating spotlight to camera position and direction
	lights["spotlight"]->position = mainCamera->position;
	lights["spotlight"]->spotDirection = mainCamera->direction;

	// milliseconds => seconds
	fire1->currentTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME));
	fire2->currentTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) + Parameters::TIME_DELAY;
	fire3->currentTime = 0.001f * static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) + Parameters::TIME_DELAY * 2;

	// set timeCallback next invocation
	glutTimerFunc(33, timerCb, 0);

	glutPostRedisplay();
}

void loadLights()
{
	// directional
	lights["directional"] = make_shared<Light>(false, glm::vec3(0.01f), glm::vec3(0.2f), glm::vec3(0.2f), glm::vec3(19.2928f, -0.941646f, -39.3773f));
	// pointlight1 u domu
	lights["pointlight1"] = make_shared<Light>(false, glm::vec3(0.05f), glm::vec3(0.9f), glm::vec3(0.9f), glm::vec3(1.22838f, 0.935077f, 1.06817f));
	// pointlight2 u domu
	lights["pointlight2"] = make_shared<Light>(false, glm::vec3(0.05f), glm::vec3(0.9f), glm::vec3(0.9f), glm::vec3(1.22838f, 0.935077f, -1.07979f));
	// pointlight2 u jezera
	lights["pointlight3"] = make_shared<Light>(false, glm::vec3(0.05f), glm::vec3(0.9f), glm::vec3(0.9f), glm::vec3(6.99606f, 0.223246f, 0.402348f));
	// spotlight na kamere
	lights["spotlight"] = make_shared<Light>(true, glm::vec3(0.0f), glm::vec3(0.8f), glm::vec3(0.8f) ,mainCamera->position,
		mainCamera->direction, 50.0f, 20.0f, 50.0f);

}

void loadCameras()
{
	ACamera moveableCamera = make_shared<Camera>(glm::vec3(9.88041f, 0.100676f, 6.42652f), glm::vec3(0.0f, 0.2f, 0.1f) - glm::vec3(9.88041f, 0.100676f, 6.42652f), true);
	cameras.push_back(moveableCamera);
	ACamera staticCamera1 = make_shared<Camera>(glm::vec3(7.844f, -0.670606f, -5.61034f), glm::vec3(0.0f, 0.2f, 0.1f) - glm::vec3(7.844f, -0.670606f, -5.61034f), false);
	cameras.push_back(staticCamera1);
	ACamera staticCamera2 = make_shared<Camera>(glm::vec3(4.23541f, 1.64962f, 6.27931f), glm::vec3(0.0f, 0.2f, 0.0f) - glm::vec3(4.23541f, 1.64962f, 6.27931f), false);
	cameras.push_back(staticCamera2);
	ACamera birdCamera = make_shared<Camera>(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), false);
	cameras.push_back(birdCamera);

	mainCamera = cameras[0];
	mainCamera->cameraSpeed = Parameters::CAMERA_SPEED;
}

void loadObjects()
{
	// static scene

	auto ground = make_shared<SingleMesh>("data/ground.obj", "textures/ground.png", commonShader, "ground", glm::vec3(0.0f), 10.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	cout << "loading ground" << endl;
	objects.push_back(ground);

	auto house = make_shared<SingleMesh>("data/house.obj", "textures/houseWood.jpg", commonShader, "house", glm::vec3(0.0f, 0.0f, 0.0f), 1.5f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	objects.push_back(house);
	cout << "loading house" << endl;

	auto closet = make_shared<SingleMesh>("data/closet.obj", "textures/closet.png", commonShader, "closet", glm::vec3(-0.804239f, -0.383342f, -0.675753f), 0.5f, 270.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	objects.push_back(closet);
	cout << "loading closet" << endl;

	auto tree = make_shared<SingleMesh>("data/tree.obj", "textures/tree.png", commonShader, "tree", glm::vec3(0.980002f, 0.850377f, 2.96923f), 2.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	objects.push_back(tree);
	cout << "loading tree" << endl;

	auto torch1 = make_shared<SingleMesh>("data/torch.obj", "textures/torch.png", commonShader, "torch1", glm::vec3(1.22488f, -0.182026f, 1.08235f), 0.15f, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	objects.push_back(torch1);
	cout << "loading torch1" << endl;

	auto torch2 = make_shared<SingleMesh>("data/torch.obj", "textures/torch.png", commonShader, "torch2", glm::vec3(1.22838f, -0.182026f, -1.07979f), 0.15f, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	objects.push_back(torch2);
	cout << "loading torch2" << endl;

	auto torchStick = make_shared<SingleMesh>("data/torchStick.obj", "textures/torchStick.png", commonShader, "torchStick", glm::vec3(6.99516f, -0.630807f, 0.366883f), 0.4f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	objects.push_back(torchStick);
	cout << "loading torchStick" << endl;

	auto bucket = make_shared<SingleMesh>("data/bucket.obj", "textures/bucket.png", commonShader, "bucket", glm::vec3(1.56181f, -0.750673f, -3.76075f), 0.1f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	objects.push_back(bucket);
	cout << "loading bucket" << endl;

	for (int i = 0; i < 2; i++)
	{
		auto fence1to2 = make_shared<SingleMesh>("data/fence.obj", "textures/fence.png", commonShader, "fence" + to_string(i + 1), glm::vec3(0.339748f - (i * 2.06), -0.72907f, -4.40783f), 1.0f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		objects.push_back(fence1to2);
		
	}
	cout << "loading fence1to2" << endl;
	for (int i = 0; i < 4; i++)
	{
		auto fence3to6 = make_shared<SingleMesh>("data/fence.obj", "textures/fence.png", commonShader, "fence" + to_string(i + 3), glm::vec3(-2.69445f, -0.72907f, -3.25818f + (i * 2.06f)), 1.0f, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		objects.push_back(fence3to6);
		
	}
	cout << "loading fence3to6" << endl;
	for (int i = 0; i < 2; i++)
	{
		auto fence7to8 = make_shared<SingleMesh>("data/fence.obj", "textures/fence.png", commonShader, "fence" + to_string(i + 7), glm::vec3(0.339748f - (i * 2.06), -0.72907f, 4.08783f), 1.0f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));
		objects.push_back(fence7to8);

	}
	cout << "loading fence7to8" << endl;
	
	// hardcoded model 
	auto chair = make_shared<SingleMesh>("textures/wood.jpg", commonShader, "chair", glm::vec3(-0.731236, -0.688409, 0.826383), 0.005f, 270.0f, glm::vec3(1.0f, 0.0f, 0.0f), 120.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	cout << "loading chair" << endl;
	objects.push_back(chair);
	
	lake = make_shared<SingleMesh>("data/lake.obj", "textures/water.jpg", commonShader, "lake", glm::vec3(4.93055f, -0.488689f, -4.02401f), 10.0f, 0.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	lake->isWater = true;
	lake->stencilID = 4;
	objects.push_back(lake);
	cout << "loading lake" << endl;

	bird = make_shared<SingleMesh>("data/bird.obj", "textures/bird.png", commonShader, "bird", glm::vec3(5.43287f, 1.96018f, -3.76011f), 0.5f, -90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	ellipseCenter = bird->position;
	cameras[Parameters::BIRD_CAMERA_INDEX]->position = bird->position;
	objects.push_back(bird);
	cout << "loading bird" << endl;

	fire1 = make_shared<Fire>(fireShader, "fire1", glm::vec3(1.25446f, 0.0140366f, 1.07456f), 0.1f, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f), Parameters::FIRE1_STENCIL_ID);
	cout << "loading fire1" << endl;

	fire2 = make_shared<Fire>(fireShader, "fire2", glm::vec3(1.25446f, 0.0140366f, -1.07600f), 0.1f, 90.0f, glm::vec3(0.0f, 1.0f, 0.0f), Parameters::FIRE2_STENCIL_ID);
	cout << "loading fire2" << endl;

	fire3 = make_shared<Fire>(fireShader, "fire3", glm::vec3(6.99835f, -0.184794f, 0.359371f), 0.1f, 0.0f, glm::vec3(0.0f, 1.0f, 0.0f), Parameters::FIRE3_STENCIL_ID);
	cout << "loading fire3" << endl;

	skybox = make_shared<Skybox>(skyboxShader);
	cout << "loading skybox" << endl;


}
// -----------------------  Application ---------------------------------

/**
 * \brief Initialize application data and OpenGL stuff.
 */
void initApplication() {
	// init OpenGL
	// - all programs (shaders), buffers, textures, ...

	glClearStencil(0);

	loadShaderPrograms();

	loadCameras(); 

	loadLights();

	loadObjects();



	// init your Application
	// - setup the initial application state
}



/**
 * \brief Delete all OpenGL objects and application data.
 */
void finalizeApplication(void) {

	// cleanUpObjects();

	// delete buffers
	// cleanupModels();

	// delete shaders
	//cleanupShaderPrograms();
}


/**
 * \brief Entry point of the application.
 * \param argc number of command line arguments
 * \param argv array with argument strings
 * \return 0 if OK, <> elsewhere
 */
int main(int argc, char** argv) {

	// initialize the GLUT library (windowing system)
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	// for each window
	{
		//   initial window size + title
		glutInitWindowSize(Parameters::WINDOW_WIDTH,Parameters::WINDOW_HEIGHT);
		glutCreateWindow(Parameters::WINDOW_TITLE);

		// callbacks - use only those you need
		glutDisplayFunc(displayCb);
		glutReshapeFunc(reshapeCb);
		glutKeyboardFunc(keyboardCb);
		// glutKeyboardUpFunc(keyboardUpCb);
		glutSpecialFunc(specialKeyboardCb);     // key pressed
		glutSpecialUpFunc(specialKeyboardUpCb); // key released
		glutPassiveMotionFunc(passiveMouseMotionCb);
		glutMouseFunc(mouseCb);
		glutMotionFunc(mouseMotionCb);
#ifndef SKELETON // @task_1_0
		glutTimerFunc(33, timerCb, 0);
#else
		// glutTimerFunc(33, timerCb, 0);
#endif // task_1_0

	}
	// end for each window 

	// initialize pgr-framework (GL, DevIl, etc.)
	if (!pgr::initialize(4, 3))
		pgr::dieWithError("pgr init failed, required OpenGL not supported?");

	// init your stuff - shaders & program, buffers, locations, state of the application
	initApplication();

	// handle window close by the user
	glutCloseFunc(finalizeApplication);

	// Infinite loop handling the events
	glutMainLoop();

	// code after glutLeaveMainLoop()
	// cleanup

	return EXIT_SUCCESS;
}
