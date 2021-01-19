#version 330 core

in vec3 ex_Position;
in vec2 ex_Texcoord;
in vec3 ex_Normal;

out vec4 out_Color;

uniform vec4 Color;
uniform sampler2D Texture;

uniform vec3 Ambient;

uniform samplerCube ShadowMap;

uniform Shadows {
    mat4 ShadowMatrices[6];
    float FarPlane;
};

uniform SceneGlobals 
{
    vec3 Eye;
    vec3 Light;
};

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalc(vec3 pos) {
    vec3 fragToLight = pos - Light;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(Eye - pos);
    float diskRadius = (1.0 + (viewDistance / FarPlane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(ShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= FarPlane;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
    float closestDepth = texture(ShadowMap, fragToLight).r ;
    out_Color = vec4(vec3(closestDepth), 1.0); 
    return 1.0 - shadow;
}

void main(void)
{
  	vec3 D = normalize(Light - ex_Position);
    vec3 N = normalize(ex_Normal);

    vec3 eye = normalize(Eye);

	float intensity = max(dot(D, N), 0.0);
	
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
    intensity *= ShadowCalc(ex_Position);
    
	out_Color = Color * texture(Texture, ex_Texcoord);
	out_Color.rgb = out_Color.rgb * intensity + Ambient;
}