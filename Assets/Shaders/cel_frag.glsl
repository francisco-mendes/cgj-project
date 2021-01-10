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

    vec3 eye = normalize(Eye);

	float intensity =  dot(D, N);
	
	if (intensity > 0.95) {
		intensity = 1;
	} else if (intensity > 0.5) {
		intensity = 0.6;
	} else if (intensity > 0.25) {
		intensity = 0.4;
	} else {
		intensity = 0.2;
	}
	
	if (dot(eye, N) < 0.3) {
	    intensity = 0;
    }
	out_Color = ex_Color * texture(Texture, ex_Texcoord);
	out_Color.xyz *= intensity;
}