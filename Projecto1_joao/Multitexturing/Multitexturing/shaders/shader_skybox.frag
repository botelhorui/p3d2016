#version 330 core

in vec3 texCoords;
out vec4 colorOut;

uniform samplerCube texCubeMap;

void main(){ 
    colorOut = texture(texCubeMap, texCoords);
}
