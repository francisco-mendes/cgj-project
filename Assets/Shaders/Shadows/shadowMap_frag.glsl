#version 330 core

out vec4 ex_Pos;

uniform SceneGlobals 
{
    vec3 Eye;
    vec3 Light;
};

uniform Shadows {
    mat4 ShadowMatrices[6];
    float FarPlane;
};


void main()
{
    // get distance between fragment and light source
    float lightDistance = length(ex_Pos.xyz - Light) / FarPlane;
    // write this as modified depth
    gl_FragDepth = lightDistance;
}  