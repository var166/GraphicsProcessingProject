#version 410 core

in vec3 textureCoordinates;
out vec4 color;

uniform samplerCube skybox;
uniform float fogDensity;

void main()
{
    vec4 skyboxColor = texture(skybox, textureCoordinates);
    vec4 fogColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
    float fogFactor = exp(-20.0 * fogDensity);
    color = mix(fogColor, skyboxColor, clamp(fogFactor, 0.0, 1.0));
}
