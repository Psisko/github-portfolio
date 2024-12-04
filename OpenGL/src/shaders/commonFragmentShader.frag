/**
 * \file    commonFragmentShader.frag : This fragment shader contains lighting calculations, texture usage and fog usage
 * \author  Patrik Krulík
 * \date    15/05/2024
 */

#version 140

struct Material				// structure that describes currently used material
{           
  vec3  ambient;            // ambient component
  vec3  diffuse;            // diffuse component
  vec3  specular;           // specular component
  float shininess;          // sharpness of specular reflection

  bool  useTexture;         // defines whether the texture is used or not
};

struct Light				// structure that describes pointLight and directionalLight
{
	vec3	ambient;		// ambient component
	vec3	diffuse;		// diffuse component
	vec3	specular;		// specular component

	vec3	position;		// position component
};

struct Spotlight			// structure that describes spotLight
{
	vec3	ambient;		// ambient component
	vec3	diffuse;		// diffuse component
	vec3	specular;		// specular component

	vec3	position;		// position component

	vec3	spotDirection;	// spotDirection component for direction of the cone
	float	spotCosCutOff;	// spotCosCutOff component for size of cone
	float	spotExponent;	// spotExponent component for bigger intensity inside the cone
	float	spotAttenuation;// spotAttenuation component for calculation intensity and distance of light
};

// light uniforms
uniform Light		dLight;
uniform Light		p1Light;
uniform Light		p2Light;
uniform Light		p3Light;
uniform Spotlight	sLight;


// object uniforms
uniform sampler2D	texSampler;			// sampler for the texture access
uniform Material	material;			// current material

uniform vec3		cameraPos;			// camera position in world coordinates

uniform bool		water;				// bool if model is water
uniform float		time;				// time for changing spotlight intensity and distance

smooth in vec3		positionWS;			// position in world coordinates
smooth in vec3		normalWS;			// normal in world coordinates
smooth in vec2		vertTexCoords;		// texture coordinates
smooth in vec2		vertTexCoordsOffset;// texture coordinates offseted


out vec4 fragmentColor;

vec4 directionaLight(Light light, Material material)
{
	vec3 dir = vec3(0.0);

	vec3 lightDir = normalize(-light.position);		// position is direction in directionalLight
	vec3 reflectDir = reflect(-lightDir, normalize(normalWS));
	vec3 viewDir = normalize(cameraPos - positionWS); // view from position to camera

	dir += material.ambient * light.ambient;

	if(dot(viewDir, normalize(normalWS)) < 0.0)
		return vec4(dir, 1.0);

	dir += material.diffuse * light.diffuse * max(0.0, dot(normalize(normalWS), lightDir));
	dir += material.specular * light.specular * pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);

	return vec4(dir, 0.0);
}

vec4 spotLight(Spotlight light, Material material)
{

	vec3 spot = vec3(0.0);

	vec3 dirToLight = normalize(light.position - positionWS);
	vec3 reflectDir = reflect(-dirToLight, normalize(normalWS));
	vec3 viewDir = normalize(cameraPos - positionWS);

	spot += material.ambient * light.ambient;

	float NdotL = max(0.0, dot(normalize(normalWS), dirToLight));
	
	float spotCoef = max(0.0, dot(-dirToLight, light.spotDirection));

	// light ray is within the cone => add diffuse and specular component
	if(spotCoef < light.spotCosCutOff) 
	{
		float dist = length(light.position - positionWS);
		float distAttenuate = 250 / (light.spotAttenuation * dist * dist);
		if(dist > 4)
			distAttenuate = 0;

		vec3 R = reflect(-dirToLight, normalize(normalWS));
		float RdotV = max(0.0, dot(reflectDir, viewDir));

		spot += material.diffuse * light.diffuse * NdotL;

		spot += material.specular * light.specular * pow(RdotV, material.shininess);
    
		// adjust the light intensity within the cone
		spot *= pow(spotCoef, light.spotExponent);

		spot *= distAttenuate;

	}

	return vec4(spot, 0.0);
}

vec4 pointLight(Light light, Material material, float pTime)
{
	vec3 point = vec3(0.0);

	vec3 lightDir = normalize(light.position - positionWS);
	vec3 reflectDir = reflect(-lightDir, normalize(normalWS));
	vec3 viewDir = normalize(cameraPos - positionWS); // view from position to camera

	point += material.ambient * light.ambient;

	if(dot(viewDir, normalize(normalWS)) < 0.0)
		return vec4(point, 1.0);

	float dist = length(light.position - positionWS);

	dist += sin(pTime)/(int(pTime) % 4 + 2) * 0.6;
	
	float distAttenuate = 1.0 / (1.0 + 0.2 * dist + 0.8 * dist * dist);  

	// pokud je vzdalenost vetsi nez 5, neosvetluj
	if(dist > 5.0)
		distAttenuate = 0;

	point += material.diffuse * light.diffuse * max(0.0, dot(normalize(normalWS), lightDir));
	point += material.specular * light.specular * pow(max(0.0, dot(reflectDir, viewDir)), material.shininess);

	point *= distAttenuate;

	return vec4(point, 0.0);
}


void main() 
{
	vec3 globalAmbientLight = vec3(0.1);

	fragmentColor = vec4(material.ambient * globalAmbientLight, 1.0);
	// calculating lights
	fragmentColor += directionaLight(dLight, material);
	fragmentColor += spotLight(sLight, material);
	fragmentColor += pointLight(p1Light, material, time);
	fragmentColor += pointLight(p2Light, material, time + 4);
	fragmentColor += pointLight(p3Light, material, time + 8);
	// using texture
	if(material.useTexture && !water)
		fragmentColor *= texture(texSampler, vertTexCoords);

	if(material.useTexture && water)
		fragmentColor *= texture(texSampler, vertTexCoordsOffset);
	// calculating fog
	vec4 fogColor = vec4(0.4, 0.4, 0.4, 1.0);
	float d = distance(positionWS, cameraPos);
	float fogDensity = 0.04;
	float fogCoeficient = 1.0 / (exp(d * fogDensity)); 

	fragmentColor = mix(fogColor, fragmentColor, clamp(fogCoeficient, 0.0, 0.8));

}