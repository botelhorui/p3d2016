#version 330

uniform samplerCube texCubeMap;
in vec3 eyeDirection;

out vec4 colorOut;

void main(){
	colorOut = texture(texCubeMap, eyeDirection);
}
