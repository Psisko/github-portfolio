#version 140

struct Material {           // structure that describes currently used material
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

struct Light
{
	// for all light types
	vec3	ambient;
	vec3	diffuse;
	vec3	specular;
	// position
	vec3 position;
	// for spot light
	
	//vec3 spotDirection;
	//float spotCosCutOff;
	//float spotExponent;

};

// light uniforms
uniform Light light;

// object uniforms
uniform mat4	PVM;
uniform mat4	ModelMatrix;
uniform sampler2D texSampler;  // sampler for the texture access
uniform Material material;     // current material

smooth in vec2 vertTexCoords;
smooth in vec3 normalWS;
smooth in vec3 positionWS;

out vec4 fragmentColor;

vec4 directionaLight(Light light, Material material, vec3 position, vec3 normal)
{
	vec3 dir = vec3(0.0);

	vec3 lightDir = normalize(-light.position); // position is direction in directionalLight
	vec3 reflectDir = reflect(-lightDir, normalWS);
	vec3 viewDir = normalize(- positionWS);

	dir += material.ambient * light.ambient;
	dir += material.diffuse * light.diffuse * max(0.0, dot(normalWS, lightDir));
	dir += material.specular * light.specular * pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);

	//vec3 zbytecnost = light.spotDirection + light.spotCosCutOff + light.spotExponent;

	return vec4(dir, 0.0);
}


void main() 
{
	// initialize the output color with the global ambient term
	vec3 globalAmbientLight = vec3(0.4);

	fragmentColor = vec4(material.ambient * globalAmbientLight, 1.0);

	fragmentColor += directionaLight(light, material, positionWS, normalize(normalWS));

	if(material.useTexture)
		fragmentColor *= texture(texSampler, vertTexCoords);

}