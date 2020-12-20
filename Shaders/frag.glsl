#version 330 core

in vec3 ex_Position;
in vec2 ex_Texcoord;
in vec3 ex_Normal;

in vec4 ex_Color;
out vec4 out_Color;

in vec3 LightPos;	

void main(void)
{
	vec3 direction = vec3(1.0, 0.5, 0.25);
    vec3 N = normalize(ex_Normal);
	vec3 D = normalize(direction);
	
	float intensity = max(dot(D, N), 0.0);
	out_Color = ex_Color * intensity;
}