/*
 * Utils.c
 *
 *  Created on: 16/01/2014
 *      Author: Iván
 */

#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>

void loadTeapot(const char* filename, GLuint vpLoc, GLuint vnLoc) {
	FILE* teapotFile = fopen(filename, "r");

	int vertexCount;
	fscanf(teapotFile, "v %d", &vertexCount);
	float* vertexPositions = (float*) malloc(vertexCount * 3 * sizeof(float));
	float* vPointer = vertexPositions;
	int v;
	for(v = 0; v < vertexCount; v ++) {
		fscanf(teapotFile, "%f %f %f", vPointer, vPointer + 1, vPointer + 2);
		vPointer += 3;
	}

	int normalCount;  // se espera igual que vertexCount
	fscanf(teapotFile, "\nn %d", &normalCount);
	float* vertexNormals = (float*) malloc(normalCount * 3 * sizeof(float));
	float* nPointer = vertexNormals;
	for(v = 0; v < normalCount; v ++) {
		fscanf(teapotFile, "%f %f %f", nPointer, nPointer + 1, nPointer + 2);
		nPointer += 3;
	}

	int triangleCount;
	fscanf(teapotFile, "\ni %d", &triangleCount);
	GLuint* indexes = (GLuint*) malloc(3 * triangleCount * sizeof(GLuint));
	GLuint* iPointer = indexes;
	int t;
	for(t = 0; t < triangleCount; t ++) {
		fscanf(teapotFile, "%d %d %d", iPointer, iPointer + 1, iPointer + 2);
		iPointer += 3;
	}

	GLuint va;
	glGenVertexArrays(1, &va);
	glBindVertexArray(va);
	GLuint buffers[3];
	glGenBuffers(3, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * 3 * sizeof(float), vertexPositions, GL_STATIC_DRAW);
	glVertexAttribPointer(vpLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vpLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, normalCount * 3 * sizeof(float), vertexNormals, GL_STATIC_DRAW);
	glVertexAttribPointer(vnLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vnLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * triangleCount * sizeof(GLuint), indexes, GL_STATIC_DRAW);

	free(vertexPositions);
	free(vertexNormals);
	free(indexes);
	fclose(teapotFile);
}

const char* loadShader(const char* filename) {
	FILE* shaderFile = fopen(filename, "r");
	const int BUFFER_SIZE = 256;
	char buffer[BUFFER_SIZE];
//	Count number of characters in source file
	int charCount = 0;
	int i = 0;
	while(!feof(shaderFile)) {
		if(fgets(buffer, BUFFER_SIZE, shaderFile) == NULL) continue;
		i = 0;
		while(buffer[i ++] != '\0') charCount ++;
	}
	charCount ++; // consider end-of-string character

//	Copy characters to new char array
	rewind(shaderFile);
	char* shaderData = (char*) malloc(sizeof(char) * charCount);
	int charIndex = 0;
	while(!feof(shaderFile)) {
		if(fgets(buffer, BUFFER_SIZE, shaderFile) == NULL) continue;
		i = 0;
		while(buffer[i] != '\0') shaderData[charIndex ++] = buffer[i ++];
	}
	shaderData[charIndex] = '\0';

	fclose(shaderFile);
	return shaderData;
}

GLuint compileShader(const char* filename, GLuint shaderType) {
//	char const* source = "void main() { ... ";
	char const* source = loadShader(filename);
	GLuint shaderId = glCreateShader(shaderType);
	glShaderSource(shaderId, 1, &source, NULL);
	glCompileShader(shaderId);
	return shaderId;
}

bool shaderCompiled(GLuint shaderId) {
	GLint params;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &params);
	if(params == 1) return true;

	GLint maxLength = 0;
	glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);
	GLchar* errors = (GLchar*) malloc(sizeof(int) * maxLength);
	glGetShaderInfoLog(shaderId, maxLength, &maxLength, errors);
	puts(errors);
	return false;
}

bool loadBMP(const char* filename, unsigned char **pdata, unsigned int *width, unsigned int *height) {
    unsigned char header[54];
    FILE * file = fopen(filename, "rb");
    fread(header, 1, 54, file);
    if(header[0] != 'B' || header[1] != 'M' ) {
        printf("Not a correct BMP file\n");
        return false;
    }
    unsigned char *bytePointerW      = &(header[0x12]);
    unsigned int  *fourBytesPointerW = (unsigned int*) bytePointerW;
    *width = *fourBytesPointerW;
    unsigned char *bytePointerH      = &(header[0x16]);
    unsigned int  *fourBytesPointerH = (unsigned int*) bytePointerH;
    *height = *fourBytesPointerH;
    *pdata = (unsigned char*) malloc(sizeof(unsigned char) * (*width) * (*height) * 3);
    fread(*pdata, 1, (*width) * (*height) * 3, file);
    fclose(file);
    return true;
}
