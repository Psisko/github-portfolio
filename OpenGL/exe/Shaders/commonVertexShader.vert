/**
 * \file    commonVertexShader.vert : This vertex shader contains calculation of texture offset and calculation of positions and normals to world coordinates
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

in vec3			position;
in vec3			normal;
in vec2			texCoords;

uniform mat4	PVM;			// model to clip coordinates
uniform mat4	modelMatrix;	// model to world coordinates
uniform sampler2D texSampler;	// sampler for the texture access

uniform bool water;				// bool for moving water texture
uniform float time;
uniform float multiplier;

smooth out vec3 positionWS;
smooth out vec3 normalWS;
smooth out vec2 vertTexCoords;

smooth out vec2 vertTexCoordsOffset;

void main() 
{
	
	vec3 newPosition = position;

	if(water)
	{
		float slowedTime = time * 0.0125 * multiplier;

		vec2 offset = vec2(0.0, floor(slowedTime) - slowedTime + 1.0);

		newPosition.y += sin(time + newPosition.x * 30) * 0.005;

		vertTexCoordsOffset = texCoords + offset;
	}

	positionWS = (modelMatrix * vec4(newPosition, 1.0)).xyz;	// position in world coordinates
	normalWS = (modelMatrix * vec4(normal, 0.0)).xyz;		// normal in world coordinates
	vertTexCoords = texCoords;

	gl_Position = PVM * vec4(newPosition, 1.0);
}