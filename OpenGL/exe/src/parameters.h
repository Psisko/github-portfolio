/**
 * \file    parameters.h : This header file contains values of parameters used in the scene
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#pragma once

namespace Parameters
{
	constexpr int WINDOW_WIDTH = 1800;
	constexpr int WINDOW_HEIGHT = 900;
	constexpr char WINDOW_TITLE[] = "PGR: Tajemná chata";

	constexpr float SCENE_WIDTH = 5.0f;
	constexpr float SCENE_HEIGHT = 5.0f;


	constexpr float SCENE_MAX_X = 10.0f;
	constexpr float SCENE_MIN_X = -10.0f;

	constexpr float SCENE_MAX_Y = 10.0f;
	constexpr float SCENE_MIN_Y = -0.671379f;

	constexpr float SCENE_MAX_Z = 10.0f;
	constexpr float SCENE_MIN_Z = -10.0f;

	constexpr float CAMERA_SPEED = 0.125f;

	constexpr int MAIN_CAMERA_INDEX = 0;
	constexpr int ST1_CAMERA_INDEX = 1;
	constexpr int ST2_CAMERA_INDEX = 2;
	constexpr int BIRD_CAMERA_INDEX = 3;

	constexpr int FIRE1_STENCIL_ID = 1;
	constexpr int FIRE2_STENCIL_ID = 2;
	constexpr int FIRE3_STENCIL_ID = 3;

	constexpr float TIME_DELAY = 0.02f;
}