#version 330

in vec3 vertexPosition;
in vec2 vertexTexcoord;

out vec2 vertexTexcoordToFS;

void main(){
	vertexTexcoordToFS = vertexTexcoord;
	 gl_Position = vec4(vertexPosition,1);
}