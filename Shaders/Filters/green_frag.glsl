
#version 330 core
out vec4 FragColor;
  
in vec2 ex_Texcoord;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, ex_Texcoord);
    FragColor = vec4(FragColor.r * 0.2, FragColor.g, FragColor.b * 0.2, 1.0);
}