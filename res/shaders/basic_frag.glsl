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

uniform vec3 u_ViewPos;
uniform vec3 u_VertexColor;
uniform Material u_Material;
uniform PointLight u_PointLight;

out vec4 color;

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

    // Final color
    vec3 result = (ambient + diffuse + specular) * u_PointLight.color * texColor /* * attenuation */;

    color = vec4(result, 1.0);
}
