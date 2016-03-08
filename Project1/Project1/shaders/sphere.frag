#version 330 core
out vec4 color;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec2 SphereTexCoords;
} fs_in;

uniform sampler2D diffuseMap1;
uniform sampler2D normalMap1;
uniform sampler2D specularMap1;
uniform sampler2D sphereMap;

void main()
{    
    color = texture(sphereMap, vec2(fs_in.SphereTexCoords.x, 1 - fs_in.SphereTexCoords.y));
}
