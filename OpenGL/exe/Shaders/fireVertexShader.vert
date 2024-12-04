/**
 * \file    fireVertexShader.vert : This vertex shader sends FS position in WS and interpolated TexCoords
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#version 140

in vec3 position;
in vec2 texCoords;

uniform mat4 PVM;

smooth out vec2 vertTexCoords;

void main()
{
    gl_Position = PVM * vec4(position, 1.0);
    
    vertTexCoords = texCoords;
}  