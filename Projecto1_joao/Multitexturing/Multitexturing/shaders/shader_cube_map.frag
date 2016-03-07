#version 330

uniform sampler2D texMap;
uniform samplerCube texCubeMap;

out vec4 colorOut;

const float reflectFactor = 0.75;

in Data {
	vec2 tex_coord;
	vec3 reflected;
} DataIn;

void main (void) {
	// Perform a simple 2D texture look up.
	vec3 baseColor = texture(texMap, DataIn.tex_coord).rgb;

	// Perform a cube map look up.
	vec3 cubeColor = texture(texCubeMap, DataIn.reflected).rgb;

	// Write the final pixel.
	colorOut = vec4(mix(baseColor, cubeColor, reflectFactor), 1.0);
}
