#include <stdlib.h>
#include <GL/glut.h>
#include <string.h>
#include <stdio.h>

int x1 = 0;
int y1 = 0;
int x2 = 30;
int y2 = 30;
int x3 = 60;

int p1 = 420;
int p2 = 450;
int p3 = 480;
int q1 = 420;
int q2 = 450;

int moveSpace = 120;

bool playerMove = true;
bool enemyMove = false;

void drawPiece(int x1, int y1, int x2, int y2, int x3){
	glLoadName(3);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 0.0, 1.0);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glVertex2i(x3, y1);
	glEnd();
}

void drawPiece2(int p1, int q1, int p2, int q2, int p3){
	glLoadName(4);
	glBegin(GL_TRIANGLES);
	glColor3f(1.0, 1.0, 0.0);
	glVertex2i(p1, q1);
	glVertex2i(p2, q2);
	glVertex2i(p3, q1);
	glEnd();
}



void drawBoard(GLenum mode){
	int i, j;

		for (i = 0; i < 8; ++i){
			for (j = 0; j < 8; ++j){
				if ((i + j) % 2 == 0)
				{
					glLoadName(2);
					glColor3f(0.0, 0.0, 0.0);
				}
				else
				{
					glLoadName(1);
					glColor3f(1.0, 0.0, 0.0);
				}

				//glRecti(x1, y1, x2, y2)
				//glRecti(i * 60, j * 60, (i + 1) * 60, (j + 1) * 60);
				glBegin(GL_POLYGON);
				glVertex2i(i * 60, j * 60);
				glVertex2i((i + 1) * 60, j * 60);
				glVertex2i((i + 1) * 60, (j + 1) * 60);
				glVertex2i(i * 60, (j + 1) * 60);
				glEnd();
			}
		}
	
}

//display function
void mydisplay(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT); 
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);


	drawBoard(GL_RENDER);
	drawPiece(x1, y1, x2, y2, x3);
	drawPiece2(p1, q1, p2, q2, p3);
	/* flush GL buffers */

	glFlush();
}

void processHits(GLint hits, GLuint buffer[])
{
	unsigned int i, j;
	GLuint names, *ptr;

	printf("hits = %d\n", hits);
	ptr = (GLuint *)buffer;
	for (i = 0; i < hits; i++)
	{ /*  for each hit  */
		names = *ptr;
		ptr += 3;
		for (j = 0; j < names; j++)
		{ /*  for each name */
			if (*ptr == 1) printf("red sqaure\n");
			else if (*ptr == 3)
			{
				enemyMove = false;
				playerMove = true;
				printf("play piece\n");
			}
			else if (*ptr == 4)
			{ 
				playerMove = false;
				enemyMove = true;
				printf("enemy piece\n"); 
			}
			else printf("black sqaure\n");
			ptr++;
		}
		printf("\n");
	}
}

#define SIZE 512

//mouse control function
void mouse(int btn, int state, int x, int y){
	GLuint selectBuf[SIZE];
	GLint hits;
	GLint viewport[4];

	if (btn == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		glGetIntegerv(GL_VIEWPORT, viewport);

		glSelectBuffer(SIZE, selectBuf);
		glRenderMode(GL_SELECT);

		glInitNames();
		glPushName(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		/* create 5x5 pixel picking region near cursor location */
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3] - y),
			5.0, 5.0, viewport);
		gluOrtho2D(0.0, 640.0, 0.0, 480.0);
		drawBoard(GL_SELECT);
		drawPiece(x1, y1, x2, y2, x3);
		drawPiece2(p1, q1, p2, q2, p3);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glFlush();

		hits = glRenderMode(GL_RENDER);
		processHits(hits, selectBuf);

		glutPostRedisplay();
	}
}

