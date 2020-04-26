#version 330

uniform vec3 materialA;
in vec3 vertexColorToFS;
out vec4 pixelColor;

void main() {
	pixelColor = vec4(vertexColorToFS, 1);
}
