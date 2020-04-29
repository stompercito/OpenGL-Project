#version 330

in vec3 vertexPosition;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec4 worldPosition;
void main() {
	worldPosition = modelMatrix * vec4(vertexPosition,1);
    gl_Position = projMatrix * viewMatrix * worldPosition;

}