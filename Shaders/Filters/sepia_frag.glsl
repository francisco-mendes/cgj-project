
#version 330 core
out vec4 FragColor;

in vec2 ex_Texcoord;

uniform sampler2D screenTexture;

void main()
{
    lowp vec4 textureColor=texture2D(screenTexture,ex_Texcoord);
    lowp float outputRed=(textureColor.x*.393)+(textureColor.y*.769)+(textureColor.z*.189);
    lowp float outputGreen=(textureColor.x*.349)+(textureColor.y*.686)+(textureColor.z*.168);
    lowp float outputBlue=(textureColor.x*.272)+(textureColor.y*.534)+(textureColor.z*.131);
    FragColor= vec4(outputRed, outputGreen, outputBlue, 1.0);
}