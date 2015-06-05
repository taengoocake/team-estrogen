//Main things to remember right now.
// '+' speeds up
//'-' slows down
//use arrow keys to change camera position slightly
//1-5 changes to preset camera positions
//space pauses



#include <stdlib.h>
#include <stdio.h>
#include <GL/glut.h>
#include <time.h>
#include <math.h>

#include <windows.h>


#define PADDLE1		1
#define PADDLE2		2
#define BALL		3
#define PLAYFIELD	4
#define LIGHT		5

//data structures for paddle and ball  

typedef struct {
	GLfloat position[3]; // x, y, z
	GLfloat size[3]; // width, height, thickness
	GLfloat color[3]; // red, green, blue
	GLint score;
} paddle_t;

typedef struct {
	GLfloat position[3]; // x, y, z
	GLfloat orientation[3]; // rotation just for fun
	GLfloat delta[6]; // dx, dy, dz
	GLfloat radius;
	GLfloat color[3]; // red, green, blue
} ball_t;

// Vertices numbers and camera
static GLfloat vertices[][3] =	{
				{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
				{1.0,1.0,-1.0},{-1.0,1.0,-1.0},
				{-1.0,-1.0,1.0},{1.0,-1.0,1.0},
				{1.0,1.0,1.0},{-1.0,1.0,1.0}
				};

static GLfloat eye[] = {0.0, 0.0, 7.0};
static GLfloat camera[] = {15.0, 85.0, 0.0};

static paddle_t paddle1 = {{-2.0, 0.0, 0.0}, {0.3, 0.3, 0.05}, {0.0, 1.0, 0.0}, 0};
static paddle_t paddle2 = {{2.0, 0.0, -.0}, {0.3, 0.3, 0.05}, {0.0, 0.0, 1.0}, 0};
static paddle_t *winner = NULL;
static ball_t ball = {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.02, 0.006, 0.01, 3.14, 14.3, 31.4}, 0.05, {1.0, 1.0, 1.0}};

static char paused = 0;
static char wireframe = 0;
static char alpha = 1;
static char light = 1;
static int frameInterval = (CLOCKS_PER_SEC / 100);

GLfloat rnd()
{
	return (rand() % 255) / 255.0;
}

void relaunchBall()
{
	if (winner == &paddle1) {
		ball.position[0] = paddle1.position[0] + paddle1.size[2];
		ball.position[1] = paddle1.position[1];
		ball.position[2] = paddle1.position[2];
		
		ball.delta[0] = rnd() / 20.0 + 0.005;
		ball.delta[1] = rnd() / 20.0 + 0.005;
		ball.delta[2] = rnd() / 20.0 + 0.005;

		if (ball.delta[0] < 0.0)
			ball.delta[0] = -ball.delta[0];
		if (rand() & 1)
			ball.delta[1] = -ball.delta[1];
		if (rand() & 1)
			ball.delta[2] = -ball.delta[2];
	} else {
		ball.position[0] = paddle2.position[0] - paddle2.size[2];
		ball.position[1] = paddle2.position[1];
		ball.position[2] = paddle2.position[2];
		
		ball.delta[0] = rnd() / 20.0 + 0.01;
		ball.delta[1] = rnd() / 20.0 + 0.005;
		ball.delta[2] = rnd() / 20.0 + 0.005;

		if (ball.delta[0] > 0.0)
			ball.delta[0] = -ball.delta[0];
		if (rand() & 1)
			ball.delta[1] = -ball.delta[1];
		if (rand() & 1)
			ball.delta[2] = -ball.delta[2];
	}
	
	ball.color[0] = 1.0;
	ball.color[1] = 1.0;
	ball.color[2] = 1.0;
}

void opponentMove()
{
	// super cool AI stuff
	static GLfloat dy = 0.03, dz = 0.03;

	if (ball.delta[0] < 0.0)
		return;
	
	if (paddle2.position[1] < ball.position[1] && paddle2.position[1] + paddle2.size[1] + dy <= 1.0)
		paddle2.position[1] += dy;
	else if (paddle2.position[1] > ball.position[1] && paddle2.position[1] - paddle2.size[1] - dy >= -1.0)
		paddle2.position[1] -= dy;
	
	if (paddle2.position[2] < ball.position[2] && paddle2.position[2] + paddle2.size[0] + dz <= 1.0)
		paddle2.position[2] += dz;
	else if (paddle2.position[2] > ball.position[2] && paddle2.position[2] - paddle2.size[0] - dz >= -1.0)
		paddle2.position[2] -= dz;
}