void keyboard(unsigned char key, int x, int y)
{
	if (playerMove == true){
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case 101:
			if (y2 != 450 || x3 != 480){
				y1 += 60;
				y2 += 60;
				x1 += 60;
				x2 += 60;
				x3 += 60;
				if (y2 > 450 || x3 > 480)
				{
					y1 -= 60;
					y2 -= 60;
					x1 -= 60;
					x2 -= 60;
					x3 -= 60;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (y2 != 450 || x3 != 480){
						y1 += 60;
						y2 += 60;
						x1 += 60;
						x2 += 60;
						x3 += 60;
						if (y2 > 450 || x3 > 480)
						{
							y1 -= 60;
							y2 -= 60;
							x1 -= 60;
							x2 -= 60;
							x3 -= 60;
						}
					}
					else
					{
						y1 -= 60;
						y2 -= 60;
						x1 -= 60;
						x2 -= 60;
						x3 -= 60;
					}
				}
			}
			break;
		case 113:
			if (y1 != 0 || x1 != 0){
				y1 -= 60;
				y2 -= 60;
				x1 -= 60;
				x2 -= 60;
				x3 -= 60;
				if (y1 < 0 || x1 < 0)
				{
					y1 += 60;
					y2 += 60;
					x1 += 60;
					x2 += 60;
					x3 += 60;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (y1 != 0 || x1 != 0){
						y1 -= 60;
						y2 -= 60;
						x1 -= 60;
						x2 -= 60;
						x3 -= 60;
						if (y1 < 0 || x1 < 0)
						{
							y1 += 60;
							y2 += 60;
							x1 += 60;
							x2 += 60;
							x3 += 60;
						}
					}
					else
					{
						y1 += 60;
						y2 += 60;
						x1 += 60;
						x2 += 60;
						x3 += 60;
					}
				}
			}
			break;
		case 119:
			//drawPiece(x1, y1 + 60, x2, y2 + 60, x3);
			if (y2 != 450){
				y1 += moveSpace;
				y2 += moveSpace;
				if (y2 > 450)
				{
					y1 -= moveSpace;
					y2 -= moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (y2 != 450){
						y1 += moveSpace;
						y2 += moveSpace;
						if (y2 > 450)
						{
							y1 -= moveSpace;
							y2 -= moveSpace;
						}
					}
					else
					{
						y1 -= moveSpace;
						y2 -= moveSpace;
					}
				}
			}
			break;
		case 100:
			//drawPiece(x1 + 60, y1, x2 + 60, y2, x3 + 60)
			if (x3 != 480){
				x1 += moveSpace;
				x2 += moveSpace;
				x3 += moveSpace;
				if (x3 > 480)
				{
					x1 -= moveSpace;
					x2 -= moveSpace;
					x3 -= moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (x3 != 480){
						x1 += moveSpace;
						x2 += moveSpace;
						x3 += moveSpace;
						if (x3 > 480)
						{
							x1 -= moveSpace;
							x2 -= moveSpace;
							x3 -= moveSpace;
						}
					}
					else
					{
						x1 -= moveSpace;
						x2 -= moveSpace;
						x3 -= moveSpace;
					}
				}
			}
			break;
		case 97:
			if (x1 != 0){
				x1 -= moveSpace;
				x2 -= moveSpace;
				x3 -= moveSpace;
				if (x1 < 0)
				{
					x1 += moveSpace;
					x2 += moveSpace;
					x3 += moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (x1 != 0){
						x1 -= moveSpace;
						x2 -= moveSpace;
						x3 -= moveSpace;
						if (x1 < 0)
						{
							x1 += moveSpace;
							x2 += moveSpace;
							x3 += moveSpace;
						}
					}
					else
					{
						x1 += moveSpace;
						x2 += moveSpace;
						x3 += moveSpace;
					}
				}
			}
			break;
		case 115:
			if (y1 != 0){
				y1 -= moveSpace;
				y2 -= moveSpace;
				if (y1 < 0)
				{
					y1 += moveSpace;
					y2 += moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (y1 != 0){
						y1 -= moveSpace;
						y2 -= moveSpace;
						if (y1 < 0)
						{
							y1 += moveSpace;
							y2 += moveSpace;
						}
					}
					else
					{
						y1 += moveSpace;
						y2 += moveSpace;
					}
				}
			}
			break;
		}
	}
	else if (enemyMove == true)
	{
		switch (key)
		{
		case 27:
			exit(0);
			break;
		case 101:
			if (q2 != 450 || p3 != 480){
				q1 += 60;
				q2 += 60;
				p1 += 60;
				p2 += 60;
				p3 += 60;
				if (q2 > 450 || p3 > 480)
				{
					q1 -= 60;
					q2 -= 60;
					p1 -= 60;
					p2 -= 60;
					p3 -= 60;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (q2 != 450 || p3 != 480){
						q1 += 60;
						q2 += 60;
						p1 += 60;
						p2 += 60;
						p3 += 60;
						if (q2 > 450 || p3 > 480)
						{
							q1 -= 60;
							q2 -= 60;
							p1 -= 60;
							p2 -= 60;
							p3 -= 60;
						}
					}
					else
					{
						q1 -= 60;
						q2 -= 60;
						p1 -= 60;
						p2 -= 60;
						p3 -= 60;
					}
				}
			}
			break;
		case 113:
			if (q1 != 0 || p1 != 0){
				q1 -= 60;
				q2 -= 60;
				p1 -= 60;
				p2 -= 60;
				p3 -= 60;
				if (q1 < 0 || p1 < 0)
				{
					q1 += 60;
					q2 += 60;
					p1 += 60;
					p2 += 60;
					p3 += 60;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (q1 != 0 || p1 != 0){
						q1 -= 60;
						q2 -= 60;
						p1 -= 60;
						p2 -= 60;
						p3 -= 60;
						if (q1 < 0 || p1 < 0)
						{
							q1 += 60;
							q2 += 60;
							p1 += 60;
							p2 += 60;
							p3 += 60;
						}
					}
					else
					{
						q1 += 60;
						q2 += 60;
						p1 += 60;
						p2 += 60;
						p3 += 60;
					}
				}
			}
			break;
		case 119:
			//drawPiece(x1, y1 + 60, x2, y2 + 60, x3);
			if (q2 != 450){
				q1 += moveSpace;
				q2 += moveSpace;
				if (q2 > 450)
				{
					q1 -= moveSpace;
					q2 -= moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (q2 != 450){
						q1 += moveSpace;
						q2 += moveSpace;
						if (q2 > 450)
						{
							q1 -= moveSpace;
							q2 -= moveSpace;
						}
					}
					else
					{
						q1 -= moveSpace;
						q2 -= moveSpace;
					}
				}
			}
			break;
		case 100:
			//drawPiece(x1 + 60, y1, x2 + 60, y2, x3 + 60)
			if (p3 != 480){
				p1 += moveSpace;
				p2 += moveSpace;
				p3 += moveSpace;
				if (p3 > 480)
				{
					p1 -= moveSpace;
					p2 -= moveSpace;
					p3 -= moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (p3 != 480){
						p1 += moveSpace;
						p2 += moveSpace;
						p3 += moveSpace;
						if (p3 > 480)
						{
							p1 -= moveSpace;
							p2 -= moveSpace;
							p3 -= moveSpace;
						}
					}
					else
					{
						p1 -= moveSpace;
						p2 -= moveSpace;
						p3 -= moveSpace;
					}
				}
			}
			break;
		case 97:
			if (p1 != 0){
				p1 -= moveSpace;
				p2 -= moveSpace;
				p3 -= moveSpace;
				if (p1 < 0)
				{
					p1 += moveSpace;
					p2 += moveSpace;
					p3 += moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (p1 != 0){
						p1 -= moveSpace;
						p2 -= moveSpace;
						p3 -= moveSpace;
						if (p1 < 0)
						{
							p1 += moveSpace;
							p2 += moveSpace;
							p3 += moveSpace;
						}
					}
					else
					{
						p1 += moveSpace;
						p2 += moveSpace;
						p3 += moveSpace;
					}
				}
			}
			break;
		case 115:
			if (q1 != 0){
				q1 -= moveSpace;
				q2 -= moveSpace;
				if (q1 < 0)
				{
					q1 += moveSpace;
					q2 += moveSpace;
				}
				glutPostRedisplay();
				if (y1 == q1 && y2 == q2 && x1 == p1 && x2 == p2 && x3 == p3){
					if (q1 != 0){
						q1 -= moveSpace;
						q2 -= moveSpace;
						if (q1 < 0)
						{
							q1 += moveSpace;
							q2 += moveSpace;
						}
					}
					else
					{
						q1 += moveSpace;
						q2 += moveSpace;
					}
				}
			}
			break;
		}

	}

}


void reshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}


/**************************************** main() ********************/
void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glColor3f(1.0, 1.0, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("checkers");
	glutDisplayFunc(mydisplay);
	glutMouseFunc(mouse);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);

	init();

	glutMainLoop();

}



