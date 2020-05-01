#version 330

in vec3 vertexPosition;
in vec3 vertexColor;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;

out vec3 vertexColorToFS;

void main() {
	vec4 worldPosition = modelMatrix * vec4(vertexPosition,1);
    gl_Position = projMatrix * viewMatrix * worldPosition;
	vertexColorToFS = vertexColor;
}