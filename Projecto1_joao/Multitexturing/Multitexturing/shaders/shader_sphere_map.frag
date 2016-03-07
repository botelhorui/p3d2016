#version 330

uniform sampler2D texEnvironment;
uniform sampler2D texMap;

out vec4 colorOut;

in Data {
	vec2 texCoord;
	vec2 sphereCoord;
} DataIn;

const float reflectFactor = 0.5;

void main (void) {
	vec4 color = texture(texMap, DataIn.texCoord.st);
	vec4 environment = texture(texEnvironment, DataIn.sphereCoord.st);

	// Write the final pixel.
	//colorOut = color + environment * reflectFactor;
	colorOut = environment * reflectFactor;
	//colorOut = color;
}
