#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
uniform float boost;

void main()
{
    color = vec4(boost * texture(skybox, textureCoordinates).xyz, 1.0f);
}
