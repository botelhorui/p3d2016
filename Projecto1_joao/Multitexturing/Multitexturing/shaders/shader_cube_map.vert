#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat4 m_view;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

out Data {
	//vec3 normal;
	//vec3 eye;
	//vec3 lightDir;
	vec2 tex_coord;
	//vec4 tangentDebug;
	vec3 halfVec_tangent;
	vec3 lightVec_tangent;
	vec3 reflected;
} DataOut;

void main() {
	gl_Position = m_pvm * position; /* Vertex Position to Clip Space */
	DataOut.tex_coord = texCoord.st;

	vec3 e = normalize( vec3(m_viewModel * position) );
	vec3 n = normalize( m_normal * normal.xyz ); //

	vec3 reflected = reflect(e, n); //reflection vector in eye coord
	DataOut.reflected = vec3 (inverse (m_view) * vec4 (reflected, 0.0)); //reflection vector in world coord
}
