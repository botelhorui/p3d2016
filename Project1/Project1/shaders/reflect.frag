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
    
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
     // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // Combine results
    vec3 diffuseColor = texture(diffuseMap1, fs_in.TexCoords).rgb;
    vec3 specularColor = texture(specularMap1, fs_in.TexCoords).rgb;

    vec3 ambient = vec3(0.5) * diffuseColor;
    vec3 diffuse = vec3(1.0) * diff * diffuseColor;
    vec3 specular = vec3(1.0) * spec * specularColor;

    vec4 textureColor = vec4(ambient + diffuse + specular, 1.0f);

    vec3 R = reflect(-viewDir, normal);
    // cubemap lookup
    vec4 cubeColor = texture(skybox, R);
    color = mix(textureColor, cubeColor, reflectFactor);
    
}
