#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 LightPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(view * model * vec4(position, 1.0));   
    vs_out.TexCoords = texCoords;    
    vs_out.Normal = transpose(inverse(mat3(view* model))) * normal;
    vs_out.LightPos = vec3(view * vec4(lightPos, 1.0));
}