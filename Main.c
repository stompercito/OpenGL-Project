
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string.h>
#include "Transforms.h"
#include "Utils.h"
#include <stdio.h>
#include <math.h>
#include "Vec4.h"
#include "Vec3.h"
#include "Sphere.h"
#include <stdlib.h>



static Mat4   modelMatrix, projectionMatrix, viewMatrix, distanceMatrix, distanceColisionMatrix;
static GLuint programId1, vertexPositionLoc,vertexColorLoc, vertexNormalLoc, vertexTexcoordLoc,  modelMatrixLoc,  projectionMatrixLoc,  viewMatrixLoc, distanceMatrixLoc; //dibujado
static GLuint programId2, vertexPositionLoc2, modelMatrixLoc2,  projectionMatrixLoc2,  viewMatrixLoc2, positionLoc2, radiusLoc2,distanceMatrixLoc2; //colision

GLuint cubeVA, cubeIndBufferId; 			//dibujado de cubo central
GLuint cubeVA2, cubeIndBufferId2;			//colision del cubo central

GLuint labVA,  labIndBufferId; 			//dibujado del laberinto
GLuint labVA2, labIndBufferId2;			//colision del laberinto

GLuint lightCubeVA, lightCubeIndBufferId; 			//dibujado de cubo de luz

GLuint glass; //textura 1 (grass)


static GLuint whichDrawLoc;
static int whichDraw = 0;
static const float clearColor[] = {0.0,0,0,1};


//Iluminacion
static GLuint lightsBufferId;

static GLuint ambientLightLoc, materialALoc, materialDLoc;
static GLuint materialSLoc, cameraPositionLoc;

static vec3 ambientLight  = {0, 0, 0};

static vec3 materialA     = {0.8, 0.8, 0.8};
static vec3 materialD     = {0.3, 0.3, 0.3};
static vec3 materialS     = {0.6, 0.6, 0.6};
							//color	 //algo		//posicion		//algo		direccion	algo
static float lights[]   = { 1, 1, 1,  0,   		0, 0, 0,  		1,	 		0, 0, 1,   0,		// Luz de la esfera
							1, 1, 0,  0,   		0, 0, 0,  		128,	 	0, 0, -1,  0,
};

#define RESET 0xFFFFFFFF

typedef enum { IDLE, LEFT, RIGHT, FRONT, BACK, UP, DOWN } MOTION_TYPE;
vec3 position;

//Informacion de la cámara
static float rotationSpeed = 1;
static MOTION_TYPE camaraDirection = IDLE;

// variables de rotaciones y de la cara visible
Vec4 up = {1,0,0,0};
Vec4 right ={0,1,0,0};
int camaraFace = 0;
MOTION_TYPE camaraLad = DOWN;
static float const MIN_CAMARA_DISTANCE = 50;
static float const MAX_CAMARA_DISTANCE = 150;
static float const CAMARA_CHANGE = 1;
static float camaraDistance = 100;

static const float CUBE_WIDTH = 40;
static const float CUBE_HEIGHT = 40;
static const float CUBE_DEPTH = 40;

//Valores del raton
static int clicX, clicY;
bool leftClick = false;
bool rightClick = false;
bool midleClick = false;

//Valores del Laberinto

typedef struct{
	int size;
	int**faces[6];
	int*edge[12];
	int vertex[8];

}Laberinto;
Laberinto laberinto;


//Valores del punto final
vec3 finalPoint;
static float 	finalPointCubeAngle,
				finalPointCubeRotationX,
				finalPointCubeRotationY,
				finalPointCubeRotationZ,
				finalPointCubeSize;

static float const 	LIGHTCUBE_SCALECHANGE = 0.01,
					LIGHTCUBE_XCHANGE = 1,
					LIGHTCUBE_YCHANGE = 1,
					LIGHTCUBE_ZCHANGE = 1;


//Informacion interna de la esfera
Sphere sphere;
static const int   SPHERE_COLUMNS = 10;
static const int   SPHERE_ROWS	  = 10;
static const float SPHERE_RED	  = 0.8;
static const float SPHERE_GREEN	  = 0.8;
static const float SPHERE_BLUE	  = 0.8;
static float sphereRadius;

//variables que almacena la posicion y señala el movimiento de la esfera

static MOTION_TYPE sphereVerticalMove = IDLE;
static MOTION_TYPE sphereHorizontalMove = IDLE;
static float sphereSpeed,
			sphereX,sphereY,sphereZ;