void moveBall(void)
{
	static clock_t nextTime = 0;
	clock_t currentTime = clock();

	if (nextTime >= currentTime || paused)
		return;

	opponentMove();

	ball.position[0] += ball.delta[0];
	ball.position[1] += ball.delta[1];
	ball.position[2] += ball.delta[2];

	// Bounce
	if (ball.position[1] >= 1.0 - ball.radius || ball.position[1] <= -1.0 + ball.radius)
		ball.delta[1] = -ball.delta[1];
	if (ball.position[2] >= 1.0 - ball.radius || ball.position[2] <= -1.0 + ball.radius)
		ball.delta[2] = -ball.delta[2];

	// Lame collision detection for the paddles
	// The ball bounced on the paddle
	if (ball.position[0] < -2.0 + ball.radius + paddle1.size[2]
		&& ball.delta[0] < 0.0
		&& ball.position[1] >= paddle1.position[1] - paddle1.size[1]
		&& ball.position[1] <= paddle1.position[1] + paddle1.size[1]
		&& ball.position[2] >= paddle1.position[2] - paddle1.size[0]
		&& ball.position[2] <= paddle1.position[2] + paddle1.size[0]
	) {
		// paddle1 bounces ball
		ball.delta[0] = -ball.delta[0];

		ball.color[0] = paddle1.color[0];
		ball.color[1] = paddle1.color[1];
		ball.color[2] = paddle1.color[2];

		if (ball.position[1] > paddle1.position[1] + paddle1.size[1] / 2.0
			&& ball.position[1] <= paddle1.position[1] + paddle1.size[1] / 2.0
			&& ball.position[2] >= paddle1.position[2] - paddle1.size[0] / 2.0
	                && ball.position[2] <= paddle1.position[2] + paddle1.size[0] / 2.0) {
			// Accelerate forward
			ball.delta[0] *= 1.5;
		} else {
			// Accelerate sideways
			if (ball.delta[1] > 0.0) {
				if (ball.position[1] > paddle1.position[1])
					ball.delta[1] += 0.005;
				else
					ball.delta[1] = -ball.delta[1] + 0.005;
			} else {
				if (ball.position[1] < paddle1.position[1])
					ball.delta[1] -= 0.005;
				else
					ball.delta[1] = -ball.delta[1] - 0.005;
			}
			
			if (ball.delta[2] > 0.0) {
				if (ball.position[2] > paddle1.position[2])
					ball.delta[2] += 0.005;
				else
					ball.delta[2] = -ball.delta[2] + 0.005;
			} else {
				if (ball.position[2] < paddle1.position[2])
					ball.delta[2] -= 0.005;
				else
					ball.delta[2] = -ball.delta[2] - 0.005;
			}
		}
	}

	// Copy 'n paste evilness
	if (ball.position[0] > 2.0 - ball.radius - paddle2.size[2]
		&& ball.delta[0] > 0.0
		&& ball.position[1] >= paddle2.position[1] - paddle2.size[1]
		&& ball.position[1] <= paddle2.position[1] + paddle2.size[1]
		&& ball.position[2] >= paddle2.position[2] - paddle2.size[0]
		&& ball.position[2] <= paddle2.position[2] + paddle2.size[0]
	) {
		// paddle2 bounces ball
		ball.delta[0] = -ball.delta[0];

		ball.color[0] = paddle2.color[0];
		ball.color[1] = paddle2.color[1];
		ball.color[2] = paddle2.color[2];

		if (ball.position[1] > paddle2.position[1] + paddle2.size[1] / 2.0
			&& ball.position[1] <= paddle2.position[1] + paddle2.size[1] / 2.0
			&& ball.position[2] >= paddle2.position[2] - paddle2.size[0] / 2.0
	                && ball.position[2] <= paddle2.position[2] + paddle2.size[0] / 2.0) {
			// Accelerate forward
			ball.delta[0] *= 1.5;
		} else {
			// Accelerate sideways
			if (ball.delta[1] > 0.0) {
				if (ball.position[1] > paddle2.position[1])
					ball.delta[1] += 0.005;
				else
					ball.delta[1] = -ball.delta[1] + 0.005;
			} else {
				if (ball.position[1] < paddle2.position[1])
					ball.delta[1] -= 0.005;
				else
					ball.delta[1] = -ball.delta[1] - 0.005;
			}
			
			if (ball.delta[2] > 0.0) {
				if (ball.position[2] > paddle2.position[2])
					ball.delta[2] += 0.005;
				else
					ball.delta[2] = -ball.delta[2] + 0.005;
			} else {
				if (ball.position[2] < paddle2.position[2])
					ball.delta[2] -= 0.005;
				else
					ball.delta[2] = -ball.delta[2] - 0.005;
			}
		}
	}
	

	// If the ball didn't bounce
	if (ball.position[0] > 2.0 - ball.radius) {
		winner = &paddle1;
		paddle1.score++;
		relaunchBall();
	}
	else if (ball.position[0] < -2.0 + ball.radius) {
		winner = &paddle2;
		paddle2.score++;
		relaunchBall();
	}
	
	// Normalize
	if (ball.position[1] > 1.0 - ball.radius)
		ball.position[1] = 1.0 - ball.radius;
	else if (ball.position[1] < -1.0 + ball.radius)
		ball.position[1] = -1.0 + ball.radius;
	if (ball.position[2] > 1.0 - ball.radius)
		ball.position[2] = 1.0 - ball.radius;
	else if (ball.position[2] < -1.0 + ball.radius)
		ball.position[2] = -1.0 + ball.radius;

	// it's a spinning ball
	ball.orientation[0] += ball.delta[3];
	ball.orientation[1] += ball.delta[4];
	ball.orientation[2] += ball.delta[5];
	
	// set frequency to 100hz
	nextTime = currentTime + frameInterval;
	glutPostRedisplay();
}

