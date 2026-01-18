#version 410 core

in vec3 fPosition;
in vec3 fNormal;
in vec2 fTexCoords;

out vec4 fColor;

//lighting
uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 spotLightPos;
uniform vec3 spotLightDir;
uniform vec3 spotLightColor;
uniform float spotLightCutoff;
uniform float spotLightOuterCutoff;

uniform mat4 view;
// textures
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D normalTexture;
uniform sampler2D emissiveTexture;
uniform sampler2D metallicTexture;
uniform sampler2D roughnessTexture;

uniform bool hasNormalMap;
uniform bool hasEmissiveMap;

uniform float fogDensity;

//components
float ambientStrength = 0.1f;
float specularStrength = 0.5f;

vec3 computeLight(vec3 lightDir, vec3 lightColor, vec3 baseColor, vec3 specMap)
{
    //compute eye space coordinates
    vec3 normalEye;
    
    if (hasNormalMap) {
        vec3 nm = texture(normalTexture, fTexCoords).rgb;
        nm = normalize(nm * 2.0 - 1.0);

        normalEye = normalize(fNormal + nm * 0.5);
    } else {
        normalEye = normalize(fNormal);
    }

    //normalize light direction
    vec3 lightDirN = vec3(normalize(view * vec4(lightDir, 0.0f)));

    //compute view direction (in eye coordinates, the viewer is situated at the origin
    vec3 viewDir = normalize(- fPosition);

    //compute ambient light
    vec3 localAmbient = ambientStrength * lightColor * baseColor;

    //compute diffuse light
    vec3 localDiffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor * baseColor;

    //compute specular light
    vec3 reflectDir = reflect(-lightDirN, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 localSpecular = specularStrength * specCoeff * lightColor * specMap;

    return (localAmbient + localDiffuse + localSpecular);
}

vec3 computeSpotLight(vec3 baseColor, vec3 specMap)
{
    vec3 lightPosEye = vec3(view * vec4(spotLightPos, 1.0f));
    vec3 lightDirEye = vec3(normalize(view * vec4(spotLightDir, 0.0f)));

    vec3 lightVec = normalize(lightPosEye - fPosition);
    float dist = length(lightPosEye - fPosition);

    // Spotlight effect
    float theta = dot(lightVec, normalize(-lightDirEye));
    float epsilon = spotLightCutoff - spotLightOuterCutoff;
    float intensity = clamp((theta - spotLightOuterCutoff) / epsilon, 0.0, 1.0);

    // attenuation
    float constant = 1.0f;
    float linear = 0.045f;
    float quadratic = 0.0075f;
    float attenuation = 1.0 / (constant + linear * dist + quadratic * (dist * dist));

    // compute eye space coordinates
    vec3 normalEye;
    if (hasNormalMap) {
        vec3 nm = texture(normalTexture, fTexCoords).rgb;
        nm = normalize(nm * 2.0 - 1.0);
        normalEye = normalize(fNormal + nm * 0.5);
    } else {
        normalEye = normalize(fNormal);
    }

    // view direction
    vec3 viewDir = normalize(-fPosition);

    // ambient
    vec3 localAmbient = ambientStrength * spotLightColor * baseColor;

    // diffuse
    vec3 localDiffuse = max(dot(normalEye, lightVec), 0.0f) * spotLightColor * baseColor;

    // specular
    vec3 reflectDir = reflect(-lightVec, normalEye);
    float specCoeff = pow(max(dot(viewDir, reflectDir), 0.0f), 32);
    vec3 localSpecular = specularStrength * specCoeff * spotLightColor * specMap;

    return (localAmbient + localDiffuse + localSpecular) * attenuation * intensity;
}

float computeFog()
{
    float dist = length(fPosition);
    float fogFactor = exp(-pow(dist * fogDensity, 2.0f));

    return clamp(fogFactor, 0.0f, 1.0f);
}

void main() 
{
    vec3 baseColor = texture(diffuseTexture, fTexCoords).rgb;
    vec3 specMap = texture(specularTexture, fTexCoords).rgb;

    vec3 color = computeLight(lightDir, lightColor, baseColor, specMap);
    color += computeSpotLight(baseColor, specMap);
    color = min(color, 1.0f);

    if (hasEmissiveMap) {
        color += texture(emissiveTexture, fTexCoords).rgb;
    }

    float fogFactor = computeFog();
    vec4 fogColor = vec4(0.3f, 0.3f, 0.3f, 1.0f);
    fColor = mix(fogColor, vec4(color, 1.0f), fogFactor);
}
