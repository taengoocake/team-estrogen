//3DPong.c
//'+' speeds up the ball
//'-' slows down the ball.
//Use the mouse to move your player paddle (red paddle) around
//Arrow keys tilt the camera a little bit.
//Spacebar pauses
//Page Up zooms in a bit. Page Down zooms out.
//ESC displays the splash screen.
//Changed reset ball key to r

//---------------------------------------------------------------------------------------------------------

#define MAXCOL 512
#define MAXROW 256 //handles a 512x256 image.

//---------------------------------------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <GL/freeglut.h>

//--------------------------------------------------------------------------------------------------------

#pragma warning(disable : 4996) //disables the "sprintf" and fopen warnings because they're a pain

//--------------------------------------------------------------------------------------------------------

//definitions for the image stuff

typedef unsigned char pixel;	//one handle for each pixel
typedef char name[15];
name image_file_name;

pixel image_buf[MAXROW*MAXCOL];		//image buffer
pixel image[MAXROW][MAXCOL] ;		//image array
pixel p;
name inf_name;		//filename
FILE *inf_handle;	//file handle
int charin;
int r,c;

//-------------------------------------------------------------------------------------------------------

//definitions for the Pong

//defines C-style struct for the paddle
typedef struct {
	GLfloat position[3];			//x, y and z coordinates
	GLfloat dimensions[3];			//width, height, thickness
	GLfloat colour[3];				//RGB
	GLint score;					//scores
} paddle;	


//defines C-style struct for the ball
typedef struct {
	GLfloat position[3];			//x, y and z coordinates
	GLfloat rotation[3];			//rotation of the ball
	GLfloat transformation[6];		//the change in position of the ball.
	GLfloat rad;					//radius of circles and stuff
	GLfloat colour[3];				//RGB
} ball_struct;

//Vertices
static GLfloat vertices[][3] =	{{-1.0f,-1.0f,-1.0f},{1.0f,-1.0f,-1.0f}, {1.0f,1.0f,-1.0f},{-1.0f,1.0f,-1.0f}, {-1.0f,-1.0f,1.0f},{1.0f,-1.0f,1.0f}, {1.0f,1.0f,1.0f},{-1.0f,1.0f,1.0f}};

//The eye and camera position
static GLfloat eye[] = {0.0f, 0.0f, 7.0f};
static GLfloat camera[] = {0.0f, 90.0f, 0.0f};

//Initialises the winner to NULL
static paddle *winner = NULL;


//Initialises the paddle details
static paddle playerPaddle = {{-2.0f, 0.0f, 0.0f}, {0.5f, 0.2f, 0.1f}, {0.55f, 0.09f, 0.09f}, 0};
static paddle opponentPaddle = {{2.0f, 0.0f, -0.0f}, {0.5f, 0.2f, 0.1f}, {0.45f, 0.73f, 0.8f}, 0};

//Initialises the ball details
static ball_struct ball = {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f}, {0.02f, 0.006f, 0.01f, 3.14f, 14.3f, 31.4f}, 0.05f, {1.0f, 1.0f, 1.0f}};

static char paused = 0;
//static char light = 1;
static int ball_speed = (CLOCKS_PER_SEC / 100);


//Function prototypes

//For the main stuff
void initGL();
void Reshape(int w, int h);
void keys(unsigned char key, int x, int y);
void specialKeys(int value, int x, int y);
void mouseButton(int button, int state, int x, int y);
void DisplayImage(void);
void opponentMove();
void resetBallPosition();
void moveBall(void);
void polygon(int a, int b, int c , int d);
void cube();
void drawWall();
void drawPaddle();
void drawBall();
void display(void);
void mouseMove(int x, int y);
void scoreText(GLint x, GLint y, char *scoreText);


//For the splash screen
void DisplayImage(void);
void WriteCaptions(void);
void InputImage(void);
void mouseButton(int button, int state, int x, int y);

//----------------------------------------------------------------------------------------------

int main(int argc, char **argv)
{
	//Initialises GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);

	//Initialises the window for the program display
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("PONG by Team Estrogen!");
	
	initGL(); //additional initialisation

	srand(time(NULL)); //Gets a random number

	resetBallPosition(); //resets the ball position

	//All the callback functions
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	glutIdleFunc(moveBall);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);
	glutKeyboardFunc(keys);
	glutSpecialFunc(specialKeys);

	//Launches the GLUT program
	glutMainLoop();
	return 0;
}