void text(GLint x, GLint y, char *text)
{
	char *s;
	glRasterPos2i(x, y);
	for (s = text; *s; s++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *s);
}

void polygon(int a, int b, int c , int d)
// Makes a polygon with vertices a, b, c, and d.
{
	glBegin(GL_POLYGON);
		glVertex3fv(vertices[a]);
		glVertex3fv(vertices[b]);
		glVertex3fv(vertices[c]);
		glVertex3fv(vertices[d]);
	glEnd();
}

void cube()
{
	polygon(0,3,2,1);
	polygon(2,3,7,6);
	polygon(0,4,7,3);
	polygon(1,2,6,5);
	polygon(4,5,6,7);
	polygon(0,1,5,4);
}

void colorcube()
{
	// Each face has a different colour

    glColor3ub(255,0,0); 
	polygon(0,3,2,1);
	
    glColor3ub(0,255,0); 
	polygon(2,3,7,6);
	
	glColor3ub(0,0,255); 
	polygon(0,4,7,3);

	glColor3ub(0,255,255); 
	polygon(1,2,6,5);

	glColor3ub(255,0,255); 
	polygon(4,5,6,7);

	glColor3ub(255,255,0); 
	polygon(0,1,5,4);
}

void kegel()
{
	GLfloat x,y,z,phi,r, thet;
    double c = 3.14159/180.0;

	x = y = 0.0;
	z = 1.0;
	r = 1.0;

	// underside
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, z);

		for (phi = 0.0; phi <= 360; phi += 20.0) {
			x = r * cos(c*phi);
			y = r * sin(c*phi);
			glVertex3f(x, y, z);
		}
	glEnd();

	x = y = 0.0;

	// node
	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, -z);

		for (phi = 0.0; phi <= 360; phi += 20.0) {
			x = r * cos(c*phi);
			y = r * sin(c*phi);
			glVertex3f(x, y, z);
		}
	glEnd();

	thet = 2.0 / 0.70;
	for (z = -1.0; z <= 1.0; z += 0.1) {
		glBegin(GL_QUAD_STRIP);
			r = (z+1.0) / thet;

			for (phi = 0.0; phi <= 360; phi += 20.0) {
				x = r * cos(c*phi);
				y = r * sin(c*phi);
				glVertex3f(x, y, z);

				x = r * cos(c*(phi+20.0));
				y = r * sin(c*(phi+20.0));
				glVertex3f(x, y, z);
			}
		glEnd();
	}
}

