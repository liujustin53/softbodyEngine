#version 410 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D normal;
    float shininess;
};

struct PointLight
{
    vec3 position;
    vec3 color;

    float ambient;
    float diffuse;
    float specular;

    float constant;
    float linear;
    float quadratic;
};

in vec3 fragPos;
in vec2 texCoord;
in vec3 vertexNormal;
in vec4 fragPosLightSpace;

uniform vec3 u_ViewPos;
uniform vec3 u_VertexColor;
uniform Material u_Material;
uniform PointLight u_PointLight;
uniform sampler2D u_DepthMap;

out vec4 color;

float shadowCalculation()
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(u_DepthMap, projCoords.xy).r;
    float currentDepth = projCoords.z;

    vec3 normal = normalize(vertexNormal);
    vec3 lightDir = normalize(u_PointLight.position - fragPos);
    float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);

    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(u_DepthMap, 0);
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(u_DepthMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
        }
    }
    shadow /= 9.0;

    if (projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

// Entry point of program
void main()
{
    vec3 texColor = u_VertexColor;
    vec3 specularColor = vec3(1.0);
    vec3 normal = normalize(vertexNormal);
    // Maps
    if (texCoord.x >= 0.0 && texCoord.y >= 0.0) {
        texColor = texture(u_Material.diffuse, texCoord).rgb;
        specularColor = texture(u_Material.specular, texCoord).rgb;
        normal = texture(u_Material.normal, texCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);
    }

    // Ambient
    vec3 ambient = vec3(u_PointLight.ambient);

    // Diffuse
    vec3 lightDir = normalize(u_PointLight.position - fragPos);
    float diffImpact = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = vec3(u_PointLight.diffuse) * diffImpact;

    // Specular
    vec3 viewDir = normalize(u_ViewPos - fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float specImpact = pow(max(dot(viewDir, reflectDir), 0.0), u_Material.shininess);
    vec3 specular = u_PointLight.specular * specImpact * specularColor;

    // Attenuation
    float distance = length(u_PointLight.position - fragPos);
    float attenuation = 1.0 / (u_PointLight.constant + u_PointLight.linear * distance
                               + u_PointLight.quadratic * (distance * distance));

    // Shadow
    float shadow = shadowCalculation();

    // Final color
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * u_PointLight.color * texColor /* * attenuation */;

    color = vec4(result, 1.0);
}