void display(void)
{
	static char size[5];

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);
		
	glPushMatrix();
		
		glLoadIdentity();
		glTranslatef(-eye[0], -eye[1], -eye[2]);

		//This attaches lighting to the ball
		glPushMatrix();
		{
			GLfloat LightPosition[] = {0.0f, 0.0f, 0.0f, 1.0f};
			glTranslatef(ball.position[0], ball.position[1], ball.position[2]);
			glLightfv(GL_LIGHT0, GL_POSITION,LightPosition);
		}
		glPopMatrix();

		
		//Lighting is disabled for the score area
		glDisable(GL_LIGHTING);
			
			//Player's score
			glColor3f(0.5f, 0.2f, 0.1f);
			sprintf(size, "%d", playerPaddle.score);
			scoreText(0, -2, size);

			//Opponent's score
			glColor3f(0.45f, 0.73f, 0.8f);
			sprintf(size, "%d", opponentPaddle.score);
			scoreText(0, 2, size);
		
		//Re-enable lighting
		glEnable(GL_LIGHTING);

		//Rotates the camera a bit
		glRotatef(camera[0], 1.0, 0.0, 0.0);
		glRotatef(camera[1], 0.0, 1.0, 0.0);
		glRotatef(camera[2], 0.0, 0.0, 1.0);

		//Draws the ball
		glColor3f(ball.colour[0], ball.colour[1], ball.colour[2]);
		glPushMatrix();
			glTranslatef(ball.position[0], ball.position[1], ball.position[2]);
			glScalef(ball.rad, ball.rad, ball.rad);
			glRotatef(ball.rotation[0], 1.0, 0.0, 0.0);
			glRotatef(ball.rotation[0], 0.0, 1.0, 0.0);
			glRotatef(ball.rotation[0], 0.0, 0.0, 1.0);
			drawBall();
		glPopMatrix();

		//player paddle
		glColor4f(playerPaddle.colour[0], playerPaddle.colour[1], playerPaddle.colour[2], 0.5);
		glPushMatrix();
			glTranslatef(playerPaddle.position[0] + opponentPaddle.dimensions[2], playerPaddle.position[1], playerPaddle.position[2]);
			glScalef(playerPaddle.dimensions[2], playerPaddle.dimensions[1], playerPaddle.dimensions[0]);
			drawPaddle();
		glPopMatrix();

		//opponent paddle
		glColor4f(opponentPaddle.colour[0], opponentPaddle.colour[1], opponentPaddle.colour[2], 0.5);
		glPushMatrix();
			glTranslatef(opponentPaddle.position[0] - opponentPaddle.dimensions[2], opponentPaddle.position[1], opponentPaddle.position[2]);
			glScalef(opponentPaddle.dimensions[2], opponentPaddle.dimensions[1], opponentPaddle.dimensions[0]);
			drawPaddle();
		glPopMatrix();

		//playing field
		//Note: the field itself consists of two sets of walls.
		glColor4f(0.49f, 0.15f, 0.80f, 0.3f);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		glPushMatrix();
			glTranslatef(-1.0, 0.0, 0.0);
			drawWall();
			glTranslatef(2.0, 0.0, 0.0);
			drawWall();
		glPopMatrix();

	glPopMatrix();

	glFlush();
	glutSwapBuffers();
}

void Reshape(int w, int h)
{
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0, (float)(w/h), 1.0, 300.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void initGL()
{
	//Two types of lighting
	GLfloat ambientLight[]  = {0.2f, 0.2f, 0.2f, 1.0f};
	GLfloat diffuseLight[]  = {0.5f, 0.5f, 0.5f, 0.5f};

	//Positions the light
	GLfloat LightPosition[] = {-eye[0], -eye[1] + (GLfloat) 2.0, -eye[2], 1.0f};

	//Shading
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);

	
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);

	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT,GL_NICEST);
}

void drawWall()
{
	glBegin(GL_QUADS);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);

		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);

		glNormal3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);

		glNormal3f(0.0f, 0.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
	glEnd();
}

void drawPaddle() //draws the player's paddle
{
	glPushMatrix();
		cube();
	glPopMatrix();
}