//Informacion y funciones del cubo base
static void initShaders() {
	GLuint vShader = compileShader("shaders/phong.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	GLuint fShader = compileShader("shaders/phong.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;
	programId1 = glCreateProgram();
	glAttachShader(programId1, vShader);
	glAttachShader(programId1, fShader);
	glLinkProgram(programId1);

	vertexPositionLoc   = glGetAttribLocation(programId1, "vertexPosition");
	vertexColorLoc 		= glGetAttribLocation(programId1, "vertexColor");
	vertexNormalLoc		= glGetAttribLocation(programId1, "vertexNormal");
	vertexTexcoordLoc   = glGetAttribLocation(programId1, "vertexTexcoord");

	modelMatrixLoc      = glGetUniformLocation(programId1, "modelMatrix");
	viewMatrixLoc       = glGetUniformLocation(programId1, "viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId1, "projMatrix");
	distanceMatrixLoc   = glGetUniformLocation(programId1, "distanceMatrix");

	ambientLightLoc     = glGetUniformLocation(programId1, "ambientLight");
	materialALoc        = glGetUniformLocation(programId1, "materialA");
	materialDLoc        = glGetUniformLocation(programId1, "materialD");
	materialSLoc        = glGetUniformLocation(programId1, "materialS");
	cameraPositionLoc   = glGetUniformLocation(programId1, "cameraPosition");
	whichDrawLoc   		= glGetUniformLocation(programId1, "whichDraw");

	//colisiones
	vShader = compileShader("shaders/colision.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	fShader = compileShader("shaders/colision.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;
	programId2 = glCreateProgram();
	glAttachShader(programId2, vShader);
	glAttachShader(programId2, fShader);
	glLinkProgram(programId2);

	vertexPositionLoc2   = glGetAttribLocation(programId2, "vertexPosition");
	modelMatrixLoc2      = glGetUniformLocation(programId2, "modelMatrix");
	viewMatrixLoc2       = glGetUniformLocation(programId2, "viewMatrix");
	projectionMatrixLoc2 = glGetUniformLocation(programId2, "projMatrix");
	distanceMatrixLoc2   = glGetUniformLocation(programId2, "distanceMatrix");

	positionLoc2			= glGetUniformLocation(programId2, "position");
	radiusLoc2			= glGetUniformLocation(programId2, "radius");
}

static void initCube(){
	float w1 = -CUBE_WIDTH  / 2, w2 = CUBE_WIDTH  / 2;
	float h1 = -CUBE_HEIGHT / 2, h2 = CUBE_HEIGHT / 2;
	float d1 = -CUBE_DEPTH  / 2, d2 = CUBE_DEPTH  / 2;
	float positions[] = {	w2, h2, d1, 	w2, h1, d1, 	w1, h1, d1, 	w1, h2, d1,  // Frente
			             	w2, h2, d2, 	w2, h1, d2, 	w1, h1, d2,		w1, h2, d2,  // Atrás
							w1, h2, d2, 	w1, h1, d2, 	w1, h1, d1, 	w1, h2, d1,  // Izquierda
							w2, h2, d1, 	w2, h1, d1, 	w2, h1, d2, 	w2, h2, d2,  // Derecha
							w1, h1, d1, 	w1, h1, d2, 	w2, h1, d2, 	w2, h1, d1,  // Abajo
							w1, h2, d2, 	w1, h2, d1, 	w2, h2, d1, 	w2, h2, d2   // Arriba
	};
	float colors[] = {  	0,0,1,	0,0,1,	0,0,1, 	0,0,1,
							0,1,0,	0,1,0,	0,1,0,	0,1,0,
							1,0,0,	1,0,0,	1,0,0,	1,0,0,
							1,1,0,	1,1,0,	1,1,0,	1,1,0,
							0,1,1,	0,1,1,	0,1,1,	0,1,1,
							1,0,1,	1,0,1,	1,0,1,	1,0,1
	};
	float wh = (float) CUBE_WIDTH / CUBE_HEIGHT;
	float dh = (float) CUBE_DEPTH / CUBE_HEIGHT;
	float texcoords[] = {   	0, 2,			0, 0,  		2 * wh, 0,    	2 * wh, 2,
					           	2 * wh, 2,  	2 * wh, 0,  0, 0,     		0, 2,
							    0, 2,       	0, 0,  		2 * dh, 0, 		2 * dh, 2,
								2 * dh, 2,  	2 * dh, 0,  0, 0,     		0, 2,
								0, 2,      		0, 0,  		2 * wh, 0,   	2 * wh, 2,
								2 * wh, 2,  	2 * wh, 0,  0, 0,        	0, 2,
	};
	float normals[] = {
							0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1,
							0,0,1,	0,0,1,	0,0,1,	0,0,1,
							-1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
							1,0,0,	1,0,0,	1,0,0,	1,0,0,
							0,-1,0,	0,-1,0,	0,-1,0,	0,-1,0,
							0,1,0,	0,1,0,	0,1,0,	0,1,0
		};

	GLuint indices[4*6+5];
	int pos = 0;
	for(int i = 0; i<6;i++){
		for(int j = 0;j<4;j++){
			indices[pos] = i*4+j;
			pos++;
		}
		if(i<5)indices[pos] = RESET;
		pos++;
	}
	//Bindeo para dibujo
	glUseProgram(programId1);
	glGenVertexArrays(1, &cubeVA);
	glBindVertexArray(cubeVA);
	GLuint buffers[5];
	glGenBuffers(5, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexColorLoc);

	cubeIndBufferId = buffers[2];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexTexcoordLoc, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexTexcoordLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	glPrimitiveRestartIndex(RESET);
	glEnable(GL_PRIMITIVE_RESTART);


	//Bindeo para colisiones
	glUseProgram(programId2);
	glGenVertexArrays(1, &cubeVA2);
	glBindVertexArray(cubeVA2);
	GLuint buffers2[2];
	glGenBuffers(2, buffers2);

	glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc2, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc2);

	cubeIndBufferId2 = buffers2[1];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers2[1]);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);

	glPrimitiveRestartIndex(RESET);
	glEnable(GL_PRIMITIVE_RESTART);

}

static void initLightCube(){
	float width = CUBE_WIDTH/laberinto.size;
	float height = CUBE_HEIGHT/laberinto.size;
	float depth = CUBE_DEPTH/laberinto.size;
	float w1 = -width  / 2, w2 = width  / 2;
	float h1 = -height / 2, h2 = height / 2;
	float d1 = -depth  / 2, d2 = depth  / 2;
		float positions[] = {	w2, h2, d1, 	w2, h1, d1, 	w1, h1, d1, 	w1, h2, d1,  // Frente
				             	w2, h2, d2, 	w2, h1, d2, 	w1, h1, d2,		w1, h2, d2,  // Atrás
								w1, h2, d2, 	w1, h1, d2, 	w1, h1, d1, 	w1, h2, d1,  // Izquierda
								w2, h2, d1, 	w2, h1, d1, 	w2, h1, d2, 	w2, h2, d2,  // Derecha
								w1, h1, d1, 	w1, h1, d2, 	w2, h1, d2, 	w2, h1, d1,  // Abajo
								w1, h2, d2, 	w1, h2, d1, 	w2, h2, d1, 	w2, h2, d2   // Arriba
		};
		float colors[] = {  	1,1,0,	1,1,0,	1,1,0,	1,1,0,
								1,1,0,	1,1,0,	1,1,0,	1,1,0,
								1,1,0,	1,1,0,	1,1,0,	1,1,0,
								1,1,0,	1,1,0,	1,1,0,	1,1,0,
								1,1,0,	1,1,0,	1,1,0,	1,1,0,
								1,1,0,	1,1,0,	1,1,0,	1,1,0
		};
		float normals[] = {
								0,0,1,	0,0,1,	0,0,1,	0,0,1,
								0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1,
								1,0,0,	1,0,0,	1,0,0,	1,0,0,
								-1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
								0,1,0,	0,1,0,	0,1,0,	0,1,0,
								0,-1,0,	0,-1,0,	0,-1,0,	0,-1,0
			};

		GLuint indices[4*6+5];
		int pos = 0;
		for(int i = 0; i<6;i++){
			for(int j = 0;j<4;j++){
				indices[pos] = i*4+j;
				pos++;
			}
			if(i<5)indices[pos] = RESET;
			pos++;
		}
		//Bindeo para dibujo
		glUseProgram(programId1);
		glGenVertexArrays(1, &lightCubeVA);
		glBindVertexArray(lightCubeVA);
		GLuint buffers[4];
		glGenBuffers(4, buffers);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, 0, 0, 0);
		glEnableVertexAttribArray(vertexPositionLoc);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
		glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, 0, 0, 0);
		glEnableVertexAttribArray(vertexColorLoc);

		lightCubeIndBufferId = buffers[2];
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
		glEnableVertexAttribArray(vertexNormalLoc);

		glPrimitiveRestartIndex(RESET);
		glEnable(GL_PRIMITIVE_RESTART);
}

