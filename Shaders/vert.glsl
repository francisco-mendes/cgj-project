#version 330 core

in vec3 in_Position;
in vec2 in_Texcoord;
in vec3 in_Normal;

out vec3 ex_Position;
out vec2 ex_Texcoord;
out vec3 ex_Normal;
out vec4 ex_Color;

uniform vec4 Color;
uniform mat4 ModelMatrix;

uniform CameraMatrices
{
    mat4 ViewMatrix;
    mat4 ProjectionMatrix;
};

void main(void)
{
	ex_Position = in_Position;
	ex_Texcoord = in_Texcoord;
	ex_Normal = in_Normal;
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(in_Position, 1);
	ex_Color = Color;
}