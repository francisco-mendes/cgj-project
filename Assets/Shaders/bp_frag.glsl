#version 330 core

in vec3 ex_Position;
in vec2 ex_Texcoord;
in vec3 ex_Normal;

out vec4 out_Color;

uniform vec4 Color;

uniform vec3 Ambient;
uniform vec3 Specular;
uniform float Shininess;

uniform sampler2D Texture;

uniform SceneGlobals 
{
    vec3 Eye;
    vec3 Light;
};

void main(void)
{
	vec3 light_dir = normalize(Light - ex_Position);
    vec3 normal = normalize(ex_Normal);
	vec3 diffuse = max(dot(light_dir, normal), 0.0) * Color.rgb;
	
    vec3 view_dir = normalize(Eye - ex_Position);
    vec3 reflect_dir = reflect(-light_dir, normal);  
	vec3 specular = pow(max(dot(view_dir, reflect_dir), 0.0), Shininess) * Specular;
	
	out_Color = vec4(Ambient + diffuse + specular, Color.a);
    out_Color *= texture(Texture, ex_Texcoord);
}