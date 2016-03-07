#version 330

uniform mat4 m_pvm;
uniform mat4 m_viewModel;
uniform mat3 m_normal;

in vec4 position;
in vec4 normal;    //por causa do gerador de geometria
in vec4 texCoord;

out Data {
	vec2 texCoord;
	vec2 sphereCoord;
} DataOut;

void main() {
	gl_Position = m_pvm * position; /* Vertex Position to Clip Space */
	DataOut.texCoord = texCoord.st;

	vec3 e = normalize(vec3(m_viewModel * position) );
	vec3 n = normalize(m_normal * normal.xyz ); //
	vec3 r = reflect(e, n); //reflection vector in eye coord

	float m = 2.0 * sqrt(r.x * r.x + r.y * r.y + (r.z + 1.0) *(r.z + 1.0));
	DataOut.sphereCoord.s = r.x/m + 0.5;
	DataOut.sphereCoord.t = r.y/m + 0.5;
}
