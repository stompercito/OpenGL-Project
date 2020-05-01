#version 330


out vec4 pixelColor;

in vec4 worldPosition;
uniform vec3 position;

void main() {
	if(distance(worldPosition.xyz,position)<0.2)
		pixelColor = vec4(1,1,1,1);
	else
		discard;
}