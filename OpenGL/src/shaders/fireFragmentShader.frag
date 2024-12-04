/**
 * \file    fireFragmentShader.frag : This fragment shader calculates the next frame of the texture image and displays it
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#version 140

in vec2 vertTexCoords;

uniform float time;
uniform sampler2D texSampler;

out vec4 fragmentColor;

void main()
{    
	// scale texture coordinate to one frame (texture is 5 frames in width and 5 in height)
	vec2 offsetTexCoord = vec2(vertTexCoords.x / 5, vertTexCoords.y / 5);

	int frame = int(time / 0.08);

	offsetTexCoord += vec2(frame % 5, frame / 5) / vec2(5, 5);

    fragmentColor = texture(texSampler, offsetTexCoord);
}