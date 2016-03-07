#version 330

in vec3 position;
out vec3 texCoords;

uniform mat4 m_projection;
uniform mat4 m_view;

out vec3 eyeDirection;

void main(){
/**/
    mat4 inverseProjection = inverse(m_projection);
    mat3 inverseModelview = transpose(mat3(m_view));
    vec3 unprojected = (inverseProjection * vec4(position, 1.0)).xyz;
    eyeDirection = inverseModelview * unprojected;

    gl_Position = vec4(position, 1.0);
/** /
    //gl_Position = m_projection * vec4(position, 1.0);  
    //gl_Position = m_view * vec4(position, 1.0);  	
	//gl_Position = vec4(position, 1.0);

    gl_Position = m_projection * m_view * vec4(position, 1.0);  
    texCoords = position;
/**/
}