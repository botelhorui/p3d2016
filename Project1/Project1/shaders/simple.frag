#version 330 core
out vec4 color;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 LightPos;
} fs_in;

uniform float shininess;
uniform sampler2D diffuseMap1;
uniform sampler2D normalMap1;
uniform sampler2D specularMap1;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{    
    vec3 result;
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 normal = normalize(fs_in.Normal);
    
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
     // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Combine results
    vec3 ambient = 0.1 * vec3(texture(diffuseMap1, fs_in.TexCoords));
    vec3 diffuse = 0.3 * diff * vec3(texture(diffuseMap1, fs_in.TexCoords));
    vec3 specular = 0.3 * spec * vec3(texture(specularMap1, fs_in.TexCoords));

    color = vec4(ambient + diffuse + specular, 1.0f);
}
