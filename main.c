/* Iluminacion.c
 *
 *  Created on: 18/10/2016
 *      Author: hpiza
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Transforms.h"
#include "Utils.h"
#include <stdio.h>
#include "Sphere.h"

static Sphere sphere;
static Mat4   modelMatrix, projectionMatrix, viewMatrix;

static GLuint programId, vertexPositionLoc, vertexColorLoc, vertexNormalLoc, modelMatrixLoc, projectionMatrixLoc, viewMatrixLoc, cameraLoc;

static GLuint ambientLightLoc, diffuseLightLoc, lightPositionLoc, materialALoc, materialDLoc, materialSLoc, exponentLoc, cameraLoc;

static int motionType = 0;
static float observerX = 0, observerZ = 0;
static int N = 20;

static float ambientLight[]  = {0.3, 0.3,.3};
static float materialA[]     = {0.5, 0.5, 0.5};

//Fuente de luz
static float diffuseLight[]  = {1.0, 1.0,  1.0};
static float lightPosition[] = {0.0, 0.0, -1.0};
static float materialD[]     = {0.5, 0.5, 0.5};
static float materialS[]     = {0.5, 0.5, 0.5};;
static float exponent = 16;

static void initShaders() {
	GLuint vShader = compileShader("shaders/gouraud.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;

	GLuint fShader = compileShader("shaders/color.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;

	programId = glCreateProgram();
	glAttachShader(programId, vShader);
	glAttachShader(programId, fShader);
	glLinkProgram(programId);

	vertexPositionLoc   = glGetAttribLocation(programId, "vertexPosition");
	vertexColorLoc   = glGetAttribLocation(programId, "vertexColor");
	vertexNormalLoc     = glGetAttribLocation(programId, "vertexNormal");

	modelMatrixLoc      = glGetUniformLocation(programId, "modelMatrix");
	viewMatrixLoc       = glGetUniformLocation(programId, "viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId, "projMatrix");
	glUseProgram(programId);
//	Obtenci�n de los id de las variables uniformes relacionadas con la iluminaci�n

	ambientLightLoc     = glGetUniformLocation(programId, "ambientLight");
	diffuseLightLoc     = glGetUniformLocation(programId, "diffuseLight");
	lightPositionLoc    = glGetUniformLocation(programId, "lightPosition");
	materialALoc        = glGetUniformLocation(programId, "materialA");
	materialDLoc        = glGetUniformLocation(programId, "materialD");
	materialSLoc        = glGetUniformLocation(programId, "materialS");

	cameraLoc        = glGetUniformLocation(programId, "camera");
	materialSLoc        = glGetUniformLocation(programId, "materialS");
	exponentLoc        = glGetUniformLocation(programId, "exponent");
	glUniform3fv(ambientLightLoc,  1, ambientLight);
	glUniform3fv(diffuseLightLoc,  1, diffuseLight);
	glUniform3fv(lightPositionLoc, 1, lightPosition);
	glUniform3fv(materialALoc,     1, materialA);
	glUniform3fv(materialDLoc,     1, materialD);
	glUniform3fv(materialSLoc,     1, materialS);
	glUniform1f(exponentLoc, exponent);

}


static void moveForward() {
	observerZ -= (N / 80.0);
}


static void moveBack() {
	observerZ += (N / 80.0);
}

static void turnLeft() {
	observerX -= (N / 80.0);
}

static void turnRight() {
	observerX += (N / 80.0);
}


static float rotateAngle = 0;

static void displayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programId);


	switch(motionType) {
		case 1: moveForward(); break;
		case 2: moveBack();    break;
		case 3: turnRight();   break;
		case 4: turnLeft();
	}

	glUniform3f(cameraLoc, observerX, 0, observerZ);


	mIdentity(&viewMatrix);
	translate(&viewMatrix, -observerX, 0, -observerZ);
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);

	mIdentity(&modelMatrix);
	translate(&modelMatrix, 0, 0, -5);
	rotateY(&modelMatrix, rotateAngle);
	rotateX(&modelMatrix, rotateAngle);
	rotateAngle += .3;

	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);

	sphere_draw(sphere);

	glutSwapBuffers();

}

static void reshapeFunc(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    float aspect = (float) w / h;
    setPerspective(&projectionMatrix, 45, aspect, -0.1, -500);
	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
}

static void timerFunc(int id) {
	glutTimerFunc(10, timerFunc, id);
	glutPostRedisplay();
}

static void keyReleasedFunc(int key,int x, int y) {
	motionType = 0;
}

static void specialKeyPressedFunc(int key, int x, int y) {
	switch(key) {
		case 101: motionType = 1; break;
		case 103: motionType = 2; break;
		case 102: motionType = 3; break;
		case 100: motionType = 4;
	}
}


static void keyPressedFunc(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }
 }

int main(int argc, char **argv) {
	setbuf(stdout, NULL);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("Esfera");
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);
    glutTimerFunc(10, timerFunc, 1);
    glutKeyboardFunc(keyPressedFunc);


    glutSpecialFunc(specialKeyPressedFunc);
    glutSpecialUpFunc(keyReleasedFunc);


    glewInit();
    glEnable(GL_DEPTH_TEST);
    initShaders();
	vec3 myColorcito = {0.8, 0.3, 0.8};
	sphere = sphere_create(1.5, 40, 40, myColorcito);

	sphere_bind(sphere,vertexPositionLoc, vertexColorLoc, vertexNormalLoc);
    glClearColor(0, 0, 0, 1.0);
    glutMainLoop();
	return 0;
}