static void drawCube(GLuint VA, GLuint id){
	glBindVertexArray(VA);
	glBindTexture(GL_TEXTURE_2D, glass);
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glDrawElements(GL_TRIANGLE_FAN, 4*6+5, GL_UNSIGNED_INT, 0);
}

void findPoint(int x, int y, int face, vec3 point){
	float desp = CUBE_WIDTH/laberinto.size;
	vec3 right[] = 	{{1,0,0},{1,0,0}, {1,0,0}, {1,0,0}, {0,0,1}, {0,0,-1}};
	vec3 up[]	=	{{0,1,0},{0,0,-1},{0,-1,0},{0,0,1}, {0,1,0}, {0,1,0}};
	point[0] = up[face][0]*((CUBE_DEPTH/2+desp/2)-y*desp)-right[face][0]*((CUBE_DEPTH/2+desp/2)-x*desp);
	point[1] = up[face][1]*((CUBE_DEPTH/2+desp/2)-y*desp)-right[face][1]*((CUBE_DEPTH/2+desp/2)-x*desp);
	point[2] = up[face][2]*((CUBE_DEPTH/2+desp/2)-y*desp)-right[face][2]*((CUBE_DEPTH/2+desp/2)-x*desp);
	point[0] = (face == 5)?CUBE_DEPTH/2+desp/2:(face == 4)?-(CUBE_DEPTH/2+desp/2):point[0];
	point[1] = (face == 1)?CUBE_DEPTH/2+desp/2:(face == 3)?-(CUBE_DEPTH/2+desp/2):point[1];
	point[2] = (face == 0)?CUBE_DEPTH/2+desp/2:(face == 2)?-(CUBE_DEPTH/2+desp/2):point[2];
}

static void initLaberinto(){
	//Apertura del archivo
	FILE *archivo;
	archivo = fopen("laberinto.txt","r");
	if(archivo == NULL){
		printf("No se encontro el archivo, procedo con la muertacion");
		exit(-1);
	}
	//Inicializacion del laberinto
	Laberinto l;
	fscanf(archivo, "%d",&l.size);
	//Solicita el espacio de memoria para cada cara, arista
	for(int face = 0; face< 6; face++){
		l.faces[face] = (int**)malloc(sizeof(int*)*l.size);
		for(int i = 0; i< l.size; i++)
			l.faces[face][i] = (int*)malloc(sizeof(int)*l.size);

	}
	for(int i = 0; i< 12; i++)l.edge[i] = malloc(sizeof(int)*l.size);


	//Se almacena en cada cara
	for(int cara = 0; cara < 6; cara++)
		for(int i = 0; i<l.size;i++)
			for(int j = 0; j<l.size;j++)
				fscanf(archivo, "%d",&l.faces[cara][i][j]);

	//Se almacena cada arista
	for(int arista = 0; arista <12; arista++)
		for(int i = 0; i<l.size;i++)
			fscanf(archivo, "%d",&l.edge[arista][i]);

	//Se almacena cada vertice
	for(int vertice = 0; vertice <8; vertice ++)
		fscanf(archivo, "%d",&l.vertex[vertice]);

	int startX, startY,finalFace, finalX, finalY;
	//Se almacena el punto inicial de la esfera
	fscanf(archivo,"%d",&startX);
	fscanf(archivo,"%d",&startY);

	//Se almacena el punto final de la esfera
	fscanf(archivo,"%d",&finalFace);
	fscanf(archivo,"%d",&finalX);
	fscanf(archivo,"%d",&finalY);
	fclose(archivo);

	//Se guardan en las variables globales
	laberinto = l;

	//Se detectan los puntos finales e iniciales
	findPoint(finalX, finalY, finalFace, finalPoint);
	vec3 startPoint;
	findPoint(startX, startY,0,startPoint);
	sphereX = startPoint[0];
	sphereY = startPoint[1];
	sphereZ = startPoint[2];

}

