#version 330 core

out vec4 FragColor;

in vec2 ex_Texcoord;

uniform sampler2D screenTexture;

vec3 rgb2hsv(vec3 c)
{
    vec4 K=vec4(0.,-1./3.,2./3.,-1.);
    vec4 p=mix(vec4(c.bg,K.wz),vec4(c.gb,K.xy),step(c.b,c.g));
    vec4 q=mix(vec4(p.xyw,c.r),vec4(c.r,p.yzx),step(p.x,c.r));
    
    float d=q.x-min(q.w,q.y);
    float e=1.e-10;
    return vec3(abs(q.z+(q.w-q.y)/(6.*d+e)),d/(q.x+e),q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K=vec4(1.,2./3.,1./3.,3.);
    vec3 p=abs(fract(c.xxx+K.xyz)*6.-K.www);
    return c.z*mix(K.xxx,clamp(p-K.xxx,0.,1.)*5,c.y);
}

void main()
{
    
    //buscar a cor
    FragColor=texture(screenTexture,ex_Texcoord);
    vec3 fragRGB=FragColor.rgb;
    vec3 fragHSV=rgb2hsv(fragRGB).xyz;
    fragRGB=hsv2rgb(fragHSV);
    FragColor=vec4(fragRGB,FragColor.w);
}