/*
 * Vec3.c
 *
 *  Created on: 21/04/2020
 *      Author: hpiza
 */

#include "Vec3.h"
#include <math.h>

float vec3_magnitude(Vec3 *v) {
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

void vec3_normalize(Vec3 *v) {
	float m = vec3_magnitude(v);
	v->x /= m;
	v->y /= m;
	v->z /= m;
}