static void initPared(){

	float width = CUBE_WIDTH/laberinto.size;
	float height = CUBE_HEIGHT/laberinto.size;
	float depth = CUBE_DEPTH/laberinto.size;
	float w1 = -width  / 2, w2 = width  / 2;
	float h1 = -height / 2, h2 = height / 2;
	float d1 = -depth  / 2, d2 = depth  / 2;
	float positions[] = {	w2, h2, d1, 	w2, h1, d1, 	w1, h1, d1, 	w1, h2, d1,  // Frente
				        	w2, h2, d2, 	w2, h1, d2, 	w1, h1, d2,		w1, h2, d2,  // Atrás
							w1, h2, d2, 	w1, h1, d2, 	w1, h1, d1, 	w1, h2, d1,  // Izquierda
							w2, h2, d1, 	w2, h1, d1, 	w2, h1, d2, 	w2, h2, d2,  // Derecha
							w1, h1, d1, 	w1, h1, d2, 	w2, h1, d2, 	w2, h1, d1,  // Abajo
							w1, h2, d2, 	w1, h2, d1, 	w2, h2, d1, 	w2, h2, d2   // Arriba
			};

	float colors[] = {  	0,0,1,	0,0,1,	0,0,1, 	0,0,1,
								0,1,0,	0,1,0,	0,1,0,	0,1,0,
								1,0,0,	1,0,0,	1,0,0,	1,0,0,
								1,1,0,	1,1,0,	1,1,0,	1,1,0,
								0,1,1,	0,1,1,	0,1,1,	0,1,1,
								1,0,1,	1,0,1,	1,0,1,	1,0,1
		};
	float wh = (float) width / height;
	float dh = (float) depth / height;
	float texcoords[] = {   0, 2,			0, 0,  		2 * wh, 0,    2 * wh, 2,
				           	2 * wh, 2,  	2 * wh, 0,  0, 0,     0, 2,
						    0, 2,       	0, 0,  		2 * dh, 0,  2 * dh, 2,
							2 * dh, 2,  	2 * dh, 0,  0, 0,     0, 2,
							0, 2,      		0, 0,  		2 * wh, 0,   2 * wh, 2,
							2 * wh, 2,  	2 * wh, 0,  0, 0,        0, 2,
	};
	float normals[] = {
							0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1,
							0,0,1,	0,0,1,	0,0,1,	0,0,1,
							-1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
							1,0,0,	1,0,0,	1,0,0,	1,0,0,
							0,-1,0,	0,-1,0,	0,-1,0,	0,-1,0,
							0,1,0,	0,1,0,	0,1,0,	0,1,0
		};

	GLuint indices[4*6+5];
	int pos = 0;
	for(int i = 0; i<6;i++){
		for(int j = 0;j<4;j++){
			indices[pos] = i*4+j;
			pos++;
		}
		if(i<5)indices[pos] = RESET;
		pos++;
	}

	//Bindeo para dibujo
	glUseProgram(programId1);
	glGenVertexArrays(1, &labVA);
	glBindVertexArray(labVA);
	GLuint buffers[5];
	glGenBuffers(5, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexColorLoc);

	labIndBufferId = buffers[2];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexTexcoordLoc, 2, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexTexcoordLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	glPrimitiveRestartIndex(RESET);
	glEnable(GL_PRIMITIVE_RESTART);

	//Bundeo para colision
	glUseProgram(programId2);
	glGenVertexArrays(1, &labVA2);
	glBindVertexArray(labVA2);
	GLuint buffers2[2];
	glGenBuffers(2, buffers2);

	glBindBuffer(GL_ARRAY_BUFFER, buffers2[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc2, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc2);

	labIndBufferId2 = buffers2[1];
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers2[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,GL_STATIC_DRAW);

	glPrimitiveRestartIndex(RESET);
	glEnable(GL_PRIMITIVE_RESTART);
}

static void drawPared(GLuint VA, GLuint id){
	glBindVertexArray(VA);
	glBindTexture(GL_TEXTURE_2D, glass);
	glBindBuffer(GL_ARRAY_BUFFER,id);
	glDrawElements(GL_TRIANGLE_FAN, 4*6+5, GL_UNSIGNED_INT, 0);
}

static void drawLaberinto(GLuint VA, GLuint id){
	glBindVertexArray(VA);
	glBindBuffer(GL_ARRAY_BUFFER,id);

	vec3 despColumna[] = 	{{1,0,0},{1,0,0}, {1,0,0}, {1,0,0}, {0,0,1}, {0,0,-1}};
	vec3 despFila[]	=		{{0,1,0},{0,0,-1},{0,-1,0},{0,0,1}, {0,1,0}, {0,1,0}};
	vec3 caras[]	 = 		{{0,0,1},{0,1,0}, {0,0,-1},{0,-1,0},{-1,0,0},{1,0,0}};

	float despC = CUBE_WIDTH/2;
	float desp = despC/laberinto.size;

	//Dibuja las caras
	for(int face = 0; face <6; face++){
		mIdentity(&modelMatrix);
		translate(&modelMatrix,(despC+desp)*caras[face][0],			(despC+desp)*caras[face][1],		(despC+desp)*caras[face][2]);
		translate(&modelMatrix,(despC-desp)*despColumna[face][0]*-1,(despC-desp)*despColumna[face][1]*-1,(despC-desp)*despColumna[face][2]*-1);
		translate(&modelMatrix,(despC-desp)*despFila[face][0],	(despC-desp)*despFila[face][1],	(despC-desp)*despFila[face][2]);
		pushMatrix(&modelMatrix);
		for(int i = 0; i<laberinto.size; i++){
			popMatrix(&modelMatrix);
			pushMatrix(&modelMatrix);
			for(int j = 0; j<laberinto.size; j++){
				if(laberinto.faces[face][i][j] == 1){
					glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
					drawPared(VA, id);
				}
				translate(&modelMatrix,despColumna[face][0]*desp*2,despColumna[face][1]*desp*2,despColumna[face][2]*desp*2);
			}
			popMatrix(&modelMatrix);
			translate(&modelMatrix,despFila[face][0]*desp*-2,despFila[face][1]*desp*-2,despFila[face][2]*desp*-2);
			pushMatrix(&modelMatrix);
		}

	}


	loadIdentity(&modelMatrix);
	//dibuja las aristas

	vec3 edgePos[] = {{0,1,0},{1,0,0},{0,-1,0},{-1,0,0},{0,1,0},{-1,0,0},{0,-1,0},{1,0,0},{0,1,0},{0,-1,0},{0,1,0},{0,-1,0}};
	vec3 edgeDesp[] ={{1,0,0},{0,-1,0},{-1,0,0},{0,1,0},{-1,0,0},{0,-1,0},{1,0,0},{0,1,0},{0,0,1},{0,0,1},{0,0,1},{0,0,1}};
	for(int arista = 0; arista <12; arista++){
		mIdentity(&modelMatrix);
		if(arista < 4)		translate(&modelMatrix,0,0,(despC+desp));
		else if(arista <8)	translate(&modelMatrix,0,0,-(despC+desp));
		else if(arista<10)	translate(&modelMatrix,-(despC+desp),0,0);
		else				translate(&modelMatrix,(despC+desp),0,0);

		translate(&modelMatrix,edgePos[arista][0]*(despC+desp),edgePos[arista][1]*(despC+desp),edgePos[arista][2]*(despC+desp));
		translate(&modelMatrix,-edgeDesp[arista][0]*(despC-desp),-edgeDesp[arista][1]*(despC-desp),-edgeDesp[arista][2]*(despC-desp));
		for(int i = 0; i<laberinto.size;i++){
			if(laberinto.edge[arista][i]){
				glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
				drawPared(VA, id);
			}
			translate(&modelMatrix,edgeDesp[arista][0]*desp*2,edgeDesp[arista][1]*desp*2,edgeDesp[arista][2]*desp*2);
		}
	}
	loadIdentity(&modelMatrix);

	//Dibujar las esquinas del laberinto
	for(int esquina = 0; esquina < 8; esquina++){
		mIdentity(&modelMatrix);
		if(esquina < 4)		translate(&modelMatrix,0,0,(despC+desp));
		else translate(&modelMatrix,0,0,-(despC+desp));
		translate(&modelMatrix, edgePos[esquina][0]*(despC+desp),  edgePos[esquina][1]*(despC+desp),  edgePos[esquina][2]*(despC+desp));
		translate(&modelMatrix,-edgeDesp[esquina][0]*(despC+desp),-edgeDesp[esquina][1]*(despC+desp),-edgeDesp[esquina][2]*(despC+desp));
		if(laberinto.vertex[esquina]){
			glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
			drawPared(VA, id);
		}
	}
	loadIdentity(&modelMatrix);
}


//Carga la textura.
static GLuint initTextures(const char *filename, GLint min_mag_filt, GLint wrap_mode)
{
	unsigned char *data;
	unsigned int width, height;
	GLuint texture;

		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		loadBMP(filename, &data, &width, &height);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return texture;
}

static void initSphere(){
	//Se asignan variables globales
	sphereRadius = CUBE_WIDTH /laberinto.size*0.8/2;

	//Se crea y bindea la esfera
	vec3 color = {SPHERE_RED,SPHERE_GREEN,SPHERE_BLUE};
	sphere = sphere_create(sphereRadius, SPHERE_COLUMNS, SPHERE_ROWS, color);
	glUseProgram(programId1);
	sphere_bind(sphere, vertexPositionLoc, vertexColorLoc, vertexNormalLoc);

	//Se envia el valor del radio para las colisiones
	glUseProgram(programId2);
	glUniform1f(radiusLoc2,sphereRadius);

	//Se calcula la velocidad de la esfera
	sphereSpeed = CUBE_WIDTH/laberinto.size*0.1;
}

static void initLights() {
	glUseProgram(programId1);
	glUniform3fv(ambientLightLoc,  1, ambientLight);

	glUniform3fv(materialALoc,     1, materialA);
	glUniform3fv(materialDLoc,     1, materialD);
	glUniform3fv(materialSLoc,     1, materialS);

	glUniform3f(cameraPositionLoc, 0, 0, 0);

	lights[16] = finalPoint[0];
	lights[17] = finalPoint[1];
	lights[18] = finalPoint[2];

	glGenBuffers(1, &lightsBufferId);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW);
	GLuint uniformBlockIndex = glGetUniformBlockIndex(programId1, "LightBlock");
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsBufferId);
	glUniformBlockBinding(programId1, uniformBlockIndex, 0);

}

