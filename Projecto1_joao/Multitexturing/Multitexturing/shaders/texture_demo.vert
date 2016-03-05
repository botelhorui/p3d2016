#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;


uniform vec4 l_pos;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;
in vec4 tangent;

out Data {
	//vec3 normal;
	//vec3 eye;
	//vec3 lightDir;
	vec2 tex_coord;
	//vec4 tangentDebug;
	vec3 halfVec_tangent;
	vec3 lightVec_tangent;
} DataOut;

void main () {
	DataOut.tex_coord = texCoord.st;
	//DataOut.tangentDebug = tangent;


	// Building the matrix Eye Space -> Tangent Space
	vec3 n = normalize(m_normal * normal.xyz);
	vec3 t = normalize(m_normal * tangent.xyz);
	vec3 b = tangent.w * cross (n, t);

	vec3 eye_position = vec3(m_viewModel * position);
	vec3 lightDir = normalize(l_pos.xyz - eye_position);

	// transform light and half angle vectors by tangent basis
	vec3 v;
	v.x = dot(lightDir, t);
	v.y = dot(lightDir, b);
	v.z = dot(lightDir, n);
	DataOut.lightVec_tangent = normalize (v);

	vec3 halfVector = normalize(lightDir - eye_position);
	v.x = dot(halfVector, t);
	v.y = dot(halfVector, b);
	v.z = dot(halfVector, n);
	DataOut.halfVec_tangent = normalize (v);

	//DataOut.lightDir = lightDir;
	//DataOut.eye = eye_position;

	gl_Position = m_pvm * position;
	//gl_Position = m_pvm * tangent;	
}