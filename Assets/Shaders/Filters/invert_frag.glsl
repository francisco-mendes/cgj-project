
#version 330 core
out vec4 FragColor;
  
in vec2 ex_Texcoord;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = vec4(vec3(1.0 - texture(screenTexture, ex_Texcoord)), 1.0);
}