void moveVerticalSphere();
void moveHorizontalSphere();
void returnVerticalSphere();
void returnHorizontalSphere();

void moveAndCollisionFunc(){
	glUseProgram(programId2);

	//Se envia la informacion que no cambia
	//Se envia matriz de proyección
	glUniformMatrix4fv(projectionMatrixLoc2, 1, true, projectionMatrix.values);

	//Se encia la matriz de distancia
	mIdentity(&distanceColisionMatrix);
	translate(&distanceColisionMatrix,	-sphereX,-sphereY,-sphereZ-CUBE_WIDTH*2/laberinto.size);
	glUniformMatrix4fv(distanceMatrixLoc2, 1, true,distanceColisionMatrix.values);

	//se envia matriz de vista
	Mat4 viewSphereColision;
	mIdentity(&viewSphereColision);
	glUniformMatrix4fv(viewMatrixLoc2, 1, true, viewSphereColision.values);


	//Se mueve y evalua verticalmente

	if (sphereVerticalMove != IDLE) {
		moveVerticalSphere();
		//Se debe de enviar la posicion de la esfera
		glUniform3fv(positionLoc2, 1, position);
		//Se dibuja el escenario
		mIdentity(&modelMatrix);
		glUniformMatrix4fv(modelMatrixLoc2, 1, true, modelMatrix.values);
		drawLaberinto(labVA2,labIndBufferId2);
		mIdentity(&modelMatrix);
		glUniformMatrix4fv(modelMatrixLoc2, 1, true, modelMatrix.values);
		drawCube(cubeVA2, cubeIndBufferId2);

		int collision = 0;
		if (abs(sphereX) > (CUBE_WIDTH / 2  + CUBE_WIDTH/laberinto.size/2))
			collision = 1;
		if (abs(sphereY) > (CUBE_HEIGHT / 2 + CUBE_HEIGHT/laberinto.size/2))
			collision = 1;
		if (abs(sphereZ) > (CUBE_DEPTH / 2  + CUBE_DEPTH/laberinto.size/2))
			collision = 1;

		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		float* pixels = (float*) malloc(sizeof(float) * width * height * 3);
		glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, pixels);
		for (int i = 0; i < width * height * 3; i++) {
			if (collision)
				break;
			if (pixels[i] == 1) {
				collision = 1;
				break;
			}
		}
		free(pixels);
		if (collision)returnVerticalSphere();
	}


	if (sphereHorizontalMove != IDLE) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		moveHorizontalSphere();
		//Se debe de enviar la posicion de la esfera
		glUniform3fv(positionLoc2, 1, position);
		//Se dubuja el escenario
		mIdentity(&modelMatrix);
		glUniformMatrix4fv(modelMatrixLoc2, 1, true, modelMatrix.values);
		drawLaberinto(labVA2,labIndBufferId2);
		mIdentity(&modelMatrix);
		glUniformMatrix4fv(modelMatrixLoc2, 1, true, modelMatrix.values);
		drawCube(cubeVA2, cubeIndBufferId2);



		int width = glutGet(GLUT_WINDOW_WIDTH);
		int height = glutGet(GLUT_WINDOW_HEIGHT);
		float* pixels = (float*) malloc(sizeof(float) * width * height * 3);
		glReadPixels(0, 0, width, height, GL_RGB, GL_FLOAT, pixels);

		int collision = 0;
		if (abs(sphereX) > (CUBE_WIDTH / 2  + CUBE_WIDTH/laberinto.size/2))
			collision = 1;
		if (abs(sphereY) > (CUBE_HEIGHT / 2 + CUBE_HEIGHT/laberinto.size/2))
			collision = 1;
		if (abs(sphereZ) > (CUBE_DEPTH / 2  + CUBE_DEPTH/laberinto.size/2))
			collision = 1;
		for (int i = 0; i < width * height * 3; i++) {
			if (collision)
				break;
			if (pixels[i] == 1) {
				collision = 1;
				break;
			}
		}
		free(pixels);
		if (collision)returnHorizontalSphere();
	}

}

//funciones relacionadas con la vista
static void rotateDirection();
void viewFunc();

//Funciones de movimiento de la esfera
void moveVerticalSphere(){

	if(camaraDirection != IDLE)return;
	if(sphereVerticalMove == UP){
		sphereX += sphereSpeed*right.x;
		sphereY += sphereSpeed*right.y;
		sphereZ += sphereSpeed*right.z;
	}
	else if(sphereVerticalMove == DOWN){
		sphereX -= sphereSpeed*right.x;
		sphereY -= sphereSpeed*right.y;
		sphereZ -= sphereSpeed*right.z;
	}
	position[0] = sphereX;
	position[1] = sphereY;
	position[2] = sphereZ;
}

void moveHorizontalSphere(){
	if(camaraDirection != IDLE)return;
	if(sphereHorizontalMove == RIGHT){
		sphereX += sphereSpeed*up.x;
		sphereY += sphereSpeed*up.y;
		sphereZ += sphereSpeed*up.z;
	}
	else if(sphereHorizontalMove == LEFT){
		sphereX -= sphereSpeed*up.x;
		sphereY -= sphereSpeed*up.y;
		sphereZ -= sphereSpeed*up.z;
	}
	position[0] = sphereX;
	position[1] = sphereY;
	position[2] = sphereZ;
}

