#version 330

in vec2 vertexTexcoordToFS;
uniform sampler2D myTexture;
uniform vec2 textureMove;
out vec4 pixelColor;

void main() {
	vec4 textures =texture(myTexture, (vertexTexcoordToFS+textureMove));
	if(distance(textures.xyz,vec3(0,1,0))<0.5)
		discard;
	else
		pixelColor = textures;
}