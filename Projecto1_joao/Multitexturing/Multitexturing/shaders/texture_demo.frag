#version 330

uniform sampler2D texmap;
uniform sampler2D texNormal;
uniform samplerCube texCubeMap;

uniform int texMode;
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

in Data {
	//vec3 normal;
	//vec3 eye;
	//vec3 lightDir;
	vec2 tex_coord;
	//vec4 tangentDebug;
	vec3 halfVec_tangent;
	vec3 lightVec_tangent;
} DataIn;

//in vec4 tangentDebug;

void main() {
	//colorOut = vec4(DataIn.tangentDebug.xyz, 1.0);
	//colorOut = vec4(DataIn.eye, 1.0);

	vec3 normalFromTexture = 2.0 * texture(texNormal, DataIn.tex_coord).rgb - 1.0;
	normalFromTexture = normalize(normalFromTexture);

	//colorOut = vec4(normalFromTexture, 1.0);

	//vec3 l = normalize(DataIn.lightDir);
	vec3 l = normalize(DataIn.lightVec_tangent);

	float intensity = max(dot(l, normalFromTexture), 0.0);
	//float intensity = max(dot(normalFromTexture, l), 0.0);

	if(intensity > 0.0){
		float intSpec = max(dot(DataIn.halfVec_tangent, normalFromTexture), 0.0);
		vec4 spec = mat.specular * pow(intSpec, mat.shininess);
		vec4 texel = texture(texmap, DataIn.tex_coord);  // texel from lighwood.tga
		colorOut = max(intensity * mat.diffuse * texel + spec, mat.ambient * texel);
	}

/*
	vec4 diffuseMaterial = vec4(0.0);
	vec4 spec = vec4(0.0);

	colorOut = vec4(0.0);

	if(intensity > 0.0){
		diffuseMaterial = texture(texmap, DataIn.tex_coord) * mat.diffuse;
		
		// shininess
		float intSpec = max(dot(DataIn.halfVec_tangent, normalFromTexture), 0.0);
		spec = mat.specular * pow(intSpec, mat.shininess);

		//colorOut = diffuseMaterial * intensity
		colorOut = intensity * diffuseMaterial + mat.specular;
	}

	colorOut += mat.ambient;
*/
}