void returnHorizontalSphere(){
	if(sphereHorizontalMove == LEFT){
		sphereX += sphereSpeed*up.x;
		sphereY += sphereSpeed*up.y;
		sphereZ += sphereSpeed*up.z;
	}
	else if(sphereHorizontalMove == RIGHT){
		sphereX -= sphereSpeed*up.x;
		sphereY -= sphereSpeed*up.y;
		sphereZ -= sphereSpeed*up.z;
	}
	position[0] = sphereX;
	position[1] = sphereY;
	position[2] = sphereZ;
}

void returnVerticalSphere(){
	if(sphereVerticalMove == DOWN){
		sphereX += sphereSpeed*right.x;
		sphereY += sphereSpeed*right.y;
		sphereZ += sphereSpeed*right.z;
	}
	else if(sphereVerticalMove == UP){
		sphereX -= sphereSpeed*right.x;
		sphereY -= sphereSpeed*right.y;
		sphereZ -= sphereSpeed*right.z;
	}
	position[0] = sphereX;
	position[1] = sphereY;
	position[2] = sphereZ;
}

double distanceToFinalPoint(){
	return sqrt(pow(sphereX-finalPoint[0],2)+pow(sphereY-finalPoint[1],2)+pow(sphereZ-finalPoint[2],2));
}

void updateFinalPoint(){
	finalPointCubeAngle = ((finalPointCubeAngle +LIGHTCUBE_SCALECHANGE)<180)?finalPointCubeAngle +LIGHTCUBE_SCALECHANGE:0;
	finalPointCubeRotationX = (int)(finalPointCubeRotationX +LIGHTCUBE_XCHANGE)%180;
	finalPointCubeRotationY = (int)(finalPointCubeRotationY +LIGHTCUBE_YCHANGE)%180;
	finalPointCubeRotationZ = (int)(finalPointCubeRotationZ +LIGHTCUBE_ZCHANGE)%180;
	finalPointCubeSize = cos(finalPointCubeAngle)*0.5;
}

//Funciones generales
static void displayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//colisiones
	moveAndCollisionFunc();
	//Se actualiza la rotacion de la matriz
	viewFunc();

	//Se actualiza la posicion de la luz
	lights[4] = sphereX;
	lights[5] = sphereY;
	lights[6] = sphereZ;
	glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW);
	GLuint uniformBlockIndex = glGetUniformBlockIndex(programId1, "LightBlock");
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsBufferId);
	glUniformBlockBinding(programId1, uniformBlockIndex, 0);

	if(whichDraw != 3){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(programId1);
	//Se envia matriz de proyección
	glUniformMatrix4fv(projectionMatrixLoc, 1, true,projectionMatrix.values);

	//Se envia la matrix de distancia
	glUniformMatrix4fv(distanceMatrixLoc, 1, true,distanceMatrix.values);

	//se envia matriz de vista
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);

	//Se envia matriz de modelo
	mIdentity(&modelMatrix);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);

	//Se dubuja la base del laberinto
	drawCube(cubeVA, cubeIndBufferId);

	//Se dibujan las paredes
	drawLaberinto(labVA,labIndBufferId);

	//Se dibuja la esfera
	mIdentity(&modelMatrix);
	translate(&modelMatrix,sphereX, sphereY, sphereZ);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	sphere_draw(sphere);

	//Se dibuja el cubo del punto final
	mIdentity(&modelMatrix);
	translate(&modelMatrix,finalPoint[0], finalPoint[1], finalPoint[2]);
	rotateX(&modelMatrix,finalPointCubeRotationX);
	rotateY(&modelMatrix,finalPointCubeRotationY);
	rotateZ(&modelMatrix,finalPointCubeRotationZ);
	scale(&modelMatrix,finalPointCubeSize,finalPointCubeSize,finalPointCubeSize);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	drawCube(lightCubeVA,lightCubeIndBufferId);
	}
	glutSwapBuffers();
}

static void reshapeFunc(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    float aspect = (float) w / h;
    setPerspective(&projectionMatrix, 45, aspect, -0.1, -500);
}

static void timerFunc(int id) {
	glutTimerFunc(10, timerFunc, id);
	glutPostRedisplay();
	updateFinalPoint();
	if(distanceToFinalPoint() < sphereRadius*0.7){
		exit(0);
	}
}

static void specialKeyReleasedFunc(int key,int x, int y) {
	switch(key) {
		case 100:
		case 102: sphereHorizontalMove = IDLE; break;
		case 101:
		case 103: sphereVerticalMove = IDLE;break;
	}
}

static void keyReleasedFunc(unsigned char key,int x, int y) {
	switch(key) {
	case 'a':
	case 'A':
	case 'd':
	case 'D': sphereHorizontalMove = IDLE;break;
	case 'w':
	case 'W':
	case 's':
	case 'S': sphereVerticalMove = IDLE;break;

	}
}

static void specialKeyPressedFunc(int key, int x, int y) {

	switch(key) {
		case 100: sphereHorizontalMove = LEFT;  break;
		case 102: sphereHorizontalMove = RIGHT; break;
		case 101: sphereVerticalMove = UP; break;
		case 103: sphereVerticalMove = DOWN;break;
	}
}

static void keyPressedFunc(unsigned char key, int x, int y) {
	if(key == 27)exit(0);
	switch(key) {
	case 'a':
	case 'A': sphereHorizontalMove = LEFT;break;
	case 'd':
	case 'D': sphereHorizontalMove = RIGHT;break;
	case 'w':
	case 'W': sphereVerticalMove = UP;break;
	case 's':
	case 'S': sphereVerticalMove = DOWN;break;
	case 'q':
	case 'Q': whichDraw = fmax(0,whichDraw-1);glUniform1i(whichDrawLoc,whichDraw);break;
	case 'e':
	case 'E': whichDraw = fmin(3,whichDraw+1);glUniform1i(whichDrawLoc,whichDraw);break;

	}
}

