/*
 * Utils.h
 *
 *  Created on: 16/01/2014
 *      Author: Iván
 */

#ifndef UTILS_H_
#define UTILS_H_

#include <GL/glew.h>

typedef enum { false, true } bool;

const char* loadShader(const char* filename);
GLuint compileShader(const char* filename, GLuint shaderType);
bool shaderCompiled(GLuint shaderId);
bool loadBMP(const char* filename, unsigned char **pdata, unsigned int *width, unsigned int *height);
void loadTeapot(const char* filename, GLuint vpLoc, GLuint vnLoc);

#endif /* UTILS_H_ */