void cylinder()
{
	GLfloat x,y,z,phi;
    double c = 3.14159/180.0;

	x = y = 0.0;
	z = 1.0;

	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, z);

		for (phi = 0.0; phi <= 360; phi += 20.0) {
			x = cos(c*phi);
			y = sin(c*phi);
			glVertex3f(x, y, z);
		}
	glEnd();

	x = y = 0.0;
	z = -1.0;

	glBegin(GL_TRIANGLE_FAN);
		glVertex3f(x, y, z);

		for (phi = 0.0; phi <= 360; phi += 20.0) {
			x = cos(c*phi);
			y = sin(c*phi);
			glVertex3f(x, y, z);
		}
	glEnd()

	for (z = -1.0; z <= 0.8; z += 0.20) {
		glBegin(GL_QUAD_STRIP);
			for (phi = 0.0; phi < 360; phi += 20.0) {
				x = cos(c*phi);
				y = sin(c*phi);
				glVertex3f(x, y, z);

				x = cos(c*(phi+20.0));
				y = sin(c*(phi+20.0));
				glVertex3f(x, y, z);

				x = cos(c*phi);
				y = sin(c*phi);
				glVertex3f(x, y, z+0.20);

				x = cos(c*(phi+20.0));
				y = sin(c*(phi+20.0));
				glVertex3f(x, y, z+0.20);
			}
		glEnd();
	}
}