void drawBall() //draws the ball, a simple sphere
{
	glPushMatrix();
		glutSolidSphere(1.5, 7, 7);
	glPopMatrix();
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

void polygon(int a, int b, int c , int d)
{
	glBegin(GL_POLYGON);
		glVertex3fv(vertices[a]);
		glVertex3fv(vertices[b]);
		glVertex3fv(vertices[c]);
		glVertex3fv(vertices[d]);
	glEnd();
}

void scoreText(GLint x, GLint y, char *scoreText)
{
	char *s;

	glRasterPos2i(x, y);

	for (s = scoreText; *s; s++)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *s);
}

void opponentMove() //makes the CPU paddle move around after the ball
{
	//Note: The CPU paddle only starts moving when the ball is headed its way.
	static GLfloat dy = 0.03f; 
	static GLfloat dz = 0.03f;

	if (ball.transformation[0] < 0.0)
		return;
	
	if (opponentPaddle.position[1] < ball.position[1] && opponentPaddle.position[1] + opponentPaddle.dimensions[1] + dy <= 1.0)
		opponentPaddle.position[1] += dy;
	else if (opponentPaddle.position[1] > ball.position[1] && opponentPaddle.position[1] - opponentPaddle.dimensions[1] - dy >= -1.0)
		opponentPaddle.position[1] -= dy;
	
	if (opponentPaddle.position[2] < ball.position[2] && opponentPaddle.position[2] + opponentPaddle.dimensions[0] + dz <= 1.0)
		opponentPaddle.position[2] += dz;
	else if (opponentPaddle.position[2] > ball.position[2] && opponentPaddle.position[2] - opponentPaddle.dimensions[0] - dz >= -1.0)
		opponentPaddle.position[2] -= dz;
}

