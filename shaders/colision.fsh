#version 330


out vec4 pixelColor;

in vec4 worldPosition;
uniform vec3 position;
uniform float radius;

void main() {
	if(distance(worldPosition.xyz,position)<radius)
		pixelColor = vec4(1,1,1,1);
	else
		discard;
}