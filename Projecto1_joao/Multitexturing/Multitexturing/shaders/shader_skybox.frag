#version 330

uniform samplerCube texCubeMap;

in vec3 texCoords;

out vec4 colorOut;

in vec3 eyeDirection;

void main(){
/** /
	//colorOut = vec4(eyeDirection, 1.0);
	colorOut = texture(texCubeMap, eyeDirection);
/**/
    colorOut = texture(texCubeMap, texCoords);
	//colorOut = vec4(1.0, 0.0, 0.0, 1.0);
	//colorOut = vec4(texCoords, 1.0);
/**/
}
