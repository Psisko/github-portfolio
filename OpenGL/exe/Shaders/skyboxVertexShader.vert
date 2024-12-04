/**
 * \file    skyboxVertexShader.vert : This vertex shader sends FS texCoords and interpolated position for calculating distance in WS
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#version 140

in vec3 position;

uniform mat4 PV;

smooth out vec3 positionWS;
out vec3 texCoords;


void main()
{
    gl_Position = PV * vec4(position, 1.0);
    
    positionWS = position;
    texCoords = position;
}  