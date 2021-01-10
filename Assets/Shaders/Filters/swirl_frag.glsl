#version 330 core

out vec4 FragColor;

in vec2 ex_Texcoord;

uniform sampler2D screenTexture;
uniform highp vec2 center = vec2 (0.5, 0.5);
uniform highp float radius = 0.5;
uniform highp float angle = 0.5;

void main()
{
    highp vec2 ex_TexcoordToUse=ex_Texcoord;
    highp float dist=distance(center,ex_Texcoord);
    ex_TexcoordToUse-=center;
    if(dist<radius)
    {
        highp float percent=(radius-dist)/radius;
        highp float theta=percent*percent*angle*8.;
        highp float s=sin(theta);
        highp float c=cos(theta);
        ex_TexcoordToUse=vec2(dot(ex_TexcoordToUse,vec2(c,-s)),dot(ex_TexcoordToUse,vec2(s,c)));
    }
    ex_TexcoordToUse+=center;
    
    FragColor=texture2D(screenTexture,ex_TexcoordToUse);
}