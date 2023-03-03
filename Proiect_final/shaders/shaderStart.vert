#version 410 core

layout(location=0) in vec3 vPosition;
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoords;

out vec3 fNormalEye;
out vec4 fPos;
out vec4 fPosEye;
out vec2 fTexCoords;
out vec4 fragPosLightSpace;

flat out vec4 flat_fPos;
flat out vec3 flat_fNormalEye;
flat out vec4 flat_fPosEye;
flat out vec2 flat_fTexCoords;
flat out vec4 flat_fragPosLightSpace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform	mat3 normalMatrix;
uniform mat4 lightSpaceTrMatrix;

void main() 
{
	//compute eye space coordinates
	fPos = model * vec4(vPosition, 1.0f);
	fPosEye = view * fPos;
	fNormalEye = normalize(normalMatrix * vNormal);
	fTexCoords = vTexCoords;
	fragPosLightSpace = lightSpaceTrMatrix * model * vec4(vPosition, 1.0f);
	gl_Position = projection * view * model * vec4(vPosition, 1.0f);

	//flat
	flat_fPos = fPos;
	flat_fPosEye = fPosEye;
	flat_fNormalEye = fNormalEye;
	flat_fTexCoords = fTexCoords;
	flat_fragPosLightSpace = fragPosLightSpace;
}
