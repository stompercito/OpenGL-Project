#version 330



in vec3 vertexPosition;
in vec3 vertexColor;

out vec3 vertexColorToFS;

void main(){
	vertexColorToFS = vertexColor;
	 gl_Position = vec4(vertexPosition,1);
}