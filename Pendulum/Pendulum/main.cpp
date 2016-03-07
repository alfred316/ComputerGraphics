//Alfred Shaker
//Pendulum project
//computer graphics 

#include <iostream>
#include <GL\glew.h>
#include "gl\freeglut.h"
#include <stdlib.h>
#include <vector>
#include <math.h>

//texture stuff
#define eps 0.001
#define tMax 2
#define wh 6.0

// Define a helpful macro for handling offsets into buffer objects
#define BUFFER_OFFSET( offset )   ((GLvoid*) (offset))

static GLuint texName[tMax];
int ImageWidth[tMax];
int ImageHeight[tMax];
int intensity[tMax];
GLfloat pdist[tMax] = { 0.0, 5 };
GLubyte * Image[tMax];

//GLuint startList;
float *vertices[tMax], *normals[tMax], *textures[tMax];
int *quads[tMax], sphdim[tMax];
GLfloat mat_specular[tMax][4];
GLfloat mat_shininess[tMax];
float spin[tMax] = { 0.0, 0.0 }, spinc[tMax] = { 1, (float)1.0 / 28.0 };
float orbit[tMax] = { 365.25, 28 }, rotInc[tMax] = { 0.0, 10 * 1.0 / 28.0 };
//end texture stuff header

using std::vector;

/* Win32 calling conventions. */
#ifndef CALLBACK
#define CALLBACK
#endif

GLuint startList;
GLfloat gAngle = 0.0f;
GLfloat alpha, beta;
GLint angleIndex = 0;
vector<float> alphaVec;
vector<float> betaVec;


void CALLBACK errorCallback(GLenum errorCode)
{
	const GLubyte *estring;

	estring = gluErrorString(errorCode);
	fprintf(stderr, "Quadric Error: %s\n", estring);
	exit(0);
}

//SHADER FUNCTIONS

//read in shader files
static char* readShaderSource(const char* shaderFile){
	FILE* fp = fopen(shaderFile, "r");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);

	buf[size] = '\0';
	fclose(fp);

	return buf;
}

GLuint InitShader(const char* vShaderFile, const char* fShaderFile)
{
	struct Shader {
		const char*  filename;
		GLenum       type;
		GLchar*      source;
	}  shaders[2] = {
		{ vShaderFile, GL_VERTEX_SHADER, NULL },
		{ fShaderFile, GL_FRAGMENT_SHADER, NULL }
	};

	GLuint program = glCreateProgram();



	for (int i = 0; i <2; ++i) {
		Shader& s = shaders[i];
		int size;
		s.source = readShaderSource(s.filename);
		size = strlen(s.source);
		std::cout << "s.source.size=" << size << "\n";
		std::cout << "s.source=" << s.source << "\n";
		if (shaders[i].source == NULL) {
			std::cerr << "Failed to read " << s.filename << std::endl;
			exit(EXIT_FAILURE);
		}

		GLuint shader = glCreateShader(s.type);

		glShaderSource(shader, 1, (const GLchar**)&s.source, NULL);
		glCompileShader(shader);

		GLint  compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled) {
			std::cerr << s.filename << " failed to compile:" << std::endl;
			GLint  logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			std::cerr << logMsg << std::endl;
			delete[] logMsg;

			exit(EXIT_FAILURE);
		}

		delete[] s.source;

		glAttachShader(program, shader);
	}

	/* link  and error check */
	glLinkProgram(program);

	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (!linked) {
		std::cerr << "Shader program failed to link" << std::endl;
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		std::cerr << logMsg << std::endl;
		delete[] logMsg;

		exit(EXIT_FAILURE);
	}

	/* use program object */
	glUseProgram(program);

	return program;
}


void readFromFile()
{
	FILE *Angles;
	Angles = fopen("angles.txt", "r");
	bool isNext = false;

	if (Angles == NULL)
	{
		printf("\ncant open the angles file\n");
		exit(1);
	}

	for (int i = 0; i < 10368; ++i){
		if (isNext == false){
			fscanf(Angles, "%f", &alpha);
			alphaVec.push_back(alpha);
			isNext = true;
		}
		else
		{
			fscanf(Angles, "%f", &beta);
			betaVec.push_back(beta);
			isNext = false;
		}

		//printf("\nalpha: %f \nbeta: %f\n", alpha, beta);
	}
	alpha = 0.0;
	beta = 0.0;
}

