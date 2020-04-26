/*
 * Sphere.h
 *
 *  Created on: 16 abr. 2020
 *      Author: Edgar
 */

#ifndef SPHERE_H_
#define SPHERE_H_
#include <GL/glew.h>
#include <GL/freeglut.h>

typedef struct strSphere* Sphere;

typedef float vec3[3];

Sphere sphere_create(float, int, int, vec3);

void sphere_bind(Sphere, GLuint, GLuint, GLuint);

void sphere_draw(Sphere);

void print_sphere_positions(Sphere);

void print_sphere_colors(Sphere);
#endif /* SPHERE_H_ */
