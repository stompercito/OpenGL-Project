/*
 * Vec4.c
 *
 *  Created on: 21/04/2020
 *      Author: hpiza
 */

#include "Vec4.h"
#include <math.h>

float vec4_magnitude(Vec4 *v) {
	return sqrt(v->x * v->x + v->y * v->y + v->z * v->z + v->w * v->w);
}

void vec4_normalize(Vec4 *v) {
	float m = vec4_magnitude(v);
	v->x /= m;
	v->y /= m;
	v->z /= m;
	v->w /= m;
}

void multiply(Mat4 m, Vec4 v, Vec4 *res) {
	res->x = m.at[0][0] * v.x + m.at[0][1] * v.y + m.at[0][2] * v.z + m.at[0][3] * v.w;
	res->y = m.at[1][0] * v.x + m.at[1][1] * v.y + m.at[1][2] * v.z + m.at[1][3] * v.w;
	res->z = m.at[2][0] * v.x + m.at[2][1] * v.y + m.at[2][2] * v.z + m.at[2][3] * v.w;
	res->w = m.at[3][0] * v.x + m.at[3][1] * v.y + m.at[3][2] * v.z + m.at[3][3] * v.w;
}