void timer(int value)
{
	const int desiredFPS = 120;
	glutTimerFunc(1000 / desiredFPS, timer, ++value);
	GLfloat dt = 1. / desiredFPS;

	//put your specific idle code here
	//... this code will run at desiredFPS
	gAngle += dt*360. / 8.; //rotate 360 degrees every 8 seconds
	//end your specific idle code here
	if (angleIndex < alphaVec.size())
	{
		alpha = alphaVec[angleIndex];
		beta = betaVec[angleIndex];
		 
		angleIndex += 1;
	}
	else 
	{
		angleIndex = 0;
		alpha = alphaVec[angleIndex];
		beta = betaVec[angleIndex];
	}
	glutPostRedisplay(); // initiate display() call at desiredFPS rate
}


//texture functions
void sphere(int slices, int stacks, float radius, int tag){
	int i, j, nsl;
	float delsl, delst, x, y, z;
	delsl = 2 * 3.14 / slices;
	nsl = slices + 1;
	delst = (3.14 - 2 * eps*radius) / stacks;
	vertices[tag] = (float *)calloc(3 * nsl*stacks, sizeof(float));
	normals[tag] = (float *)calloc(3 * nsl*stacks, sizeof(float));
	textures[tag] = (float *)calloc(2 * nsl*stacks, sizeof(float));
	quads[tag] = (int *)calloc(4 * slices*(stacks - 1), sizeof(int));
	sphdim[tag] = slices*(stacks - 1);
	for (j = 0; j<stacks; j++)
		for (i = 0; i<nsl; i++){
			x = radius*sin(delst*j + eps)*cos(i*delsl);
			y = radius*cos(delst*j + eps);
			z = radius*sin(delst*j + eps)*sin(i*delsl);
			vertices[tag][3 * nsl*j + 3 * i] = x;
			vertices[tag][3 * nsl*j + 3 * i + 1] = y;
			vertices[tag][3 * nsl*j + 3 * i + 2] = z;
			normals[tag][3 * nsl*j + 3 * i] = x;
			normals[tag][3 * nsl*j + 3 * i + 1] = y;
			normals[tag][3 * nsl*j + 3 * i + 2] = z;
			textures[tag][2 * nsl*j + 2 * i] = ((float)nsl - 1 - i) / (slices - 1);
			textures[tag][2 * nsl*j + 2 * i + 1] = ((float)j) / stacks;
			if ((j < stacks - 1) && (i < slices)){
				quads[tag][4 * slices*j + 4 * i] = nsl*j + i + 1;
				quads[tag][4 * slices*j + 4 * i + 1] = nsl*j + i;
				quads[tag][4 * slices*j + 4 * i + 2] = nsl*(j + 1) + i;
				quads[tag][4 * slices*j + 4 * i + 3] = nsl*(j + 1) + i + 1;
			}
		}

}


void readdata(char * filename, int tag)
{
	FILE *infile;
	unsigned char b[100], c, cred, cblue, cgreen;
	int i, j, nmi, mmj, n, m, k, nm;
	float s;

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular[tag]);
	glMaterialfv(GL_FRONT, GL_SHININESS, &mat_shininess[tag]);
	infile = fopen(filename, "r");
	fscanf(infile, "%[^\n] ", b);
	if (b[0] != 'P' || b[1] != '6')
	{
		printf("%s is not a PPM file!\n", b); exit(0);
	}
	printf("%s is a PPM file\n", b);
	fscanf(infile, "%c", &c);
	while (c == '#') {
		fscanf(infile, "%[^\n] ", b);
		printf("%s\n", b);
		fscanf(infile, "%c", &c);
	}
	ungetc(c, infile);
	fscanf(infile, "%d %d %d", &n, &m, &k);
	printf("%d rows %d columns max value= %d\n", n, m, k);
	c = fgetc(infile);
	s = 255. / k;
	nm = n*m;
	ImageWidth[tag] = n;
	ImageHeight[tag] = m;
	Image[tag] = (GLubyte *)malloc(4 * sizeof(GLubyte)*nm);
	printf("starting\n");
	for (j = 0; j<m; j++)
		for (i = 0; i<n; i++){
			fscanf(infile, "%c%c%c", &cred, &cgreen, &cblue);
			Image[tag][j * 4 * n + i * 4] = cred;
			Image[tag][j * 4 * n + i * 4 + 1] = cgreen;
			Image[tag][j * 4 * n + i * 4 + 2] = cblue;
			Image[tag][j * 4 * n + i * 4 + 3] = 255;
		}


	glBindTexture(GL_TEXTURE_2D, texName[tag]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ImageWidth[tag], ImageHeight[tag], 0,
		GL_RGBA, GL_UNSIGNED_BYTE, Image[tag]);

	glActiveTexture(GL_TEXTURE0);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);

}
//end texture functions



