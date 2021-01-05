#version 330 core

out vec4 FragColor;
  
in vec2 ex_Texcoord;

uniform sampler2D screenTexture;

uniform float brightness = 1;
uniform float contrast = 5;

void main() {

	vec3 color = texture2D(screenTexture, ex_Texcoord).rgb;
	vec3 colorContrasted = (color) * contrast;
	vec3 bright = colorContrasted + vec3(brightness,brightness,brightness);
	FragColor.rgb = bright;
	FragColor.a = 1.;
}