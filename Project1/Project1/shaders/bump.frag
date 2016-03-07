#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform float shininess;

uniform sampler2D diffuseMap1;
uniform sampler2D normalMap1;

uniform bool normalMapping;

void main()
{           
    
    // Obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap1, fs_in.TexCoords).rgb;

    // Transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);

    // Get diffuse color
    vec3 color = texture(diffuseMap1, fs_in.TexCoords).rgb;
    
    // Diffuse    
    float diff = max(dot(lightDir, normal), 0.0);
    
    // Specular    
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);    

    // Combine results
    vec3 ambient = 0.2 * color;
    vec3 diffuse = 0.7 * diff * color;
    vec3 specular = 0.3 * spec * color;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0f);

}