void init(void)
{
	GLUquadricObj *qobj;

	//glClearColor(0.0, 0.0, 0.0, 0.0);

	//glEnable(GL_DEPTH_TEST);


	/*  Create 4 display lists, each with a different quadric object.
	*  Different drawing styles and surface normal specifications
	*  are demonstrated.
	*/
	startList = glGenLists(4);
	qobj = gluNewQuadric();
	gluQuadricCallback(qobj, GLU_ERROR,
		(GLvoid(CALLBACK*) ()) errorCallback);


	//cyl 1
	gluQuadricDrawStyle(qobj, GLU_FILL); 
	gluQuadricNormals(qobj, GLU_FLAT);
	glNewList(startList, GL_COMPILE);
	gluCylinder(qobj, 0.1, 0.1, 2.0, 15, 5);
	glEndList();


	//cyl 2
	gluQuadricDrawStyle(qobj, GLU_FILL); 
	gluQuadricNormals(qobj, GLU_FLAT);
	glNewList(startList + 1, GL_COMPILE);
	gluCylinder(qobj, 0.4, 0.4, 1.0, 15, 5);
	glEndList();

	//sphere
	gluQuadricDrawStyle(qobj, GLU_FILL); /* smooth shaded */
	//   gluQuadricNormals(qobj, GLU_SMOOTH);
	glNewList(startList + 2, GL_COMPILE);
	gluSphere(qobj, 0.50, 15, 10);
	glEndList();

	
	//

	//texture and lighting

	//
	GLfloat light0_position[] = { 0.0, 0.0, 10.0, 1.0 };
	//GLfloat light0_position[] = { -1.0, 0.0, 1.0, 1.0 };
	glGenTextures(tMax, texName);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	mat_specular[0][0] = 1.0;
	mat_specular[0][1] = 1.0;
	mat_specular[0][2] = 1.0;
	mat_specular[0][3] = 1.0;
	mat_shininess[0] = 50.0;
	mat_specular[1][0] = 1.0;
	mat_specular[1][1] = 1.0;
	mat_specular[1][2] = 1.0;
	mat_specular[1][3] = 1.0;
	mat_shininess[0] = 50.0;
	readdata("earth.ppm", 0);
	readdata("moon.ppm", 1);
	sphere(256, 256, 0.5, 0);
	sphere(256, 256, 0.5, 1);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.1);
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.0);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);


	//

	

	// Specify an offset to keep track of where we're placing data in our
	//   vertex array buffer.  We'll use the same technique when we
	//   associate the offsets with vertex attribute pointers.
	GLintptr offset = 0;


	//shader
	// Load shaders and use the resulting shader program
	GLuint program = InitShader("vShader.glsl", "fShader.glsl");
	glUseProgram(program);

	//
	// set up vertex arrays
	offset = 0;
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	

	GLuint vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(vColor);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(offset));
	

	GLuint vTexCoord = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(vTexCoord);
	glVertexAttribPointer(vTexCoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	// Set the value of the fragment shader texture sampler variable
	//   ("texture") to the the appropriate texture unit. In this case,
	//   zero, for GL_TEXTURE0 which was previously set by calling
	//   glActiveTexture().
	glUniform1i(glGetUniformLocation(program, "texName"), 0);

	//


	glEnable(GL_DEPTH_TEST);
	glTranslatef(0.0, 0.0, 10.0);

	
}

