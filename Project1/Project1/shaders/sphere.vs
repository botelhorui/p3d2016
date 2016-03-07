#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec2 SphereTexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
    vs_out.FragPos = vec3(model * vec4(position, 1.0f));
    vs_out.Normal = mat3(transpose(inverse(model))) * normal;
    vec3 u = vs_out.FragPos - viewPos;
    vec3 n = vs_out.Normal;
    vec3 r = reflect(u, n);
    float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) * (r.z + 1.0));
    vs_out.SphereTexCoords = vec2(r.x/m + 0.5, r.y/m + 0.5);
    vs_out.TexCoords = texCoords;
}