/*
 * Vec3.h
 *
 *  Created on: 21/04/2020
 *      Author: hpiza
 */

#ifndef VEC3_H_
#define VEC3_H_

typedef struct {
	float x, y, z;
} Vec3;

float vec3_magnitude(Vec3*);

void vec3_normalize(Vec3*);

#endif /* VEC3_H_ */
