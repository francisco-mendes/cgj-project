#version 330 core

in vec3 ex_Position;
in vec2 ex_Texcoord;
in vec3 ex_Normal;

in vec4 ex_Color;
out vec4 out_Color;

uniform vec3 Eye;
uniform vec3 Light;
uniform sampler2D Texture;

void main(void)
{
	vec3 D = normalize(Light - ex_Position);
    vec3 N = normalize(ex_Normal);
	
	float intensity = max(dot(D, N), 0.0);
    out_Color = ex_Color * texture(Texture, ex_Texcoord);
	out_Color.xyz *= intensity;
}