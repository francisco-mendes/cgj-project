#version 330 core

in vec3 in_Position;

uniform mat4 ModelMatrix;

void main(void)
{
	gl_Position = ModelMatrix * vec4(in_Position, 1);
}