static void mousePasiveMotionFunc(int x, int y){
	clicX = x;
	clicY = y;

}
static void mouseMotionFunc(int x, int y){
	if(camaraDirection != IDLE || !leftClick)return;
	float disX = (abs(x - clicX))/(float)glutGet(GLUT_WINDOW_WIDTH);
	float disY = (abs(y - clicY))/(float)glutGet(GLUT_WINDOW_HEIGHT);
	if(disX > disY){
		if(disX > 0.35){
			if(x>clicX) camaraDirection = RIGHT;
			else camaraDirection = LEFT;
		}
	}
	else if(disY > disX){
		if(disY > 0.3){
			if(y > clicY)camaraDirection = DOWN;
			else camaraDirection = UP;
		}
	}
}
static void mouseFunc(int button, int state, int x, int y){
	switch(button){
	case 0:leftClick = (state == GLUT_DOWN)?true:false;break;
	case 1:midleClick = (state == GLUT_UP)?true:false;break;
	case 2:rightClick = (state == GLUT_UP)?true:false;break;
	case 3:
		camaraDistance = fmin(camaraDistance +CAMARA_CHANGE, MAX_CAMARA_DISTANCE);
		mIdentity(&distanceMatrix);
		translate(&distanceMatrix,0,0,-camaraDistance);
	break;
	case 4:
		camaraDistance = fmax(camaraDistance -CAMARA_CHANGE, MIN_CAMARA_DISTANCE);
		mIdentity(&distanceMatrix);
		translate(&distanceMatrix,0,0,-camaraDistance);
		break;
	}
}

void init(){
    initShaders();
    //Init Matrices
    mIdentity(&viewMatrix);
    mIdentity(&distanceMatrix);
    translate(&distanceMatrix, 0, 0, -camaraDistance);
    mIdentity(&distanceColisionMatrix);
    glass = initTextures("textures/Brick.bmp", GL_LINEAR, GL_CLAMP_TO_EDGE);
    initCube();
    initLaberinto();
    initPared();
    initLightCube();
    initSphere();
    initLights();
    glUniform1i(whichDrawLoc,whichDraw);
}


int main(int argc, char **argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("El trepidante desafio de bill la bola que rueda");
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);
    glutTimerFunc(10, timerFunc, 1);
    glutKeyboardFunc(keyPressedFunc);
    glutKeyboardUpFunc(keyReleasedFunc);
    glutSpecialFunc(specialKeyPressedFunc);
    glutSpecialUpFunc(specialKeyReleasedFunc);
    glutMouseFunc(mouseFunc);				//este te dice cuando hubo un cambio en los botones del raton
    glutMotionFunc(mouseMotionFunc);		//detecta la posicion del raton cuando esta el clic presionado
    glutPassiveMotionFunc(mousePasiveMotionFunc); //este cuando no lo esta presionado
    glewInit();
    glEnable(GL_DEPTH_TEST);
    init();
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glutMainLoop();

	return 0;
}


int rotating = 0;
void viewFunc(){
	if(camaraDirection == IDLE || (!leftClick && !rotating))return;

	rotating = (int)(rotating + rotationSpeed)%90;
	switch(camaraDirection){
	case UP:
		rotateX(&viewMatrix,-rotationSpeed*up.x);
		rotateY(&viewMatrix,-rotationSpeed*up.y);
		rotateZ(&viewMatrix,-rotationSpeed*up.z);
		break;
	case DOWN:
		rotateX(&viewMatrix,rotationSpeed*up.x);
		rotateY(&viewMatrix,rotationSpeed*up.y);
		rotateZ(&viewMatrix,rotationSpeed*up.z);
		break;
	default: break;
	case LEFT:

		rotateX(&viewMatrix,-rotationSpeed*right.x);
		rotateY(&viewMatrix,-rotationSpeed*right.y);
		rotateZ(&viewMatrix,-rotationSpeed*right.z);
		break;
	case RIGHT:
		rotateX(&viewMatrix,rotationSpeed*right.x);
		rotateY(&viewMatrix,rotationSpeed*right.y);
		rotateZ(&viewMatrix,rotationSpeed*right.z);
		break;
	}
	if(rotating == 0){
		rotateDirection();
		camaraDirection = IDLE;

	}



}

