#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include <gl/gl.h>
#include <gl/freeglut.h>
#include <gl/glu.h> /* GLU extention library */

void init(void);
void display(void);
void keyboard(unsigned char, int, int);
void resize(int, int);
void drawcube(int, int, int);

int is_depth; /* depth testing flag */

GLuint texture[1];

int main (int argc, char **argv)
{

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("Boo!");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	//glutPassiveMotionFunc(look);
	//glutMotionFunc(look);

	/* constant aspect ratio */
	glutReshapeFunc(resize);  

	glutMainLoop();
	return 0;
}

void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	is_depth = 1;
	glMatrixMode(GL_MODELVIEW);
}

void display(void)
{
	if (is_depth)
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);

	/* draw the floor */
	glBegin(GL_QUADS);
		glColor3f(0.2f, 0.2f, 0.2f);
		glVertex3f(-100.0, 0.0, -100.0);
		glColor3f(0.4f, 0.4f, 0.4f);
		glVertex3f(-100.0, 0.0, 100.0);
		glColor3f(0.6f, 0.6f, 0.6f);
		glVertex3f(100.0, 0.0, 100.0);
		glColor3f(0.8f, 0.8f, 0.8f);
		glVertex3f(100.0, 0.0, -100.0);		
	glEnd();

	glDisable(GL_TEXTURE_2D);

	/* draw 12 cubes with different colors */
	drawcube(75, 57, 2);
	drawcube(-65, -12, 3);
	drawcube(50, -50, 1);
	drawcube(-56, 17, 2);
	drawcube(67, 12, 3);
	drawcube(-87, 32, 1);
	drawcube(-26, 75, 2);
	drawcube(57, 82, 3);
	drawcube(-3, 12, 1);
	drawcube(46, 35, 2);
	drawcube(37, -2, 3);
	glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
	/* This time the controls are:
	
	  "a": move left
	  "d": move right
	  "w": move forward
	  "s": move back
	  "t": toggle depth-testing

	*/
	switch (key)
	{
	case 033:
		glutLeaveMainLoop();
		break;
	case 'a':
	case 'A':
		glTranslatef(5.0, 0.0, 0.0);
		break;
	case 'd':
	case 'D':
		glTranslatef(-5.0, 0.0, 0.0);
		break;
	case 'w':
	case 'W':
		glTranslatef(0.0, 0.0, 5.0);
		break;
	case 's':
	case 'S':
		glTranslatef(0.0, 0.0, -5.0);
		break;
	case 't':
	case 'T':
		if (is_depth)
		{
			is_depth = 0;
			glDisable(GL_DEPTH_TEST);
		}
		else
		{
			is_depth = 1;
			glEnable(GL_DEPTH_TEST);
		}
	}
	display();
}

void resize(int width, int height)
{
	if (height == 0) height = 1;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	/* note we divide our width by our height to get the aspect ratio */
	gluPerspective(45.0, width / height, 1.0, 400.0);

	/* set initial position */
	glTranslatef(0.0, -5.0, -150.0);

	glMatrixMode(GL_MODELVIEW);
}

void drawcube(int x_offset, int z_offset, int color)
{
	/* this function draws a cube centerd at (x_offset, z_offset)
	x and z _big are the back and rightmost points, x and z _small are
	the front and leftmost points */

	float x_big = (float)x_offset + 5;
	float z_big = (float)z_offset + 5;
	float x_small = (float)x_offset - 5;
	float z_small = (float)z_offset - 5;
	switch(color)
	{
	case 1:
		glColor3f(1.0,0.0,0.0);
		break;
	case 2:
		glColor3f(0.0,1.0,0.0);
		break;
	case 3:
		glColor3f(0.0,0.0,1.0);
		break;
	}

	glBegin(GL_QUADS);
		glVertex3f(x_small,10.0,z_big); /* front */
		glVertex3f(x_small,0.0,z_big);
		glVertex3f(x_big,0.0,z_big);
		glVertex3f(x_big,10.0,z_big);

		glVertex3f(x_big,10.0,z_small); /* back */
		glVertex3f(x_big,0.0,z_small);
		glVertex3f(x_small,0.0,z_small);
		glVertex3f(x_small,10.0,z_small);

		glVertex3f(x_big,10.0,z_big); /* right */
		glVertex3f(x_big,0.0,z_big);
		glVertex3f(x_big,0.0,z_small);
		glVertex3f(x_big,10.0,z_small);

		glVertex3f(x_small,10.0,z_small); /* left */
		glVertex3f(x_small,0.0,z_small);
		glVertex3f(x_small,0.0,z_big);
		glVertex3f(x_small,10.0,z_big);

		glVertex3f(x_small,10.0,z_big); /* top */
		glVertex3f(x_big,10.0,z_big);
		glVertex3f(x_big,10.0,z_small);
		glVertex3f(x_small,10.0,z_small);

		glVertex3f(x_small,0.0,z_small); /* bottom */
		glVertex3f(x_big,0.0,z_small);
		glVertex3f(x_big,0.0,z_big);
		glVertex3f(x_small,0.0,z_big);
	glEnd();
}
