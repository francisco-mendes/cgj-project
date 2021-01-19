#version 330 core

in vec3 ex_Position;
in vec2 ex_Texcoord;
in vec3 ex_Normal;

out vec4 out_Color;

uniform vec4 Color;
uniform sampler2D Texture;

uniform vec3 Ambient;
uniform vec3 Specular;
uniform float Shininess;

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
        
    float closestDepth = texture(ShadowMap, fragToLight).r;
    out_Color = vec4(vec3(closestDepth), 1.0); 
    return 1.0 - shadow;
}

void main(void)
{
	vec3 light_dir = normalize(Light - ex_Position);
	float shadow = ShadowCalc(ex_Position);

    vec3 normal = normalize(ex_Normal);
	vec3 diffuse = max(dot(light_dir, normal), 0.0) * shadow * Color.rgb;
	
    vec3 view_dir = normalize(Eye - ex_Position);
    vec3 reflect_dir = reflect(-light_dir, normal);  
	vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), Shininess) * shadow * Specular;
	
    out_Color = vec4(diffuse + Ambient, Color.a) * texture(Texture, ex_Texcoord) + vec4(specular, 0);
}