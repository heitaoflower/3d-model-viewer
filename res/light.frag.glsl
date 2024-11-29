#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec4 color;
uniform float intensityMultiplier;
uniform vec3 lightPos;
uniform vec3 viewPos; 
uniform float shininess;

void main()
{
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    vec3 normalizedNormal = normalize(Normal);
    float diff = max(dot(normalizedNormal, lightDir), 0.0);
    float spec = pow(max(dot(normalizedNormal, halfDir), 0.0), shininess); 

    vec3 ambient = 0.1 * vec3(texture(texture1, TexCoord)); // Ambient component
    vec3 diffuse = diff * vec3(texture(texture1, TexCoord)); // Diffuse component
    vec3 specular = spec * vec3(1.0); // Specular component

    vec3 result = (ambient + diffuse + specular) * intensityMultiplier;

    if (color.a < 0.1)
        FragColor = vec4(result, 1.0);
    else
        FragColor = color * vec4(result, 1.0);
}