void coordinates()
{
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(100.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 100.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 100.0);
	glEnd();

	glLineStipple(1, 0xF00F);
	glEnable(GL_LINE_STIPPLE);
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(-100.0, 0.0, 0.0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, -100.0, 0.0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(0.0, 0.0, -100.0);
	glEnd();
	glDisable(GL_LINE_STIPPLE);
}

void drawWall()
{
	glBegin(GL_QUADS);
		// bottom
		glNormal3f(0.0, 1.0, 0.0);
		glVertex3f(-1.0, -1.0, 1.0);
		glVertex3f(1.0, -1.0, 1.0);
		glVertex3f(1.0, -1.0, -1.0);
		glVertex3f(-1.0, -1.0, -1.0);

		// top
		glNormal3f(0.0, -1.0, 0.0);
		glVertex3f(-1.0, 1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glVertex3f(1.0, 1.0, -1.0);
		glVertex3f(-1.0, 1.0, -1.0);

		// near-side
		glNormal3f(0.0, 0.0, 1.0);
		glVertex3f(-1.0, 1.0, 1.0);
		glVertex3f(1.0, 1.0, 1.0);
		glVertex3f(1.0, -1.0, 1.0);
		glVertex3f(-1.0, -1.0, 1.0);

		// far-side
		glNormal3f(0.0, 0.0, -1.0);
		glVertex3f(-1.0, 1.0, -1.0);
		glVertex3f(1.0, 1.0, -1.0);
		glVertex3f(1.0, -1.0, -1.0);
		glVertex3f(-1.0, -1.0, -1.0);
	glEnd();
}

void drawPaddle1()
{
	// middle
	glPushMatrix();
		glScalef(1.0, 0.5, 0.5);
		cube();
	glPopMatrix();

	// horizontal cross
	glPushMatrix();
		glScalef(0.5, 0.3, 1.0);
		cube();
	glPopMatrix();

	// vertical cross
	glPushMatrix();
		glScalef(0.5, 1.0, 0.3);
		cube();
	glPopMatrix();
}

void drawPaddle2()
{
	// middle
	glPushMatrix();
		glScalef(1.0, 0.75, 0.75);
		cube();
	glPopMatrix();

	// side cilinders
	glPushMatrix();
		glRotatef(90.0, 1.0, 0.0, 0.0);
		glScalef(1.0, 0.15, 1.0);
		glTranslatef(0.0, 5.0, 0.0);
		cylinder();

		glTranslatef(0.0, -10.0, 0.0);
		cylinder();
	glPopMatrix();
}

void drawBall()
{
	// left ball
	glPushMatrix();
		glTranslatef(-2.0, 0.0, 0.0);
		glutSolidSphere(1.0, 6, 6);
	glPopMatrix();

	// connecting cilinder
	glPushMatrix();
		glRotatef(90.0, 0.0, 1.0, 0.0);
		glScalef(0.3, 0.3, 2.0);
		cylinder();
	glPopMatrix();
	
	// right ball
	glPushMatrix();
		glTranslatef(2.0, 0.0, 0.0);
		glutSolidSphere(1.0, 6, 6);
	glPopMatrix();
}

void drawLight()
{
	glPushMatrix();
		glScalef(2.0, 2.0, 1.0);
		kegel();
	glPopMatrix();
}

void display(void)
{
	static char buffer[20];

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	if (wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	if (light)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	if (alpha)
		glEnable(GL_BLEND);
	else
		glDisable(GL_BLEND);

	glPushMatrix();
		glLoadIdentity();
		glTranslatef(-eye[0], -eye[1], -eye[2]);

		// Lighting
		glPushMatrix();
		{
			GLfloat LightPosition[] = { 0.0, 0.0, 0.0, 1.0f };
			glTranslatef(ball.position[0], ball.position[1], ball.position[2]);
			glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
		}
		glPopMatrix();

		// Light off for the score
			glDisable(GL_LIGHTING);
			// player 1 score
			glColor3f(0.0, 1.0, 0.0);
			sprintf(buffer, "%d", paddle1.score);
			text(-2, 2, buffer);

			// player 2 score
			glColor3f(0.0, 0.0, 1.0);
			sprintf(buffer, "%d", paddle2.score);
			text(2, 2, buffer);
		// light on
		if (light)
			glEnable(GL_LIGHTING);

		glRotatef(camera[0], 1.0, 0.0, 0.0);
		glRotatef(camera[1], 0.0, 1.0, 0.0);
		glRotatef(camera[2], 0.0, 0.0, 1.0);

		// the ball
		glColor3f(ball.color[0], ball.color[1], ball.color[2]);
		glPushMatrix();
			glTranslatef(ball.position[0], ball.position[1], ball.position[2]);
			glScalef(ball.radius, ball.radius, ball.radius);
			glRotatef(ball.orientation[0], 1.0, 0.0, 0.0);
			glRotatef(ball.orientation[0], 0.0, 1.0, 0.0);
			glRotatef(ball.orientation[0], 0.0, 0.0, 1.0);
			drawBall();
		glPopMatrix();

		// opponent paddle
		glColor4f(paddle2.color[0], paddle2.color[1], paddle2.color[2], 0.5);
		glPushMatrix();
			glTranslatef(paddle2.position[0] - paddle2.size[2], paddle2.position[1], paddle2.position[2]);
			glScalef(paddle2.size[2], paddle2.size[1], paddle2.size[0]);
			drawPaddle2();
		glPopMatrix();

		// player paddle
		glColor4f(paddle1.color[0], paddle1.color[1], paddle1.color[2], 0.5);
		glPushMatrix();
			glTranslatef(paddle1.position[0] + paddle2.size[2], paddle1.position[1], paddle1.position[2]);
			glScalef(paddle1.size[2], paddle1.size[1], paddle1.size[0]);
			drawPaddle1();
		glPopMatrix();

		// playing area
		glColor4f(1.0, 0.0, 0.0, 0.3);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glPushMatrix();
			// object consists of two walls
			glTranslatef(-1.0, 0.0, 0.0);
			drawWall();
			glTranslatef(2.0, 0.0, 0.0);
			drawWall();
		glPopMatrix();

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void mouseMove(int x, int y)
{
	static GLint oldX = 0;
	static GLint oldY = 0;

	if (paused)
		return;

	// moving up
	if (oldY > y && paddle1.position[1] < 1.0 - paddle1.size[1] - 0.01)
		paddle1.position[1] += 0.05;
	// downwards
	if (oldY < y && paddle1.position[1] > -1.0 + paddle1.size[1] + 0.01)
		paddle1.position[1] -= 0.05;

	// to the right
	if (oldX < x && paddle1.position[2] < 1.0 - paddle1.size[0] - 0.01)
		paddle1.position[2] += 0.05;
	// to the left
	if (oldX > x && paddle1.position[2] > -1.0 + paddle1.size[0] + 0.01)
		paddle1.position[2] -= 0.05;

	oldX = x;
	oldY = y;

	glutPostRedisplay();
}

void mouse(int btn, int state, int x, int y)
{
	if (btn == GLUT_LEFT_BUTTON) {
	} else if (btn == GLUT_MIDDLE_BUTTON) {
	} else if (btn == GLUT_RIGHT_BUTTON) {
	}

	glutPostRedisplay();
}

void special(int value, int x, int y)
{
    switch (value)
    {
        case GLUT_KEY_F1:
			glutFullScreen();
			break;
		case GLUT_KEY_F2:
			glutPositionWindow(100, 100);
			glutReshapeWindow(500, 500);
			break;
		case GLUT_KEY_UP:
			camera[0] -= 3.0;
			break;
		case GLUT_KEY_DOWN:
			camera[0] += 3.0;
			break;
		case GLUT_KEY_RIGHT:
			camera[1] += 3.0;
			break;
		case GLUT_KEY_LEFT:
			camera[1] -= 3.0;
			break;

    }

	glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
	switch (key) {
		case ' ': // pause toggle
			paused = !paused;
			break;
		case '+': // increase speed
			frameInterval /= 2;
			printf("Interval %d\n", frameInterval);
			break;
		case '-': // decrease speed
			frameInterval *= 2;
			if (frameInterval == 0)
				frameInterval = 1;
			printf("Interval %d\n", frameInterval);
			break;
		case 'w': // wireframe toggle
			wireframe = !wireframe;
			break;
		case 'a': // alpha-blending toggle
			alpha = !alpha;
			break;
		case 'l': // light toggle
			light = !light;
			break;
		case 27: // ESCAPE key
			exit(0);
			break;
		case 'r':
			eye[2] -= 1.0;
			break;
		case 'R':
			eye[2] += 1.0;
			break;
		case 'i':
			relaunchBall();
			break;
		case '1': // restore original position
			eye[0] = 0.0;
			eye[1] = 0.0;
			eye[2] = 7.0;
			camera[0] = 15.0;
			camera[1] = 85.0;
			camera[2] = 0.0;
			break;
		case '2': // backview
			eye[0] = 0.0;
			eye[1] = 0.0;
			eye[2] = 7.0;
			camera[0] = 0.0;
			camera[1] = 90.0;
			camera[2] = 0.0;
			break;
		case '3': // topview
			eye[0] = 0.0;
			eye[1] = 0.0;
			eye[2] = 7.0;
			camera[0] = 90.0;
			camera[1] = 90.0;
			camera[2] = 0.0;
			break;
		case '4': // reverse backview (crazy)
			eye[0] = 0.0;
			eye[1] = 0.0;
			eye[2] = 7.0;
			camera[0] = 0.0;
			camera[1] = -90.0;
			camera[2] = 0.0;
			break;
		case '5': // bottomview
			eye[0] = 0.0;
			eye[1] = 0.0;
			eye[2] = 7.0;
			camera[0] = -90.0;
			camera[1] = 90.0;
			camera[2] = 0.0;
			break;
		default:
			break;
	}

   glutPostRedisplay();
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity();

	gluPerspective(45.0, (float)w/(float)h, 1.0, 300.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void disablePointer()
{
#ifdef WIN32
	ShowCursor(FALSE);
#endif

#ifdef LINUX
  if ( None == pointer ) {
	  char bm[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	  Pixmap pix = XCreateBitmapFromData( dpy, win, bm, 8, 8 );
	  XColor black;
	  memset( &black, 0, sizeof( XColor ) );
	  black.flags = DoRed | DoGreen | DoBlue;
	  pointer = XCreatePixmapCursor( dpy, pix, pix, &black, &black, 0, 0 );
	  XFreePixmap( dpy, pix );
  }
  XDefineCursor( dpy, win, pointer );
  XSync( dpy, False ); /* again, optional */
#endif
}

void initGL()
{
	GLfloat LightAmbient[]  = { 0.2f, 0.2f, 0.2f, 1.0f };
	GLfloat LightDiffuse[]  = { 0.5f, 0.5f, 0.5f, 0.5f };
	GLfloat LightPosition[] = { -eye[0], -eye[1] + 2.0, -eye[2], 1.0f };

	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	// For the light
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	// create the window
	glutInitWindowSize(500, 500);
	glutCreateWindow("PONG by Team Estrogen!");

	// misc initialisation
	initGL();
	srand( time(NULL) );
	relaunchBall(); // initiate the ball position
	disablePointer();

	// callbacks
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutIdleFunc(moveBall);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glutSpecialFunc(special);

	glutMainLoop();
	return 0;
}
