#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    vec3 LightPos;
} fs_in;

uniform sampler2D diffuseMap1;
uniform sampler2D normalMap1;
uniform sampler2D specularMap1;

void main()
{           
    // Get diffuse color
    vec3 texel = texture(diffuseMap1, fs_in.TexCoords).rgb;
    vec3 normal = normalize(fs_in.Normal);  // this normal is in tangent space

    // Diffuse shading
    vec3 lightDir = normalize(fs_in.LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    
    // Specular shading
    vec3 viewDir = normalize(-fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = texture(specularMap1, fs_in.TexCoords).rgb;

    // combine
    vec3 ambient = 0.1 * texel;
    vec3 diffuse = diff * texel;
    // Specular    
    if(length(specular) > 0.0){
        specular *= spec;
    }else{
        // if specular map is not defined
        specular = vec3(0.2) * spec;
    }

    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}