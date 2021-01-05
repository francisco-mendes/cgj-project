
#version 330 core
out vec4 FragColor;

in vec2 ex_Texcoord;

uniform sampler2D screenTexture;

void main()
{
    vec4 top=texture(screenTexture,vec2(ex_Texcoord.x,ex_Texcoord.y+1./200.));
    vec4 bottom=texture(screenTexture,vec2(ex_Texcoord.x,ex_Texcoord.y-1./200.));
    vec4 left=texture(screenTexture,vec2(ex_Texcoord.x-1./300.,ex_Texcoord.y));
    vec4 right=texture(screenTexture,vec2(ex_Texcoord.x+1./300.,ex_Texcoord.y));
    vec4 topLeft=texture(screenTexture,vec2(ex_Texcoord.x-1./300.,ex_Texcoord.y+1./200.));
    vec4 topRight=texture(screenTexture,vec2(ex_Texcoord.x+1./300.,ex_Texcoord.y+1./200.));
    vec4 bottomLeft=texture(screenTexture,vec2(ex_Texcoord.x-1./300.,ex_Texcoord.y-1./200.));
    vec4 bottomRight=texture(screenTexture,vec2(ex_Texcoord.x+1./300.,ex_Texcoord.y-1./200.));
    vec4 sx=-topLeft-2*left-bottomLeft+topRight+2*right+bottomRight;
    vec4 sy=-topLeft-2*top-topRight+bottomLeft+2*bottom+bottomRight;
    vec4 sobel=sqrt(sx*sx+sy*sy);
    FragColor=sobel;
}