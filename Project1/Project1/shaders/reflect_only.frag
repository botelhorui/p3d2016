#version 330 core
out vec4 color;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

uniform float shininess;

uniform sampler2D diffuseMap1;
uniform sampler2D normalMap1;
uniform sampler2D specularMap1;
uniform samplerCube skybox;

uniform vec3 lightPos;
uniform vec3 viewPos;

const float reflectFactor = 0.50;

void main()
{    
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    vec3 R = reflect(-viewDir, normal);
    // cubemap lookup
    color = texture(skybox, R);
}
