/*
 * Sphere.c
 *
 *  Created on: 16 abr. 2020
 *      Author: Edgar
 */


#include <stdio.h>
#include <stdlib.h>
#include "Sphere.h"
#include <limits.h>
#include <math.h>
#define RESET 0xFFFFFFFF

struct strSphere{
	float radius;
	int parallels;
	int meridians;
	int vertexNum;

	float * positions;
	float * colors;
	float * normals;
	GLuint * indixes;

	GLuint sphereVA;
	GLuint sphereBuffers[4];
};



Sphere sphere_create(float radius, int parallels, int meridians, vec3 sphereColor){
	Sphere sphere = (Sphere) malloc(sizeof(struct strSphere));
	sphere->radius = radius;
	sphere->parallels = parallels;
	sphere->meridians = meridians;

	sphere->vertexNum = ((parallels + 1) * meridians) + ( (parallels-1) * meridians);
	sphere->positions = (float *) malloc(sizeof (float ) * (sphere->vertexNum * 3) );
	sphere->colors = (float *) malloc(sizeof (float ) * (sphere->vertexNum * 3));
	sphere->normals = (float *) malloc(sizeof (float ) * (sphere->vertexNum * 3));
	sphere->indixes = (GLuint *) malloc(sizeof (GLuint ) * (sphere->vertexNum + parallels));

	float angTheta = 0, angPhi = 0;

	float incTheta = 2 * M_PI / (meridians - 1), incPhi = M_PI / (parallels - 1);

	int count = 0 , indexCount = 0, indexValue = 0;

	float red, green, blue;
	//Puntos, colores y normales.
	for(int i = 0; i < parallels -1; i++){
		angTheta = 0;
		red = sphereColor[0];
		green = sphereColor[1];
		blue = sphereColor[2];
		for(int j = 0; j < meridians; j++){
			//Calculo de i y de i + 1

			sphere->positions[count] =  radius * sin(angPhi) * cos(angTheta);
			sphere->colors[count] = red;
			sphere->normals[count++] = -sin(angPhi) * cos(angTheta);

			sphere->positions[count] = radius * sin(angPhi) * sin(angTheta);
			sphere->colors[count] = green;
			sphere->normals[count++] = -sin(angPhi) * sin(angTheta);

			sphere->positions[count] = radius * cos(angPhi);
			sphere->colors[count] = blue;
			sphere->normals[count++] = -cos(angPhi);

			sphere->indixes[indexCount ++] = indexValue++;

			sphere->positions[count] = radius * sin(angPhi + incPhi) * cos(angTheta);
			sphere->colors[count] = red;
			sphere->normals[count++] = -sin(angPhi + incPhi) * cos(angTheta);

			sphere->positions[count] = radius * sin(angPhi+ incPhi) * sin(angTheta);
			sphere->colors[count] = green;
			sphere->normals[count++] = -sin(angPhi+ incPhi) * sin(angTheta);

			sphere->positions[count] = radius * cos(angPhi+ incPhi);
			sphere->colors[count] = blue;
			sphere->normals[count++] = -cos(angPhi+ incPhi);

			sphere->indixes[indexCount ++] = indexValue++;

			angTheta += incTheta;
		}
		sphere->indixes[indexCount ++] = RESET;
		angPhi += incPhi;
	}

	return sphere;
}

void sphere_bind(Sphere sphere, GLuint vertexPosLoc, GLuint vertexColorLoc, GLuint vertexNormalLoc){
	glGenVertexArrays(1, &sphere->sphereVA);
	glBindVertexArray(sphere->sphereVA);

	glGenBuffers(4, sphere->sphereBuffers);

	//Positions
	glBindBuffer(GL_ARRAY_BUFFER, sphere->sphereBuffers[0]);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * sphere->vertexNum * 3), sphere->positions , GL_STATIC_DRAW);
    glVertexAttribPointer(vertexPosLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexPosLoc);

	//Colors
	glBindBuffer(GL_ARRAY_BUFFER, sphere->sphereBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float) * sphere->vertexNum * 3), sphere->colors , GL_STATIC_DRAW);
    glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexColorLoc);

	//Normals
	glBindBuffer(GL_ARRAY_BUFFER, sphere->sphereBuffers[2]);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(float) *sphere->vertexNum * 3), sphere->normals, GL_STATIC_DRAW);
    glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(vertexNormalLoc);

    //Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->sphereBuffers[3]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (sizeof(GLuint) * (sphere->vertexNum + sphere->parallels)), sphere->indixes, GL_STATIC_DRAW);
	glPrimitiveRestartIndex(RESET);
	glEnable(GL_PRIMITIVE_RESTART);
}



void sphere_draw(Sphere sphere){
	glBindVertexArray(sphere->sphereVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere->sphereBuffers[3]);
	glDrawElements(GL_TRIANGLE_STRIP, (sphere->vertexNum + sphere->parallels) ,GL_UNSIGNED_INT,0);
}



void print_sphere_positions(Sphere sphere){
	for(int i=0; i< sphere->vertexNum; i++){
		printf("i: %d\t x: %f y: %f z: %f\n",i, sphere->positions[3*i], sphere->positions[3*i+1], sphere->positions[3*i+2]);
	}
}


void print_sphere_colors(Sphere sphere){
	for(int i=0; i< sphere->vertexNum; i++){
		printf("i: %d\t x: %f y: %f z: %f\n",i, sphere->colors[3*i], sphere->colors[3*i+1], sphere->colors[3*i+2]);
	}
}


