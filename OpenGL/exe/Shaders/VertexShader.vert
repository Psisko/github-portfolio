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

uniform mat4	PVM;
uniform mat4	modelMatrix;
uniform sampler2D texSampler;  // sampler for the texture access

smooth out vec2 vertTexCoords;
smooth out vec3 normalWS;
smooth out vec3 positionWS;


void main() 
{

	gl_Position = PVM * vec4(position, 1.0);

	positionWS = (modelMatrix * vec4(position, 1.0)).xyz;
	normalWS = (modelMatrix * vec4(normal, 0.0)).xyz;
	vertTexCoords = texCoords;



}