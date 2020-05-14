#version 330

in vec3 worldVertexPosition;
in vec3 worldVertexNormal;
in vec2 vertexTexcoordToFS;
in vec3 vertexColorToFS;

out vec4 pixelColor;
uniform sampler2D myTexture;

void main() {
	//pixelColor = vec4(vertexColorToFS,1);
	 pixelColor = vec4(vertexColorToFS,1)*0.5 + texture(myTexture, vertexTexcoordToFS)*0.5;
	//pixelColor = texture(myTexture, vertexTexcoordToFS);
}