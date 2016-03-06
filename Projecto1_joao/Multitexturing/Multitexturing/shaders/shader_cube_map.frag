#version 330

uniform sampler2D texMap;
uniform samplerCube texCubeMap;

out vec4 colorOut;

struct Materials {
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
	vec4 emissive;
	float shininess;
	int texCount;
};
uniform Materials mat;

//in vec4 tangentDebug;

const float reflect_factor = 0.5;

in Data {
	//vec3 normal;
	//vec3 eye;
	//vec3 lightDir;
	vec2 tex_coord;
	//vec4 tangentDebug;
	vec3 halfVec_tangent;
	vec3 lightVec_tangent;
	vec3 reflected;
} DataIn;

void debug();

void main (void) {
	// Perform a simple 2D texture look up.
	vec3 base_color = texture(texMap, DataIn.tex_coord).rgb;
	// Perform a cube map look up.
	vec4 cube_color = texture(texCubeMap, DataIn.reflected);
	// Write the final pixel.
	//colorOut = vec4(mix(base_color, cube_color, reflect_factor), 1.0);
	colorOut = mix(vec4(base_color, 1.0), cube_color, reflect_factor);
}
