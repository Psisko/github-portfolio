/**
 * \file    skyboxFragmentShader.frag : This fragment shader displays the skybox and adds fog to it
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#version 140

smooth in vec3 positionWS;
in vec3 texCoords;

uniform samplerCube skybox;			// cubemap texture sampler in fragment shader
uniform vec3 cameraPos;

out vec4 fragmentColor;

void main()
{    
    fragmentColor = texture(skybox, texCoords);

    vec4 fogColor = vec4(0.4, 0.4, 0.4, 1.0);
	float d = distance(positionWS, cameraPos);
	float fogDensity = 0.004;
	float fogCoeficient = 1.0 / (exp(d * fogDensity)); 

	fragmentColor = mix(fogColor, fragmentColor, clamp(fogCoeficient, 0.0, 0.8));
}