void moveBall(void)		//Here's where the fun of pong happens: collisions galore!
{
	static clock_t nextTime = 0;
	clock_t currentTime = clock();

	int i;

	if (nextTime >= currentTime || paused)
		return;

	opponentMove();

	for(i = 0; i < 3; i++)
	{
		ball.position[i] += ball.transformation[i];
	}

	//Bounces against the wall!
	if (ball.position[1] <= (-1.0 + ball.rad) || ball.position[1] >= (1.0 - ball.rad))
		ball.transformation[1] = -ball.transformation[1];
	
	if (ball.position[2] >= (1.0 - ball.rad)  || ball.position[2] <= (-1.0 + ball.rad))
		ball.transformation[2] = -ball.transformation[2];

		//Ball bounces against the paddle!
	    if (ball.transformation[0] < 0.0
		&& (ball.position[0] < -2.0 + ball.rad + playerPaddle.dimensions[2])
		&& (ball.position[1] >= (playerPaddle.position[1] - playerPaddle.dimensions[1]))
		&& (ball.position[1] <= (playerPaddle.position[1] + playerPaddle.dimensions[1]))
		&& (ball.position[2] >= (playerPaddle.position[2] - playerPaddle.dimensions[0]))
		&& (ball.position[2] <= (playerPaddle.position[2] + playerPaddle.dimensions[0])))
		{
			//changes the colour of the ball to the colour of the player's paddle
			for(i = 0; i < 3; i ++)
			{
				ball.colour[i] = playerPaddle.colour[i];
			}


			//player's paddle hits the ball
			ball.transformation[0] = -ball.transformation[0];

		
			if	(ball.position[1] > playerPaddle.position[1] + playerPaddle.dimensions[1] / 2.0
			  && ball.position[1] <= playerPaddle.position[1] + playerPaddle.dimensions[1] / 2.0
			  && ball.position[2] >= playerPaddle.position[2] - playerPaddle.dimensions[0] / 2.0
			  && ball.position[2] <= playerPaddle.position[2] + playerPaddle.dimensions[0] / 2.0) 
			{
					ball.transformation[0] *= 1.5;  //speeds up the ball
			} 
			else 
			{
				if (ball.transformation[1] > 0.0f) 
				{
					if (ball.position[1] > playerPaddle.position[1])
						ball.transformation[1] += (GLfloat) 0.005;
					else			
						ball.transformation[1] = (GLfloat) 0.005 - ball.transformation[1];
				} 
				else 
				{
					if (ball.position[1] < playerPaddle.position[1])
						ball.transformation[1] -= (GLfloat) 0.005;
					else
						ball.transformation[1] = -ball.transformation[1] - (GLfloat) 0.005;
				}


				if (ball.transformation[2] > 0.0) 
				{
					if (ball.position[2] > playerPaddle.position[2])
						ball.transformation[2] += (GLfloat) 0.005;
					else
						ball.transformation[2] = (GLfloat) 0.005 - ball.transformation[2];
				} 
				else
				{
					if (ball.position[2] < playerPaddle.position[2])
						ball.transformation[2] -= (GLfloat) 0.005;
					else
						ball.transformation[2] = -ball.transformation[2] - (GLfloat) 0.005;
				}//end-nested-if
			}//endif
		}//end-big-if

	//The same thing happens with the opponent's paddle
	if (ball.position[0] > 2.0 - ball.rad - opponentPaddle.dimensions[2]
		&& ball.transformation[0] > 0.0
		&& ball.position[1] >= opponentPaddle.position[1] - opponentPaddle.dimensions[1]
		&& ball.position[1] <= opponentPaddle.position[1] + opponentPaddle.dimensions[1]
		&& ball.position[2] >= opponentPaddle.position[2] - opponentPaddle.dimensions[0]
		&& ball.position[2] <= opponentPaddle.position[2] + opponentPaddle.dimensions[0]
	) {

		ball.transformation[0] = -ball.transformation[0];

		for(i = 0; i < 3; i++)
		{
			ball.colour[i] = opponentPaddle.colour[i];
		}

		if (ball.position[1] > opponentPaddle.position[1] + opponentPaddle.dimensions[1] / 2.0
			&& ball.position[1] <= opponentPaddle.position[1] + opponentPaddle.dimensions[1] / 2.0
			&& ball.position[2] >= opponentPaddle.position[2] - opponentPaddle.dimensions[0] / 2.0
	                && ball.position[2] <= opponentPaddle.position[2] + opponentPaddle.dimensions[0] / 2.0) {

			ball.transformation[0] *= 1.5;
		} else {

			if (ball.transformation[1] > 0.0) {
				if (ball.position[1] > opponentPaddle.position[1])
					ball.transformation[1] += (GLfloat) 0.005;
				else
					ball.transformation[1] = ((GLfloat) 0.005) - ball.transformation[1];
			} else {
				if (ball.position[1] < opponentPaddle.position[1])
					ball.transformation[1] -= (GLfloat) 0.005;
				else
					ball.transformation[1] = -ball.transformation[1] - ((GLfloat) 0.005);
			}
			
			if (ball.transformation[2] > 0.0) {
				if (ball.position[2] > opponentPaddle.position[2])
					ball.transformation[2] += (GLfloat) 0.005;
				else
					ball.transformation[2] = (GLfloat) 0.005 - ball.transformation[2];
			} else {
				if (ball.position[2] < opponentPaddle.position[2])
					ball.transformation[2] -= (GLfloat) 0.005;
				else
					ball.transformation[2] = -ball.transformation[2] - (GLfloat) 0.005;
			}
		}
	}
	

	//Sets the winner
	if (ball.position[0] > 2.0 - ball.rad) {
		winner = &playerPaddle;
		playerPaddle.score++;
		resetBallPosition();
	}
	else if (ball.position[0] < -2.0 + ball.rad) {
		winner = &opponentPaddle;
		opponentPaddle.score++;
		resetBallPosition();
	}

	glutPostRedisplay();
}

GLfloat randomPos()
{
	return (GLfloat) ((rand() % 200) / 200.0);
}

void resetBallPosition()
{
	int i;

	if (winner == &playerPaddle) {
		ball.position[0] = playerPaddle.position[0] + playerPaddle.dimensions[2];
		ball.position[1] = playerPaddle.position[1];
		ball.position[2] = playerPaddle.position[2];
		
		for(i = 0; i < 3; i++)
		{
			ball.transformation[i] = (GLfloat) (randomPos() / 20.0 + 0.005);
		}

		if (ball.transformation[0] < 0.0)
			ball.transformation[0] = -ball.transformation[0];
		if (rand() & 1)
			ball.transformation[1] = -ball.transformation[1];
		if (rand() & 1)
			ball.transformation[2] = -ball.transformation[2];
	} 
	else if(winner == &opponentPaddle)
	{
		ball.position[0] = opponentPaddle.position[0] - opponentPaddle.dimensions[2];
		ball.position[1] = opponentPaddle.position[1];
		ball.position[2] = opponentPaddle.position[2];
		
		ball.transformation[0] = randomPos() / (GLfloat) (20.0 + 0.01);

		for(i = 1; i < 3; i++)
		{
			ball.transformation[i] = randomPos() / (GLfloat) (20.0 + 0.005);
		}

		if (ball.transformation[0] > 0.0)
			ball.transformation[0] = -ball.transformation[0];

		if (rand() & 1)
			ball.transformation[1] = -ball.transformation[1];

		if (rand() & 1)
			ball.transformation[2] = -ball.transformation[2];
	}
	
	for(i = 0; i < 3; i++)
	{
		ball.colour[i] = 1.0f;
	}
}

