/*
 * Transforms.h
 *
 *  Created on: 07/09/2016
 *      Author: Cursos
 */

#ifndef TRANSFORMS_H_
#define TRANSFORMS_H_

#include "Mat4.h"

void translate(Mat4*, float, float, float);
void rotateX  (Mat4*, float);
void rotateY  (Mat4*, float);
void rotateZ  (Mat4*, float);
void scale    (Mat4*, float, float, float);
void setOrtho (Mat4*, float left, float right, float bottom, float top, float near, float far);
void setPerspective(Mat4* matrix, float fovy,  float aspect, float nearZ, float farZ);
void inverse(Mat4 s, Mat4* t);

void loadIdentity(Mat4*);
void pushMatrix(Mat4*);
int  popMatrix(Mat4*);

#endif /* TRANSFORMS_H_ */