static void rotateDirection(){
	int c = 0;
	MOTION_TYPE m = IDLE;
	switch(camaraFace){
	case 0:
		switch(camaraLad){
		case DOWN:
			switch(camaraDirection){
			case UP:c =3;m = DOWN;break;
			case DOWN:c =1;m = DOWN;break;
			case RIGHT:c=4;m = DOWN;break;
			case LEFT:c = 5;m = DOWN;break;
			default:break;
			}
			break;
		case UP:
			switch(camaraDirection){
			case UP:c =1;m = UP;break;
			case DOWN:c =3;m = UP;break;
			case RIGHT:c=5;m = UP;break;
			case LEFT:c = 4;m = UP;break;
			default:break;
			}
			break;
		case RIGHT:
			switch(camaraDirection){
			case UP:c =4;m = RIGHT;break;
			case DOWN:c =5;m = RIGHT;break;
			case RIGHT:c=1;m = RIGHT;break;
			case LEFT:c =3;m = RIGHT;break;
			default:break;
			}
			break;
		case LEFT:
			m = LEFT;
			switch(camaraDirection){
			case UP:c =5;break;
			case DOWN:c =4;break;
			case RIGHT:c=3;break;
			case LEFT:c =1;break;
			default:break;
			}
			break;
		default:break;
		}
		break;
		case 1:
			switch(camaraLad){
			case DOWN:
				switch(camaraDirection){
				case UP:c =0;m = DOWN;break;
				case DOWN:c =2;m = DOWN;break;
				case RIGHT:c=4;m = LEFT;break;
				case LEFT:c = 5;m = RIGHT;break;
				default:break;
				}
				break;
			case UP:
				switch(camaraDirection){
				case UP:c =2;m = UP;break;
				case DOWN:c =0;m = UP;break;
				case RIGHT:c=5;m = LEFT;break;
				case LEFT:c = 4;m = RIGHT;break;
				default:break;
				}
				break;
			case RIGHT:
				switch(camaraDirection){
				case UP:c =4;m = DOWN;break;
				case DOWN:c =5;m = UP;break;
				case RIGHT:c=2;m = RIGHT;break;
				case LEFT:c =0;m = RIGHT;break;
				default:break;
				}
				break;
			case LEFT:
				switch(camaraDirection){
				case UP:c =5;m = DOWN;break;
				case DOWN:c =4;m = UP;break;
				case RIGHT:c=0;m = LEFT;break;
				case LEFT:c =2;m = LEFT;break;
				default:break;
				}
				break;
			default:break;
			}
			break;
	case 2:
		switch(camaraLad){
		case DOWN:
			switch(camaraDirection){
			case UP:c =1;m = DOWN;break;
			case DOWN:c =3;m = DOWN;break;
			case RIGHT:c=4;m = UP;break;
			case LEFT:c = 5;m = UP;break;
			default:break;
			}
			break;
		case UP:
			switch(camaraDirection){
			case UP:c =3;m = UP;break;
			case DOWN:c =1;m = UP;break;
			case RIGHT:c=5;m = DOWN;break;
			case LEFT:c = 4;m = DOWN;break;
			default:break;
			}
			break;
		case RIGHT:
			switch(camaraDirection){
			case UP:c =4;m = LEFT;break;
			case DOWN:c =5;m = LEFT;break;
			case RIGHT:c=3;m = RIGHT;break;
			case LEFT:c =1;m = RIGHT;break;
			default:break;
			}
			break;
		case LEFT:
			switch(camaraDirection){
			case UP:c =5;m = RIGHT;break;
			case DOWN:c =4;m = RIGHT;break;
			case RIGHT:c=1;m = LEFT;break;
			case LEFT:c =3;m = LEFT;break;
			default:break;
			}
			break;
		default:break;
		}
		break;
		case 3:
			switch(camaraLad){
			case DOWN:
				switch(camaraDirection){
				case UP:c =2;m = DOWN;break;
				case DOWN:c =0;m = DOWN;break;
				case RIGHT:c=4;m = RIGHT;break;
				case LEFT:c = 5;m = LEFT;break;
				default:break;
				}
				break;
			case UP:
				switch(camaraDirection){
				case UP:c =0;m = UP;break;
				case DOWN:c =2;m = UP;break;
				case RIGHT:c=5;m = RIGHT;break;
				case LEFT:c = 4;m = LEFT;break;
				default:break;
				}
				break;
			case RIGHT:
				switch(camaraDirection){
				case UP:c =4;m = UP;break;
				case DOWN:c =5;m = DOWN;break;
				case RIGHT:c=0;m = RIGHT;break;
				case LEFT:c =2;m = RIGHT;break;
				default:break;
				}
				break;
			case LEFT:
				switch(camaraDirection){
				case UP:c =5;m = UP;break;
				case DOWN:c =4;m = DOWN;break;
				case RIGHT:c=2;m = LEFT;break;
				case LEFT:c =0;m = LEFT;break;
				default:break;
				}
				break;
			default:break;
			}
			break;
	case 4:
		switch (camaraLad) {
		case DOWN:
			switch (camaraDirection) {
			case UP:c = 3;m = LEFT;break;
			case DOWN:c = 1;m = RIGHT;break;
			case RIGHT:c = 2;m = UP;break;
			case LEFT:c = 0;m = DOWN;break;
			default:break;
			}
			break;
		case UP:
			switch (camaraDirection) {
			case UP:c = 1;m = LEFT;break;
			case DOWN:c = 3;m = RIGHT;break;
			case RIGHT:c = 0;m = UP;break;
			case LEFT:c = 2;m = DOWN;break;
			default:break;
			}
			break;
		case RIGHT:
			switch (camaraDirection) {
			case UP:c = 2;m = LEFT;break;
			case DOWN:c = 0;m = RIGHT;break;
			case RIGHT:c = 1;m = UP;break;
			case LEFT:c = 3;m = DOWN;break;
			default:break;
			}
			break;
		case LEFT:
			switch (camaraDirection) {
			case UP:c = 0;m = LEFT;break;
			case DOWN:c = 2;m = RIGHT;break;
			case RIGHT:c = 3;m = UP;break;
			case LEFT:c = 1;m = DOWN;break;
			default:break;
			}
			break;
		default:
			break;
		}
		break;
	case 5:
		switch (camaraLad) {
		case DOWN:
			switch (camaraDirection) {
			case UP:c = 3;m = RIGHT;break;
			case DOWN:c = 1;m = LEFT;break;
			case RIGHT:c = 0;m = DOWN;break;
			case LEFT:c = 2;m = UP;break;
			default:break;
			}
			break;
		case UP:
			switch (camaraDirection) {
			case UP:c = 1;m = RIGHT;break;
			case DOWN:c = 3;m = LEFT;break;
			case RIGHT:c = 2;m = DOWN;break;
			case LEFT:c = 0;m = UP;break;
			default:break;
			}
			break;
		case RIGHT:
			switch (camaraDirection) {
			case UP:c = 0;m = RIGHT;break;
			case DOWN:c = 2;m = LEFT;break;
			case RIGHT:c = 1;m = DOWN;break;
			case LEFT:c = 3;m = UP;break;
			default:break;
			}
			break;
		case LEFT:
			switch (camaraDirection) {
			case UP:c = 2;m = RIGHT;break;
			case DOWN:c = 0;m = LEFT;break;
			case RIGHT:c = 3;m = DOWN;break;
			case LEFT:c = 1;m = UP;break;
			default:break;
			}
			break;
		default:
			break;
		}
		break;
		default:
			exit(0);
	}
	camaraFace = c;
	camaraLad = m;
	int v = -1;
	switch(camaraLad){
	case DOWN: v = 0;
	break;
	case RIGHT: v = 1;
	break;
	case LEFT: v = 2;
	break;
	case UP: v = 3;
	break;
	default:break;
	}

	int x = (c<=3&&(v == 0 || v == 3));
	int neg_x = (v==3);
	int y = ((c !=3 && c!= 1)&&(v==1 || v == 2));
	int neg_y = ((c==4 || c == 5 || c==0) && (v==1))||((c==2) && v == 2);
	int z = ((c==1 || c==3)&&(v == 1 || v == 2))||((c==4 || c== 5)&&(v==0 || v == 3));
	int neg_z = (c== 1 && v==2)|| (c==3&& v == 1)|| (c==4 && v == 3)|| (c==5 && v == 0);
	up.x = x?!neg_x?1:-1:0;
	up.y = y?!neg_y?1:-1:0;
	up.z = z?!neg_z?1:-1:0;

	x = (c<=3&&(v == 1 || v == 2));
	neg_x = ((c<=3)&&(v==2));
	y = ((c !=3 && c!= 1)&&(v==0 || v == 3));
	neg_y = ((c==0 || c == 4 || c == 5)&&(v==3))||(c==2 && v == 0);
	z = ((c==1 || c==3)&&(v == 0 || v == 3))||((c==4 || c== 5)&&(v==1 || v == 2));
	neg_z = (c== 1 && v==0)|| (c==3&& v == 3)|| (c==4 && v == 2)|| (c==5 && v == 1);
	right.x = x?!neg_x?1:-1:0;
	right.y = y?!neg_y?1:-1:0;
	right.z = z?!neg_z?1:-1:0;
	//printf("Cara: %d Sub: %d,\t up<%.1f,%.1f,%.1f>, right<%.1f,%.1f,%.1f>\n",camaraFace,v,up.x,up.y,up.z,right.x,right.y,right.z);
}


