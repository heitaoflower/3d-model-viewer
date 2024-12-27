
#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D diffuse;
uniform vec4 color;
void main()
{
  if (color.a < 0.1)
    FragColor = texture(diffuse, TexCoord);
  else
    FragColor = color;
}
