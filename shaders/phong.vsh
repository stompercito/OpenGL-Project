#version 330

in vec3 vertexPosition;
in vec3 vertexColor;
in vec3 vertexNormal;
in vec2 vertexTexcoord;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 distanceMatrix;

out vec3 vertexColorToFS;
out vec2 vertexTexcoordToFS;
out vec3 worldVertexPosition;
out vec3 worldVertexNormal;

void main() {
	vec4 worldPosition = modelMatrix * vec4(vertexPosition,1);
    gl_Position = projMatrix * distanceMatrix*viewMatrix * worldPosition;
    worldVertexPosition = worldPosition.xyz;
    
    mat4 G = transpose(inverse(modelMatrix));
    worldVertexNormal = (G * vec4(vertexNormal, 0)).xyz;
    
	vertexColorToFS = vertexColor;
	vertexTexcoordToFS = vertexTexcoord;
}