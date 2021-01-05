#version 330 core

in vec2 in_Pos;
in vec2 in_Texcoord;

out vec2 ex_Texcoord;

void main()
{
    gl_Position = vec4(in_Pos.x, in_Pos.y, 0.0, 1.0); 
    ex_Texcoord = in_Texcoord;
}  