void display_planet(int tag)
{
	int i, j, v;
	float x, y, z, s, t;
	glBindTexture(GL_TEXTURE_2D, texName[tag]);

	//spin[tag] = spin[tag] + spinc[tag];
	//if (spin[tag] > 360) spin[tag] = 0.0;
	glPushMatrix();
	//glRotatef(spin[tag], 0.0, 1.0, 0.0);
	glShadeModel(GL_SMOOTH);
	for (i = 0; i<sphdim[tag]; i++){
		glBegin(GL_QUADS);
		for (j = 0; j<4; j++){
			v = quads[tag][4 * i + j];
			x = vertices[tag][3 * v]; y = vertices[tag][3 * v + 1]; z = vertices[tag][3 * v + 2];
			s = textures[tag][2 * v];
			t = textures[tag][2 * v + 1];
			glTexCoord2f(s, t);  //gl_MultiTexCoord0
			glNormal3f(x, y, z); 
			glVertex3f(x, y, z); //gl_Vertex
		}
		glEnd();
	}
	//glDrawElements(GL_QUADS,sphdim, GL_INT, quads);
	glPopMatrix();
	glFlush();
	//glutSwapBuffers();
}


void display(void)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	
	//the order they are drawn is the order of the heirarchy
	//changing the transform of the higher objects influences anything under it
	//lower objects dont change anything above them

	/*
	push 1 matrix at the beginning for the 360 y rotation that affects the whole pendulum
	then push another matrix for the alpha rotation around the x axis
	then, without popping anything yet, we push another rotation for the beta rotation also around the x axis
	then we pop all 3
	what this does is that it lets the cylynders under the alpha rotation 
	get affected by that rotation aswell so that it looks like it's attached
	to the first group of cylinders.
	*/

	//alpha
	//cyl 1
	glShadeModel(GL_FLAT);
	//color and translation for the first cylinder
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(-0.5, 2.0, 0.0);
	//vertical y axis rotation
	glRotatef(gAngle, 0.0, 1.0, 0.0); 
	glPushMatrix();
	//alpha rotation
	glRotatef(alpha, 1.0, 0.0, 0.0);
	//cyl 1
	glCallList(startList);
	//cyl 2
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(1.0, 0.0, 0.0);
	glCallList(startList);
	//ball
	glColor3f(0.0, 0.0, 1.0);
	glTranslatef(-0.5, 0.0, 2.0);
	//glCallList(startList + 2);
	//display_planet(0);
	

	//beta
	//cyl 4
	glShadeModel(GL_FLAT);
	//color and translation of first cylinder
	glColor3f(1.0, 1.0, 1.0);
	glTranslatef(0.0f, 0.0, 0.0);
	glPushMatrix();
	//beta rotation
	glRotatef(beta, 1.0, 0.0, 0.0);
	//cyl 1
	//put the first planet here to spin appropriately
	display_planet(0);
	glCallList(startList);
	//ball
	glColor3f(0.0, 0.0, 1.0);
	glTranslatef(0.0f, 0.0, 2.0);
	//glCallList(startList + 2);
	display_planet(1);

	//pop the beta matrix
	glPopMatrix();
	//pop the alpha matrix
	glPopMatrix();
	//pop the vertical rotation matrix
	glPopMatrix();
	//...and flush
	glFlush();
	glutSwapBuffers();
}

void reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-5.5, 5.5, -5.5*(GLfloat)h / (GLfloat)w,
		5.5*(GLfloat)h / (GLfloat)w, -10.0, 10.0);
	else
		glOrtho(-5.5*(GLfloat)w / (GLfloat)h,
		5.5*(GLfloat)w / (GLfloat)h, -5.5, 5.5, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		exit(0);
		break;
	}
}

int main(int argc, char **argv)
{

	readFromFile();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1000, 500);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Pendulum");

	//display version of glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	

	//functions
	init();
	glutTimerFunc(0, timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	glutMainLoop();

	return 0;
}