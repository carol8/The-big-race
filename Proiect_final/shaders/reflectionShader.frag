#version 410 core

in vec3 fNormalEye;
in vec4 fPosEye;

out vec4 fColor;

uniform samplerCube skybox;

uniform int enableFog;

//transparency
uniform float alpha;

float computeFog()
{
	float fogDensity = 0.02f;
	float fragmentDistance = length(fPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2));
	return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
	vec3 viewDirectionN = normalize(fPosEye.xyz);
	vec3 normalN = normalize(fNormalEye);
	vec3 reflection = reflect(viewDirectionN, normalN);
	vec3 reflectionColor = vec3(texture(skybox, reflection));
	vec4 color = vec4(reflectionColor, 1.0f);
	if(enableFog == 1){
		float fogFactor = computeFog();
		vec4 fogColor = vec4(0.584f, 0.584f, 0.584f, 1.0f);

		fColor = mix(fogColor, color, fogFactor);
	}
	else{
		fColor = color;
	}
}