void mouseMove(int x, int y)
{
	static GLint tmp_x = 0;
	static GLint tmp_y = 0;

	if (paused)
		return;

	//paddle go up
	if (tmp_y > y && playerPaddle.position[1] < 1.0 - playerPaddle.dimensions[1] - 0.01)
		playerPaddle.position[1] += (GLfloat) 0.05;
	//paddle go down
	if (tmp_y < y && playerPaddle.position[1] > -1.0 + playerPaddle.dimensions[1] + 0.01)
		playerPaddle.position[1] -= (GLfloat) 0.05;

	//paddle to the left, to the left
	if (tmp_x > x && playerPaddle.position[2] > -1.0 + playerPaddle.dimensions[0] + 0.01)
		playerPaddle.position[2] -= (GLfloat) 0.05;
	//paddle to the right
	if (tmp_x < x && playerPaddle.position[2] < 1.0 - playerPaddle.dimensions[0] - 0.01)
		playerPaddle.position[2] += (GLfloat) 0.05;

	tmp_x = x;
	tmp_y = y;

	glutPostRedisplay();
}

void keys(unsigned char key, int x, int y)
{
	switch (key) {
		case ' ':		//spacebar key
			paused = !paused;
			break;
		case '+':		//increase ball speed
			ball_speed /= 2;
			break;
		case '-':		//decrease ball speed
			ball_speed *= 2;
			if (ball_speed == 0)
				ball_speed = 1;
			break;
		case 'r':
			resetBallPosition();
			break;
		case 27:
			{
				glutIdleFunc(DisplayImage);

				glutMouseFunc(mouseButton);
			}
			break;
		default:
			break;
	}

   glutPostRedisplay();
}

void specialKeys(int value, int x, int y)
{
    switch (value)
    {
        case GLUT_KEY_F1:
			glutFullScreen();
			break;
		case GLUT_KEY_F2:
			glutPositionWindow(100, 100);
			glutReshapeWindow(600, 600);
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
		case GLUT_KEY_PAGE_UP:
			eye[2] -= 1.0;
			break;
		case GLUT_KEY_PAGE_DOWN:
			eye[2] += 1.0;
			break;
    }

	glutPostRedisplay();
}


void InputImage(void)
{
	inf_handle = fopen("starks.raw", "rb"); //reads the file in binary mode
    
    //reads the 
    for ( r = 0; r < MAXROW; r++ )
        for ( c = 0; c < MAXCOL; c++) {
            charin=fgetc(inf_handle);
            image[r][c] = charin;
        }

    fclose(inf_handle); /* close input file */
}

void WriteCaptions(void)
{
    int i, j;
    char caption1[] = "SHER DODANI";
	char caption2[] = "HANAII PARKER";
    glColor3f(1.0, 1.0, 1.0);

    glRasterPos2i(75, 250);
    for (i=0; i< sizeof(caption1) ; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, caption1[i]);

	glRasterPos2i(325, 250);
	for (j=0; j< sizeof(caption2) ; j++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, caption2[j]);
}

void DisplayImage(void)
{
    int offset;

	InputImage();
	
    offset = 0;
    for ( r = MAXROW-1; r >= 0; r-- ) {
        for ( c = 0; c < MAXCOL; c++) {
            image_buf[MAXCOL*offset + c] = image[r][c];
        }
        offset++;
    }
		
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //set up an orthographic projection in 2D with a 600x600 viewing window
    gluOrtho2D(0.0,600.0,0.0,600.0);

    glMatrixMode(GL_MODELVIEW);
    WriteCaptions();
    /* set raster position for displaying image in graphics image buffer*/
    glRasterPos2i(40, 300); // (x,y)
    /* load graphics image buffer with image from your own image buffer */
    glDrawPixels(MAXCOL, MAXROW, GL_LUMINANCE, GL_UNSIGNED_BYTE, image_buf);
    glutSwapBuffers();
}

void mouseButton(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON)
	{
		if(state == GLUT_DOWN)
		{
			glutLeaveMainLoop();
		}
	}
}
