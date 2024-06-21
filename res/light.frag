#version 330 core

in vec2 TexCoord;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture1;
uniform vec4 color;
uniform float intensityMultiplier;
void main()
{
  vec3 lightDir = vec3(0.0, 1.0, 1.0);
  vec3 normalizedNormal = normalize(Normal);
  vec3 normalizedLightDir = normalize(lightDir);
  float intensity = max(dot(normalizedNormal, normalizedLightDir), intensityMultiplier);

  if (color.a < 0.1)
    FragColor = texture(texture1, TexCoord) * vec4(vec3(intensity), 1.0);
  else
     FragColor = color * vec4(vec3(intensity), 1.0); 
}
