#version 330

in vec3 position;

uniform mat4 m_projection;
uniform mat4 m_view;

out vec3 eyeDirection;
out vec3 texCoords;

void main(){
    mat4 inverseProjection = inverse(m_projection);
    mat3 inverseModelview = transpose(mat3(m_view));
    vec3 unprojected = (inverseProjection * vec4(position, 1.0)).xyz;
    eyeDirection = inverseModelview * unprojected;

    gl_Position = vec4(position, 1.0);
}